#include "umepch.h"
#include "RayTracingLayer.h"
#include "imgui/imgui.h"
#include "glm/gtc/type_ptr.hpp"

static std::vector<Ume::Vertex> cubeVertices = {
		 {-0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  0.0f, 0.0f},
		 { 0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  1.0f, 0.0f},
		 { 0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  1.0f, 1.0f},
		 {-0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  0.0f, 1.0f},

		 {-0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  1.0f, 0.0f},
		 { 0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 0.0f},
		 { 0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 1.0f},
		 {-0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  1.0f, 1.0f},

		 {-0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 0.0f},
		 {-0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f},
		 {-0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 1.0f},
		 {-0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f},

		 { 0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  1.0f, 0.0f},
		 { 0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 0.0f},
		 { 0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f},
		 { 0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  1.0f, 1.0f},

		 {-0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  0.0f, 0.0f},
		 { 0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  1.0f, 0.0f},
		 { 0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  1.0f, 1.0f},
		 {-0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  0.0f, 1.0f},

		 {-0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  0.0f, 0.0f},
		 { 0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  1.0f, 0.0f},
		 { 0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,  1.0f, 1.0f},
		 {-0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  1.0f,  0.0f, 1.0f}
};
static std::vector<uint32_t> cubeIndices = {
	0,  1,  2 , 2,  3,  0 ,
	4,  5,  6 , 6,  7,  4 ,
	8,  9,  10, 10, 11, 8 ,
	12, 13, 14, 14, 15, 12,
	16, 17, 18, 18, 19, 16,
	20, 21, 22, 22, 23, 20
};

static Ume::PolygonMode polygonMode = Ume::PolygonMode::Fill;

static bool rendering = false;

namespace Ume
{
	static Timestep timestep = 0.0f;

	constexpr int N = 4;

	static Ref<RTCamera> camera = nullptr;
	static EditorCameraController controller;

	// Begin::Gama 
	static bool useGamma = true;
	static bool useHDR = true;
	static float exposure = 3.0f;
	static bool lockCamera = true;
	static int maxSPP = 2048;
	static Ref<Framebuffer> editorFramebuffer;
	static Ref<Framebuffer> editorPostFramebuffer;
	static Ref<Framebuffer> renderPostFramebuffer;
	static Ref<Framebuffer> gbufferFramebuffer;
	static Ref<VertexArray> quadVA;
	static Ref<Shader> postShader;
	static Ref<Shader> gbufferShader;
	static Ref<Shader> skyboxShader;
	// End:: Gama
	static std::vector<glm::vec3> ssaoKernel;

	static Ref<VertexArray> cube;
	static Ref<Shader> pbrShader;
	static Ref<Shader> pbrDeferShader;
	static Ref<Shader> ssaoShader;
	static Ref<VertexArray> sphere;

	static Ref<Framebuffer> selectFramebuffer;
	static Ref<Shader>	selectShader;

	static Ref<Texture2D> t_Irradiance;
	static Ref<Texture2D> t_BRDFlut;
	static Ref<Texture2D> t_Noise;
	static float ssaoRadius = 10.0f;
	static float minRoughness = 0.015f;


	RayTracingLayer::RayTracingLayer()
		: Layer("RayTracing")
	{
		m_Scene = CreateRef<Scene>();
		m_ECS = CreateRef<ECS>();

		m_SceneViewport.SetCallback(Viewport::CallbackType::Render, [&](Viewport* viewport, void* data)
		{
			ImGui::Image((void*)editorPostFramebuffer->GetColorAttachments()[0]->GetRendererID(), *(ImVec2*)data, { 0, 1 }, { 1, 0 });
		});

		m_SceneViewport.SetCallback(Viewport::CallbackType::Resize, [&](Viewport* viewport, void* data)
		{
			auto& size = viewport->GetSize();
			camera->Resize(size.x, size.y);
			RTRenderer::Resize(size.x, size.y);
			editorFramebuffer->Resize(size.x, size.y);
			editorPostFramebuffer->Resize(size.x, size.y);
			gbufferFramebuffer->Resize(size.x, size.y);
		});

		m_SceneViewport.SetCallback(Viewport::CallbackType::MouseButtonPress, [&](Viewport* viewport, void* data)
		{
			auto info = *(Viewport::MouseClickInfo*)data;
			if (info.Buttons & MouseButtonFlagBits::Left)
				m_Scene->SelectObject(info.Position);
		});

		m_RenderViewport.SetCallback(Viewport::CallbackType::Render, [&](Viewport* viewport, void* data)
		{
			ImGui::Image((void*)renderPostFramebuffer->GetColorAttachments()[0]->GetRendererID(), *(ImVec2*)data, { 0, 1 }, { 1, 0 });
		});

		m_RenderViewport.SetCallback(Viewport::CallbackType::Resize, [&](Viewport* viewport, void* data)
		{
			auto& size = viewport->GetSize();
			camera->Resize(size.x, size.y);
			RTRenderer::Resize(size.x, size.y);
			RTRenderer::Clear();
			renderPostFramebuffer->Resize(size.x, size.y);
		});

		m_RenderViewport.SetCallback(Viewport::CallbackType::MouseButtonPress, [&](Viewport* viewport, void* data)
		{
			auto info = *(Viewport::MouseClickInfo*)data;
			if (info.Buttons & MouseButtonFlagBits::Left)
				m_Scene->SelectObject(info.Position);
		});

		auto& window = Application::Get().GetWindow();
		window.SetVSync(false);
		camera = CreateRef<RTCamera>(45.0f, window.GetWidth(), window.GetHeight(), 0.1f, 500.0f);
		controller = { camera };
		camera->Position = { 0.0f, 0.5f, 6.0f };
		//camera->Position = { 0.0f, 2.0f, 6.0f };
		camera->Pitch = 0.0f;
		camera->Recalculate();

		m_Scene->Cameras.push_back(camera);
		m_Scene->ActivedCamera = camera;
		m_Scene->Config.Ambient = 0.2f;

		TextureSpecification sp;
		sp.SRGB = true;
		sp.Filter = TextureFilter::Mipmap;
		sp.GenMips = true;
		auto radiance = AssetManager::LoadTexture("assets/textures/ibl_hdr_radiance.png", sp);
		t_Irradiance = PreCompute::GetIrradiance(radiance);
		{
			TextureSpecification sp;
			sp.Format = ImageFormat::RGB16F;
			t_BRDFlut = AssetManager::LoadTexture("assets/textures/lut/ibl_brdf_lut.png", sp);
		}
		m_Scene->SetEnvTexture(radiance);

		RTRenderer::Init(window.GetWidth(), window.GetHeight());
		RTRenderer::Config.PickLight = LightPickType::All;
		RTRenderer::Config.wp = 32.0f;
		RTRenderer::Config.wn = .1f;
		RTRenderer::Config.wc = .6f;
		RTRenderer::Config.wpl = .1f;
	}

	void RayTracingLayer::OnAttach()
	{
		CreateScene();
		CreateRenderPasses();
	}

	void RayTracingLayer::CreateScene()
	{
		// Textures
		AssetManager::GetMaterial(0).UseAlbedoTexture = true;
		AssetManager::GetMaterial(0).AlbedoTexture = AssetManager::DefaultTexture();

		// Materials
		Material* goldMaterial = new Material;
		goldMaterial->Albedo = glm::vec3(1.0f, 0.843f, 0.0f);
		goldMaterial->Roughness = 0.5f;
		goldMaterial->Metallic = 1.0f;
		goldMaterial->Emissive = false;

		Material* plasticMaterial = new Material;
		plasticMaterial->Albedo = glm::vec3(1.0f, 1.0f, 1.0f);
		plasticMaterial->Roughness = 0.015f;
		plasticMaterial->Metallic = 0.0f;
		plasticMaterial->Emissive = false;

		Material* pointEmissiveMaterial = new Material;
		pointEmissiveMaterial->Albedo = glm::vec3(1.0f, 0.9f, 0.9f);
		pointEmissiveMaterial->Roughness = 1.0f;
		pointEmissiveMaterial->Metallic = 0.0f;
		pointEmissiveMaterial->Emissive = true;
		{
			Emission* emission = new Emission;
			emission->Color = { 1.0f, 0.5f, 0.0f };
			emission->Color = { 1.0f, 1.0f, 1.0f };
			emission->Intensity = 1.0f;
			pointEmissiveMaterial->Emission = emission;
		}

		Material* directionalEmissiveMaterial = new Material;
		directionalEmissiveMaterial->Albedo = glm::vec3(1.0f, 0.9f, 0.9f);
		directionalEmissiveMaterial->Roughness = 0.2f;
		directionalEmissiveMaterial->Metallic = 0.0f;
		directionalEmissiveMaterial->Emissive = true;
		{
			Emission* emission = new Emission;
			emission->Color = { 1.0f, 1.0f, 1.0f };
			emission->Intensity = 1.0f;
			emission->Type = LightType::Directional;
			directionalEmissiveMaterial->Emission = emission;
		}

		Material* redWall = new Material;
		redWall->Albedo = glm::vec3(1.0f, 0.0f, 0.0f);
		redWall->Roughness = 0.5f;
		redWall->Metallic = 0.0f;
		redWall->Emissive = false;

		Material* greenWall = new Material;
		greenWall->Albedo = glm::vec3(0.0f, 1.0f, 0.0f);
		greenWall->Roughness = 0.5f;
		greenWall->Metallic = 0.0f;
		greenWall->Emissive = false;

		AssetManager::StoreMaterial(pointEmissiveMaterial);
		AssetManager::StoreMaterial(directionalEmissiveMaterial);
		AssetManager::StoreMaterial(goldMaterial);
		AssetManager::StoreMaterial(plasticMaterial);
		AssetManager::StoreMaterial(redWall);
		AssetManager::StoreMaterial(greenWall);

		auto sphere1 = Object::Create("Sphere1", ObjectType::Sphere);
		sphere = sphere1->GetVertexArray();
		sphere1->SetTransform(Transform({ 0.5f, 0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f }));
		sphere1->SetMaterial(plasticMaterial);

		auto sphere2 = Object::Create("Sphere2", ObjectType::Sphere);
		sphere2->SetTransform(Transform({ -0.5f, -0.5f, 0.0f }));
		sphere2->SetMaterial(goldMaterial);

		auto light = Object::Create("AreaLight", ObjectType::Quad);
		light->SetMaterial(pointEmissiveMaterial);
		light->SetTransform(Transform({ 0.0f, 2.1499f, 0.0f }, { 0.5f, 0.5f, 0.5f }, { 90.0f, 0.0f, 0.0f }));

		auto ceil = Object::Create("Ceil", ObjectType::Cube);
		ceil->SetMaterial(&AssetManager::GetMaterial(0));
		ceil->SetMaterial(new Material);
		ceil->SetTransform(Transform({ 0.0f, 2.2f, 0.0f }, { 2.5f, 0.1f, 2.5f }));
			
		auto floor = Object::Create("Floor", ObjectType::Cube);
		floor->SetMaterial(&AssetManager::GetMaterial(0));
		floor->SetMaterial(new Material);
		floor->SetTransform({});
		//floor->SetTransform(Transform({ 0.0f, 0.0f, 0.0f }, { 2.0f, 1.0f, 1.0f }));
		floor->SetTransform(Transform({ 0.0f, -1.2f, 0.0f }, { 2.5f, 0.1f, 2.5f }));

		auto wallL = Object::Create("Wall_Left", ObjectType::Cube);
		wallL->SetMaterial(redWall);
		wallL->SetTransform(Transform({ -1.2f, 0.45f, 0.0f }, { 0.1f, 3.4f, 2.5f }));
			
		auto wallR = Object::Create("Wall_Right", ObjectType::Cube);
		wallR->SetMaterial(greenWall);
		wallR->SetTransform(Transform({ 1.2f, 0.5f, 0.0f }, { 0.1f, 3.3f, 2.5f }));
			
		auto wallB = Object::Create("Wall_Back", ObjectType::Cube);
		wallB->SetMaterial(new Material);
		wallB->SetTransform(Transform({ 0.0f, 0.5f, -1.2f }, { 2.5f, 3.3f, 0.1f }));

		auto miku = Object::Create("Miku", ObjectType::Quad);
		miku->SetMaterial(new Material);
		miku->Material->UseAlbedoTexture = true;
		TextureSpecification sp;
		sp.SRGB = true;
		miku->Material->AlbedoTexture = AssetManager::LoadTexture("assets/textures/Miku.jpg", sp);
		miku->SetTransform(Transform({ 0.0f, 1.0f, -1.14f }));

		m_Scene->AddObject(light);
		m_Scene->AddObject(sphere1);
		m_Scene->AddObject(sphere2);
		m_Scene->AddObject(floor);
		m_Scene->AddObject(ceil);
		m_Scene->AddObject(wallL);
		m_Scene->AddObject(wallR);
		m_Scene->AddObject(wallB);
		m_Scene->AddObject(miku);

		m_Scene->BuildBVH();

		RTRenderer::Submit(m_Scene);
		m_Scene->Config.Ambient = 0.5f;
		RTRenderer::Config.RR = 0.8f;
		exposure = 2.7f;
	}

	void RayTracingLayer::CreateRenderPasses()
	{
		auto& window = Application::Get().GetWindow();
		{
			TextureSpecification colorInfo = { ImageFormat::RGBA16F };
			TextureSpecification flags = { ImageFormat::RGBA };
			FramebufferDescription description;
			description.Width = window.GetWidth();
			description.Height = window.GetHeight();
			description.ColorAttachments = { colorInfo };
			renderPostFramebuffer = Framebuffer::Create(description);
			editorPostFramebuffer = Framebuffer::Create(description);
			editorFramebuffer = Framebuffer::Create(description);
			selectFramebuffer = Framebuffer::Create(description);
			description.ColorAttachments = { colorInfo, colorInfo, colorInfo };
			gbufferFramebuffer = Framebuffer::Create(description);

			quadVA = VertexArray::Create(ObjectType::Quad);
			postShader = Shader::Create("assets/shaders/PostProcess.glsl");
			gbufferShader = Shader::Create("assets/shaders/Gbuffer_Defer.glsl");
			skyboxShader = Shader::Create("assets/shaders/Skybox.glsl");
			selectShader = Shader::Create("assets/shaders/Select.glsl");

			cube = VertexArray::Create(ObjectType::Cube);
			pbrShader = Shader::Create("assets/shaders/PBR.glsl");
			pbrDeferShader = Shader::Create("assets/shaders/PBR_Defer.glsl");
			ssaoShader = Shader::Create("assets/shaders/SSAO.glsl");

			uint8_t kernelSize = 64;
			ssaoKernel.reserve(kernelSize);
			auto Lerp = [](float a, float b, float f) { return a + f * (b - a); };
			for (int i = 0; i < kernelSize; i++)
			{
				auto sample = glm::normalize(glm::vec3(Random::Float() * 2.0f - 1.0f, Random::Float() * 2.0f - 1.0f, Random::Float()));
				float scale = float(i) / kernelSize;
				scale = Lerp(0.1, 1.0, scale * scale);
				ssaoKernel.push_back(sample * scale);
			}

			int size = 4;
			std::vector<glm::vec3> ssaoNoise(size * size);
			for (int i = 0; i < size * size; i++)
				ssaoNoise[i] = { Random::Float() * 2.0f - 1.0f, Random::Float() * 2.0f - 1.0f, 0.0f };

			TextureSpecification sp;
			sp.Format = ImageFormat::RGB16F;
			t_Noise = Texture2D::Create(size, size, sp);
			t_Noise->SetData(ssaoNoise.data(), sizeof(glm::vec3) * ssaoNoise.size());

		}
	}

	void RayTracingLayer::OnUpdate(Timestep ts)
	{
		timestep = ts;

		if (!lockCamera || lockCamera && !rendering)
		{
			controller.Update(ts);
		}

		auto width = editorFramebuffer->GetColorAttachments()[0]->GetWidth();
		auto height = editorFramebuffer->GetColorAttachments()[0]->GetHeight();

		RTRenderer::GBufferPass(camera->GetViewProjection());
		// ssao pass
		RTRenderer::GetGBuffer()->Bind();
		{
			ssaoShader->Bind();
			ssaoShader->SetTexture("Position", RTRenderer::GetGBuffer()->GetColorAttachments()[0], 1);
			ssaoShader->SetTexture("Normal", RTRenderer::GetGBuffer()->GetColorAttachments()[1], 2);
			ssaoShader->SetTexture("Albedo", RTRenderer::GetGBuffer()->GetColorAttachments()[3], 3);
			ssaoShader->SetTexture("FlagBits", RTRenderer::GetGBuffer()->GetColorAttachments()[2], 6);
			ssaoShader->SetTexture("u_NoiseTexture", t_Noise, 7);
			ssaoShader->SetMat4("u_ViewProjection", camera->GetViewProjection());
			ssaoShader->SetInt("u_Width", width);
			ssaoShader->SetInt("u_Height", height);
			ssaoShader->SetFloat("radius", ssaoRadius);
			ssaoShader->SetFloat3Array("u_Samples", ssaoKernel);
			Renderer::DrawQuad();
		}
		RTRenderer::GetGBuffer()->Unbind();

		if (rendering)
		{
			RTRenderer::Render(ts);
			if (RTRenderer::Frame >= maxSPP) rendering = false;
		}
		else
		{
			// pbr pass (defer)
			editorFramebuffer->Bind();
			{
				RenderCommand::SetClearColor(0.2f, 0.2f, 0.2f, 0.0f);
				RenderCommand::Clear();
				Renderer::BeginScene(camera->GetViewProjection());

				RenderCommand::SetDepthTestFunc(DepthTestFunc::Lequal);
				skyboxShader->Bind();
				skyboxShader->SetMat4("u_ViewProjection", camera->GetViewProjection());
				skyboxShader->SetFloat3("u_CameraPosition", camera->Position);
				skyboxShader->SetTexture("u_EnvironmentMap", m_Scene->GetEnvTexture(), 9);
				skyboxShader->SetFloat("u_Ambient", m_Scene->Config.Ambient);
				Renderer::Submit(cube);
				skyboxShader->Unbind();
				RenderCommand::SetDepthTestFunc(DepthTestFunc::Less);

				pbrDeferShader->Bind();
				pbrDeferShader->SetTexture("Position", RTRenderer::GetGBuffer()->GetColorAttachments()[0], 1);
				pbrDeferShader->SetTexture("Normal", RTRenderer::GetGBuffer()->GetColorAttachments()[1], 2);
				pbrDeferShader->SetTexture("Albedo", RTRenderer::GetGBuffer()->GetColorAttachments()[3], 3);
				pbrDeferShader->SetTexture("MaterialParams", RTRenderer::GetGBuffer()->GetColorAttachments()[5], 4);
				pbrDeferShader->SetTexture("Emission", RTRenderer::GetGBuffer()->GetColorAttachments()[4], 5);
				pbrDeferShader->SetTexture("FlagBits", RTRenderer::GetGBuffer()->GetColorAttachments()[2], 6);
				pbrDeferShader->SetFloat3("u_CameraPosition", camera->Position);
				pbrDeferShader->SetFloat("u_EnvIntensity", m_Scene->Config.Ambient);
				pbrDeferShader->SetTexture("u_Radiance", m_Scene->GetEnvTexture(), 7);
				pbrDeferShader->SetTexture("u_Irradiance", t_Irradiance, 8);
				pbrDeferShader->SetTexture("u_BRDFLUT", t_BRDFlut, 9);
				pbrDeferShader->SetInt("u_NumLight", (int)m_Scene->Lights.size());
				pbrDeferShader->SetInt("u_Width", width);
				pbrDeferShader->SetInt("u_Height", height);
				for (int i = 0; i < (int)m_Scene->Lights.size(); i++)
				{
					auto& light = m_Scene->Lights[i];
					if (!light->Active()) continue;
					auto str_i = std::to_string(i);
					pbrDeferShader->SetFloat3("u_Lights[" + str_i + "].Position", light->Transform.Position);
					pbrDeferShader->SetFloat3("u_Lights[" + str_i + "].Color", light->Material->Emission->Color);
					pbrDeferShader->SetFloat("u_Lights[" + str_i + "].Intensity", light->Material->Emission->Intensity);
				}
				Renderer::DrawQuad();
				Renderer::EndScene();
			}
			editorFramebuffer->Unbind();

			// pbr pass (forward)
			// editorFramebuffer->Bind();
			//{
			//	RenderCommand::SetClearColor(0.2f, 0.2f, 0.2f, 0.0f);
			//	RenderCommand::Clear();
			//	Renderer::BeginScene(camera->GetViewProjection());
			//	{
			//		pbrShader->Bind();
			//		pbrShader->SetMat4("u_ViewProjection", camera->GetViewProjection());
			//		pbrShader->SetFloat3("u_CameraPosition", camera->Position);
			//		for (int i = 0; i < (int)m_Scene->Lights.size(); i++)
			//		{
			//			auto& light = m_Scene->Lights[i];
			//			if (!light->Active()) continue;
			//			auto str_i = std::to_string(i);
			//			pbrShader->SetFloat3("u_Lights[" + str_i + "].Position", light->Transform.Position);
			//			pbrShader->SetFloat3("u_Lights[" + str_i + "].Color", light->Material->Emission->Color);
			//			pbrShader->SetFloat("u_Lights[" + str_i + "].Intensity", light->Material->Emission->Intensity);
			//		}
			//		pbrShader->SetInt("u_NumLight", (int)m_Scene->Lights.size());
			//		int lightIndex = 0;
			//		for (const auto& object : m_Scene->Objects)
			//		{
			//			if (!object->Active()) continue;
			//			pbrShader->SetInt("u_LightIndex", object->Emissive() ? lightIndex++ : -1);
			//			auto& material = object->Material;
			//			pbrShader->SetMat4("u_Model", object->Transform.ModelMatrix);
			//			pbrShader->SetMat3("u_NormalMatrix", object->Transform.NormalMatrix);
			//			pbrShader->SetFloat3("u_Material.ColorTint", material->ColorTint);
			//			pbrShader->SetFloat3("u_Material.Albedo", material->Albedo);
			//			pbrShader->SetInt("u_Material.UseAlbedoTexture", material->UseAlbedoTexture);
			//			pbrShader->SetInt("u_Material.UseRoughnessTexture", material->UseRoughnessTexture);
			//			pbrShader->SetInt("u_Material.UseMetallicTexture", material->UseMetallicTexture);
			//			pbrShader->SetInt("u_Material.UseNormalTexture", material->UseNormalTexture);
			//			pbrShader->SetFloat("u_Material.Roughness", material->Roughness);
			//			pbrShader->SetFloat("u_Material.Metallic", material->Metallic);
			//			pbrShader->SetFloat("u_EnvIntensity", m_Scene->Config.Ambient);
			//			pbrShader->SetTexture("u_Radiance", m_Scene->GetEnvTexture(), 1);
			//			pbrShader->SetTexture("u_Irradiance", t_Irradiance, 2);
			//			pbrShader->SetTexture("u_BRDFLUT", t_BRDFlut, 3);
			//			if (material->UseAlbedoTexture) pbrShader->SetTexture("u_Material.AlbedoTexture", material->AlbedoTexture, 10);
			//			if (material->UseRoughnessTexture) pbrShader->SetTexture("u_Material.RoughnessTexture", material->RoughnessTexture, 11);
			//			if (material->UseMetallicTexture) pbrShader->SetTexture("u_Material.MetallicTexture", material->MetallicTexture, 12);
			//			if (material->UseNormalTexture) pbrShader->SetTexture("u_Material.NormalTexture", material->NormalTexture, 13);
			//
			//			Renderer::Submit(object->GetVertexArray());
			//		}
			//	}
			//
			//	// environment pass
			//	{
			//		RenderCommand::SetDepthTestFunc(DepthTestFunc::Lequal);
			//		skyboxShader->Bind();
			//		skyboxShader->SetMat4("u_ViewProjection", camera->GetViewProjection());
			//		skyboxShader->SetFloat3("u_CameraPosition", camera->Position);
			//		skyboxShader->SetTexture("u_EnvironmentMap", m_Scene->GetEnvTexture(), 9);
			//		skyboxShader->SetFloat("u_Ambient", m_Scene->Config.Ambient);
			//		Renderer::Submit(cube);
			//		skyboxShader->Unbind();
			//		RenderCommand::SetDepthTestFunc(DepthTestFunc::Less);
			//	}
			//	Renderer::EndScene();
			//}
			// editorFramebuffer->Unbind();
		}

		// ray tracing post process
		{
			renderPostFramebuffer->Bind();
			RenderCommand::SetClearColor(1.0f, 0.0f, 0.0f, 1.0f);
			RenderCommand::Clear();
			postShader->Bind();
			postShader->SetTexture("u_Texture", RTRenderer::GetImage());
			postShader->SetInt("u_Gamma", (int)useGamma);
			postShader->SetInt("u_HDR", (int)useHDR);
			postShader->SetFloat("u_Exposure", exposure);
			Renderer::DrawQuad();
			renderPostFramebuffer->Unbind();
		}
		// select pass
		{
			selectFramebuffer->Bind();
			RenderCommand::SetClearColor(1.0f, 0.0f, 0.0f, 1.0f);
			RenderCommand::Clear();
			Renderer::BeginScene(camera->GetViewProjection());
			{
				selectShader->Bind();
				selectShader->SetTexture("u_ColorImage", editorFramebuffer->GetColorAttachments()[0], 0);
				selectShader->SetTexture("u_IDImage", RTRenderer::GetGBuffer()->GetColorAttachments()[2], 1);
				selectShader->SetInt("u_SelectedID", m_Scene->SelectedObject ? m_Scene->SelectedObject->ID : 0);
				selectShader->SetInt("u_Width", width);
				selectShader->SetInt("u_Height", height);
				Renderer::Submit(quadVA);
			}
			Renderer::EndScene();
			selectFramebuffer->Unbind();
		}

		// rasterization post process
		{
			editorPostFramebuffer->Bind();
			RenderCommand::SetClearColor(1.0f, 0.0f, 0.0f, 1.0f);
			RenderCommand::Clear();
			Renderer::BeginScene(camera->GetViewProjection());
			{
				postShader->Bind();
				postShader->SetTexture("u_Texture", selectFramebuffer->GetColorAttachments()[0]);
				postShader->SetInt("u_Gamma", (int)useGamma);
				postShader->SetInt("u_HDR", (int)useHDR);
				postShader->SetFloat("u_Exposure", exposure);
				Renderer::Submit(quadVA);
			}
			Renderer::EndScene();
			editorPostFramebuffer->Unbind();
		}
	}

	void RayTracingLayer::OnImGuiRender()
	{
		static bool open = true;
		GUI::ShowDockSpace(&open);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
		m_SceneViewport.Render();
		static bool rendered;
		if (rendered)
			m_RenderViewport.Render();
		ImGui::PopStyleVar();


		ImGui::Begin("Object");
		{
			for (int i = 0; i < (int)ObjectTypeNames.size(); ++i)
			{
				auto type = (ObjectType)i;
				auto name = ObjectTypeNames[i];
				ImGui::PushID(name);
				ImGui::Columns(2);
				ImGui::SetColumnWidth(0, 180);
				ImGui::Text(name);
				ImGui::NextColumn();
				ImGui::SetColumnWidth(1, 40);
				if (ImGui::Button("Add"))
				{
					static int objectID;
					auto object = Object::Create("object_" + std::to_string(objectID++), type);
					m_Scene->AddObject(object);
					m_Scene->SelectedObject = object.get();
					m_Scene->BuildBVH();
				}
				ImGui::Columns(1);
				ImGui::PopID();
			}
		}
		ImGui::End();
		ImGui::Begin("Scene");
		{
			ImGui::SetNextTreeNodeOpen(true);
			if (ImGui::TreeNodeEx("Scene", ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick))
			{
				if (ImGui::IsItemClicked(0))
				{
					m_Scene->SelectedObject = nullptr;
				}
				std::vector<uint32_t> ids;
				for (const auto& object : m_Scene->Objects)
				{
					ImGui::PushID(object->ID);
					ImGui::Columns(2);
					ImGui::SetColumnWidth(0, 150);
					if (ImGui::Button(object->Active() ? "H" : "V"))
					{
						object->Visible = !object->Visible;
					}
					ImGui::SameLine();
					if (ImGui::Selectable(object->Name.c_str()))
					{
						m_Scene->SelectedObject = object.get();
					}
					ImGui::NextColumn();
					ImGui::SetColumnWidth(1, 40);
					if (ImGui::Button("Del"))
					{
						ids.push_back(object->ID);
					}
					ImGui::Columns(1);
					ImGui::PopID();
				}
				for (auto id : ids) m_Scene->DelObject(id);
				if (!ids.empty()) m_Scene->BuildBVH();
				ImGui::TreePop();
			}
		}
		ImGui::End();


		ImGui::Begin("Properties");
		{
			auto& object = m_Scene->SelectedObject;
			if (object)
			{
				ImGui::PushID("ObjectParams");
				ImGui::Checkbox("Visible", &object->Visible);
				ImGui::Text("ID: %d", object->ID);
				auto& transform = object->Transform;
				auto& material = *object->Material;
				auto& emission = material.Emission;
				/*------------Transform-------------*/
				ImGui::Separator();
				ImGui::SetNextTreeNodeOpen(true);
				if (ImGui::TreeNode("Transform"))
				{
					bool flag = false;
					flag |= GUI::Vec3Controller("Position", transform.Position);
					flag |= GUI::Vec3Controller("Rotation", transform.EulerRotation, 1.0f);
					flag |= GUI::Vec3Controller("Scale", transform.Scale, 0.1f, 1.0f);
					if (flag)
					{
						transform.UpdateTransform();
						object->SetTransform(transform);
						m_Scene->BuildBVH();
					}
					ImGui::TreePop();
				}
				/*------------Mesh-------------*/
				ImGui::Separator();
				ImGui::SetNextTreeNodeOpen(true);
				if (ImGui::TreeNode("Mesh"))
				{
					std::string name;
					if (object->Type != ObjectType::Mesh) name = ObjectTypeNames[(int)object->Type];
					else
					{
						name = object->MeshPath;
						size_t found = name.find_last_of("/\\");
						name = found != std::string::npos ? name.substr(found + 1) : name;
					}
					ImGui::Text(name.c_str());
					if (object->Type == ObjectType::Mesh)
					{
						ImGui::SameLine();
						if (ImGui::Button("..."))
						{
							std::string filename = Application::Get().OpenFile("");
							if (filename != "")
							{
								object->ResetMesh(filename);
								m_Scene->BuildBVH();
								object->ScaleToFit(1, 1);
								m_Scene->BuildBVH();
							}
						}
					}
					ImGui::TreePop();
				}
				/*------------Material-------------*/
				ImGui::Separator();
				ImGui::SetNextTreeNodeOpen(true);
				if (ImGui::TreeNode("Light"))
				{
					bool rebuild = false;
					rebuild |= ImGui::Checkbox("Emissive", &material.Emissive);
					if (material.Emissive)
					{
						if (!material.Emission) material.Emission = new Emission;
						ImGui::SetNextTreeNodeOpen(true);
						if (ImGui::TreeNode("Light"))
						{
							ImGui::ColorEdit3("Emission", glm::value_ptr(emission->Color));
							rebuild |= ImGui::SliderFloat("Intensity", &emission->Intensity, 0.0f, 8.0f);
							//GUI::Selector<LightType>("Type", emission->Type, LightTypeNames);
							ImGui::TreePop();
						}
					}
					if (rebuild) m_Scene->BuildBVH();
					ImGui::TreePop();
				}
				ImGui::Separator();
				ImGui::SetNextTreeNodeOpen(true);
				if (ImGui::TreeNode("Albedo"))
				{
					TextureSpecification sp;
					sp.SRGB = true;
					if (GUI::ImagePicker(material.AlbedoTexture, sp)) material.UseAlbedoTexture = true;
					if (material.AlbedoTexture)
					{
						ImGui::SameLine();
						ImGui::Checkbox("Use", &material.UseAlbedoTexture);
					}
					ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo), ImGuiColorEditFlags_NoInputs);
					ImGui::SameLine();
					ImGui::ColorEdit3("ColorTint", glm::value_ptr(material.ColorTint), ImGuiColorEditFlags_NoInputs);
					ImGui::TreePop();
				}
				ImGui::Separator();
				ImGui::SetNextTreeNodeOpen(true);
				if (ImGui::TreeNode("Roughness"))
				{
					if (GUI::ImagePicker(material.RoughnessTexture)) material.UseRoughnessTexture = true;
					if (material.RoughnessTexture)
					{
						ImGui::SameLine();
						ImGui::Checkbox("Use", &material.UseRoughnessTexture);
					}
					ImGui::SliderFloat("Roughness", &material.Roughness, minRoughness, 1.0f);
					//ImGui::InputFloat("min", &minRoughness);
					ImGui::TreePop();
				}
				ImGui::Separator();
				ImGui::SetNextTreeNodeOpen(true);
				if (ImGui::TreeNode("Metallic"))
				{
					if (GUI::ImagePicker(material.MetallicTexture)) material.UseMetallicTexture = true;
					if (material.MetallicTexture)
					{
						ImGui::SameLine();
						ImGui::Checkbox("Use", &material.UseMetallicTexture);
					}
					ImGui::SliderFloat("Metallic", &material.Metallic, 0.0f, 1.0f);
					ImGui::TreePop();
				}
				ImGui::Separator();
				ImGui::SetNextTreeNodeOpen(true);
				if (ImGui::TreeNode("Normal"))
				{
					if (GUI::ImagePicker(material.NormalTexture)) material.UseNormalTexture = true;
					if (material.NormalTexture)
					{
						ImGui::SameLine();
						ImGui::Checkbox("Use", &material.UseNormalTexture);
					}
					ImGui::TreePop();
				}
				ImGui::PopID();
			}
			else
			{
				ImGui::SetNextTreeNodeOpen(true);
				if (ImGui::TreeNode("Camera"))
				{
					ImGui::SliderFloat("Rotate Speed", &camera->Sensitivity, 0.1f, 1.0f);
					ImGui::SliderFloat("Move Speed", &camera->MoveSpeed, 0.1f, 10.0f);
					ImGui::Checkbox("Lock In Render", &lockCamera);
					ImGui::TreePop();
				}
				ImGui::Separator();
				ImGui::SetNextTreeNodeOpen(true);
				if (ImGui::TreeNode("PostProcess"))
				{
					ImGui::Checkbox("Gamma", &useGamma);
					ImGui::Checkbox("HDR", &useHDR);
					ImGui::SliderFloat("Exposure", &exposure, 0.1f, 5.0f);
					ImGui::TreePop();
				}
				ImGui::Separator();
				ImGui::SetNextTreeNodeOpen(true);
				if (ImGui::TreeNode("Environment"))
				{
					TextureSpecification sp;
					sp.SRGB = true;
					sp.GenMips = true;
					sp.Filter = TextureFilter::Mipmap;
					if (GUI::ImagePicker(m_Scene->GetEnvTexture(), sp))
					{
						t_Irradiance = PreCompute::GetIrradiance(m_Scene->GetEnvTexture());
						if (!m_Scene->Config.Ambient) m_Scene->Config.Ambient = 0.3f;
					}
					ImGui::SliderFloat("Ambient", &m_Scene->Config.Ambient, 0.0f, 1.0f);
					ImGui::TreePop();
				}
				ImGui::Separator();
				ImGui::SetNextTreeNodeOpen(true);
				if (ImGui::TreeNode("Config"))
				{
					ImGui::SliderFloat("GI Strength", &RTRenderer::Config.RR, 0.1f, 0.9f);
					ImGui::DragFloat("SSAO Radius", &ssaoRadius, 0.1f, 0.0f);
					ImGui::DragInt("MaxSPP", &maxSPP, 1.0f, 1);
					//if (GUI::Selector<PolygonMode>("Polygon Mode", polygonMode, PolygonModeNames))
					//{
					//	Renderer::SetPolygonMode(polygonMode);
					//}
					ImGui::Checkbox("Antilias", &RTRenderer::Config.Antilias);
					ImGui::Checkbox("Accumulate", &RTRenderer::Config.Accumulate);
					ImGui::Checkbox("Denoise", &RTRenderer::Config.Denoise);
					ImGui::TreePop();
				}
			}
		}
		ImGui::End();

		ImGui::Begin("Infomations");
		{
			static float avg_time;
			if (rendering) avg_time = RTRenderer::Frame == 0 ? 0.0f : ((RTRenderer::Frame - 1) * avg_time + timestep.GetMilliseconds()) / RTRenderer::Frame;
			ImGui::Text("Delta Time: %.2fms", timestep.GetMilliseconds());
			ImGui::Text("Average Time: %.2fms", avg_time);
			ImGui::Text("Frame: %d", RTRenderer::Frame);
		}
		ImGui::End();

		ImGui::Begin("Operation");
		{
			std::string text = rendered ? (rendering ? "  Stop  " : "Continue") : " Start  ";
			if (ImGui::Button(text.c_str()))
			{
				rendering = !rendering;
				if (rendering) rendered = true;
			}
			ImGui::SameLine();
			if (rendered && ImGui::Button("Restart"))
			{
				RTRenderer::Clear();
				rendering = true;
			}
			ImGui::SameLine();
			if (rendered && !rendering && ImGui::Button("Export"))
			{
				static uint32_t imageIndex;
				std::string defaultName = imageIndex ? "image_" + std::to_string(imageIndex) + ".png" : "image.png";
				imageIndex++;
				auto filename = Application::Get().SaveFile({ ".png", ".jpg" }, defaultName);
				File::SaveTexture(renderPostFramebuffer->GetColorAttachments()[0], filename);
			}
		}
		ImGui::End();

		//ImGui::Begin("Denoise");
		//{
		//	//ImGui::DragFloat("Pos", &RTRenderer::Config.wp, 0.1f);
		//	//ImGui::DragFloat("Nor", &RTRenderer::Config.wn, 0.1f);
		//	//ImGui::DragFloat("Col", &RTRenderer::Config.wc, 0.1f);
		//	//ImGui::DragFloat("Pla", &RTRenderer::Config.wpl, 0.1f);
		//	ImGui::Image((void*)RTRenderer::GetGBuffer()->GetColorAttachments()[0]->GetRendererID(), ImVec2(64, 64), { 0, 1 }, { 1, 0 });
		//	ImGui::Image((void*)RTRenderer::GetGBuffer()->GetColorAttachments()[1]->GetRendererID(), ImVec2(64, 64), { 0, 1 }, { 1, 0 });
		//	ImGui::Image((void*)RTRenderer::GetGBuffer()->GetColorAttachments()[2]->GetRendererID(), ImVec2(64, 64), { 0, 1 }, { 1, 0 });
		//	ImGui::Image((void*)RTRenderer::GetGBuffer()->GetColorAttachments()[3]->GetRendererID(), ImVec2(64, 64), { 0, 1 }, { 1, 0 });
		//	ImGui::Image((void*)RTRenderer::GetGBuffer()->GetColorAttachments()[4]->GetRendererID(), ImVec2(64, 64), { 0, 1 }, { 1, 0 });
		//	ImGui::Image((void*)RTRenderer::GetGBuffer()->GetColorAttachments()[5]->GetRendererID(), ImVec2(64, 64), { 0, 1 }, { 1, 0 });
		//}
		//ImGui::End();
	}
}

