#include "umepch.h"
#include "RTRenderer.h"
#include "Ume/Util/PBR.h"
#include "BoundingBox.h"
#include "BVH.h"
#include "AssetManager.h"
#include <execution>
#include "Ume/Renderer/Renderer.h"
#include "Ume/Renderer/RenderCommand.h"
#include "Ume/Renderer/Texture.h"

namespace Ume
{
	uint32_t								RTRenderer::Frame = 0;
	bool									RTRenderer::s_Denoise = false;
	Ref<Scene>								RTRenderer::s_Scene = nullptr;
	Ref<Framebuffer>						RTRenderer::s_PreviousFramebuffer = nullptr;
	Ref<Framebuffer>						RTRenderer::s_CurrentFramebuffer = nullptr;
	Ref<Framebuffer>						RTRenderer::s_FinalFramebuffer = nullptr;
	Ref<Framebuffer>						RTRenderer::s_GFrameuffer = nullptr;
	Ref<Texture2D>							RTRenderer::s_PositionImage = nullptr;
	Ref<Texture2D>							RTRenderer::s_NormalImage = nullptr;
	Ref<Texture2D>							RTRenderer::s_IDImage = nullptr;
	RTRenderer::Configuration				RTRenderer::Config;
	std::vector<RTRenderer::PixelFormat>	RTRenderer::s_Data;
	std::vector<uint32_t>					RTRenderer::s_HorizontalIter;
	std::vector<uint32_t>					RTRenderer::s_VerticalIter;

	static Ref<Shader> denoiseShader;
	static Ref<Shader> gbufferShader;
	static Ref<VertexArray> quadVA;
	static bool first = true;

	constexpr uint32_t STRIDE = sizeof(float) * 4 / sizeof(RTRenderer::PixelFormat);

	static glm::vec3 GetLightDirection(const Ref<Ume::Object>& light, const glm::vec3& position, const glm::vec3& sampleOnLight)
	{
		auto& emission = light->Material->Emission;
		if (emission->Type == LightType::Point)
		{
			return glm::normalize(sampleOnLight - position);
		}
		else if (emission->Type == LightType::Directional)
		{
			return -emission->Direction;
		}
		return { 1.0f, 0.0f, 0.0f };
	}

	glm::vec3 RTRenderer::GetIrradiance(const Material& material, const glm::vec3& lightPos, const glm::vec3& receivePos, bool rangeCheck)
	{  
		auto& emission = material.Emission;
		if (!material.Emissive && !emission) return glm::vec3(0.0f);
		if (emission->Type == LightType::Point)
		{
			float distance = glm::length(lightPos - receivePos);
			if (!rangeCheck || rangeCheck && distance < emission->Range)
			{
				auto& lightAtt = emission->Attenuation;
				float attenuation = 1.0f / (lightAtt.x + lightAtt.y * distance + lightAtt.z * distance * distance);
				return emission->Color * emission->Intensity * attenuation;
			}
		}
		else if (emission->Type == LightType::Directional)
		{
			return emission->Color * emission->Intensity;
			float weight = 1.0f;
			if (glm::dot(glm::normalize(receivePos - lightPos), emission->Direction) > 0.98f)
				return emission->Color * emission->Intensity;
			else if (!rangeCheck) return material.Albedo;
		}
		return glm::vec3(0.0f);
	}

	static const Light& RandomPickLight(const std::vector<Light>& lights, const glm::vec3& shadePoint, float& pdf)
	{
		float size = (float)lights.size();
		pdf = 1.0f / size;
		int selected = (int)std::clamp(size * Random::Float(), 0.0f, size - 0.001f);
		return lights[selected];
	}

	static glm::vec3 BRDF(const Material& material, const glm::vec2 texcoord, const glm::vec3& N, const glm::vec3& V, const glm::vec3& L)
	{
		auto albedo = material.UseAlbedoTexture ? material.AlbedoTexture->Sample(texcoord) : material.Albedo;
		albedo *= material.ColorTint;
		auto roughness = material.UseRoughnessTexture ? material.RoughnessTexture->Sample(texcoord).x : material.Roughness;
		auto metallic = material.UseMetallicTexture ? material.MetallicTexture->Sample(texcoord).x : material.Metallic;
		return PBR::BRDF(albedo, roughness, metallic, N, V, L);
	}

	const Ref<Texture2D>& RTRenderer::GetImage()
	{
		return s_FinalFramebuffer->GetColorAttachments()[0];
	}

	void RTRenderer::Clear()
	{
		s_Data = std::vector<PixelFormat>(s_Data.size(), glm::vec4(0.0f));
		Frame = 0;
	}

	void RTRenderer::Init(uint32_t width, uint32_t height)
	{
		{
			TextureSpecification colorTexture;
			colorTexture.Format = ImageFormat::RGBA16F;
			FramebufferDescription description;
			description.Width = width;
			description.Height = height;
			description.ColorAttachments = { colorTexture };
			s_PreviousFramebuffer = Framebuffer::Create(description);
			s_CurrentFramebuffer = Framebuffer::Create(description);
			s_FinalFramebuffer = Framebuffer::Create(description);
		}
		{
			TextureSpecification coordInfo = { ImageFormat::RGB16F };
			TextureSpecification idInfo = { ImageFormat::RED8UI };

			FramebufferDescription description;
			description.Width = width;
			description.Height = height;
			description.ColorAttachments = { coordInfo, coordInfo, coordInfo };
			s_GFrameuffer = Framebuffer::Create(description);
		}
		quadVA = VertexArray::Create(ObjectType::Quad);
		denoiseShader = Shader::Create("assets/shaders/Denoise.glsl");
		gbufferShader = Shader::Create("assets/shaders/GBuffer.glsl");

	}

	void RTRenderer::Resize(uint32_t width, uint32_t height)
	{
		s_PreviousFramebuffer->Resize(width, height);
		s_CurrentFramebuffer->Resize(width, height);
		s_FinalFramebuffer->Resize(width, height);
		s_GFrameuffer->Resize(width, height);
		s_Data.resize(width * height * STRIDE, glm::vec4(0.0f));

		s_HorizontalIter.resize(width);
		s_VerticalIter.resize(height);
		for (uint32_t i = 0; i < width; i++)
			s_HorizontalIter[i] = i;
		for (uint32_t i = 0; i < height; i++)
			s_VerticalIter[i] = i;
	}

	void RTRenderer::Render(Timestep ts)
	{
		TraceScene();
		Denoise();
	}

	static glm::vec3 GetLightIrradiance(Object* light, const glm::vec3& N, const glm::vec3& L, float distance)
	{
		float NoL = std::max(glm::dot(N, L), 0.0f);
		auto& emission = light->Material->Emission;
		auto& lightAtt = emission->Attenuation;
		float attenuation = 1.0f / (lightAtt.x + lightAtt.y * distance + lightAtt.z * distance * distance);
		return emission->Color * emission->Intensity * attenuation * NoL;
	}

	//glm::vec3 RTRenderer::Shade(HitPayload& point, const glm::vec3& inDir)
	//{
	//	auto& material = *point.Object->Material;
	//	SampleMethodFlags method;
	//	if (material.Roughness < 0.2f)		method = SampleMethodFlagBits::ImportanceGGX;
	//	else if (material.Roughness < 0.5f) method = SampleMethodFlagBits::ImportanceGGX | SampleMethodFlagBits::ImportanceLight;
	//	else								method = SampleMethodFlagBits::ImportanceCos | SampleMethodFlagBits::ImportanceLight;
	//	return Shade(point, inDir, method);
	//}

	glm::vec3 RTRenderer::Shade(HitPayload& point, const glm::vec3& inDir)
	{
		glm::vec3 color(0.0f);

		auto& material = *point.Object->Material;
		auto& N = point.Vertex.Normal;
		auto& P = point.Vertex.Position;
		auto& V = -inDir;
		auto& texcoord = point.Vertex.Texcoord;

		glm::vec3 albedo(0.0f);
		float roughness = 1.0f;
		float metallic = 1.0f;
		SampleMethodFlags method;
		{
			albedo = material.UseAlbedoTexture ? material.AlbedoTexture->Sample(texcoord) : material.Albedo;
			albedo *= material.ColorTint;
			roughness = material.UseRoughnessTexture ? material.RoughnessTexture->Sample(texcoord).x : material.Roughness;
			metallic = material.UseMetallicTexture ? material.MetallicTexture->Sample(texcoord).x : material.Metallic;
			if (material.UseNormalTexture)
			{
				glm::vec3 normal = material.NormalTexture->Sample(texcoord);
				normal = glm::normalize(normal * 2.0f - 1.0f);
				glm::vec3 T = N;
				glm::vec3 B = glm::normalize(glm::cross(N, V));
				glm::mat3 TBN = { T, B, N };
				N = glm::normalize(TBN * normal);
			}
			if (roughness < 0.2f)		method = SampleMethodFlagBits::ImportanceGGX;
			else if (roughness < 0.5f) method = SampleMethodFlagBits::ImportanceGGX | SampleMethodFlagBits::ImportanceLight;
			else								method = SampleMethodFlagBits::ImportanceCos | SampleMethodFlagBits::ImportanceLight;
		}

		if (method & SampleMethodFlagBits::ImportanceLight)
		{
			float sumArea = 0.0f;
			for (auto light : s_Scene->Lights)
				if (light->Active() && light->Emissive()) sumArea += light->Area;
			float pArea = Random::Float() * sumArea;
			sumArea = 0.0f;
			for (auto light : s_Scene->Lights)
			{
				if (!light->Active() || !light->Emissive()) continue;
				sumArea += light->Area;
				if (sumArea >= pArea)
				{
					float pdf;
					auto vertex = light->Sample(pdf).Vertex;
					auto sample = vertex.Position;
					auto L = glm::normalize(sample - P);
					auto lightInter = s_Scene->TraceRay({ P, L });
					if (lightInter.Object == light.get())
					{
						float distance = glm::length(sample - P);
						float distance2 = distance * distance;
						float cos_pri = glm::dot(glm::normalize(P - sample), vertex.Normal);
						auto brdf = PBR::BRDF(albedo, roughness, metallic, N, V, L);
						//auto brdf = PBR::BSDF(albedo, roughness, 1.5, metallic, N, V, L);
						auto lighrRad = GetLightIrradiance(light.get(), N, L, distance) * std::max(glm::dot(N, L), 0.0f);
						color += brdf * lighrRad / pdf * cos_pri / distance2;
					}
					break;
				}
			}
		}

		if (Random::Float() < Config.RR)
		{
			float pdf = 1.0f;
			glm::vec3 outDir = { 1.0f, 0.0f, 0.0f };
			if (method & SampleMethodFlagBits::ImportanceCos) outDir = Random::ImportanceSampleCos(N, pdf);
			else if (method & SampleMethodFlagBits::ImportanceGGX) outDir = Random::ImportanceSampleGGX(N, inDir, material.Roughness, pdf);

			auto& nextPoint = s_Scene->TraceRay({ P, outDir });
			glm::vec3 irradiance(0.0f);
			if (nextPoint.Hit)
			{
				if (!nextPoint.Object->Material->Emissive)
					irradiance += Shade(nextPoint, outDir);
				else if (!(method & SampleMethodFlagBits::ImportanceLight))
					irradiance += GetLightIrradiance(nextPoint.Object, N, outDir, glm::length(nextPoint.Vertex.Position - P));
			}
			else irradiance += s_Scene->GetEnvironment(outDir);
			color += PBR::BRDF(albedo, roughness, metallic, N, V, outDir) * irradiance * std::max(glm::dot(outDir, N), 0.0f) / Config.RR / pdf;
		}

		return color;
	}

	glm::vec4 RTRenderer::RayGen(glm::vec2 coord, uint32_t x, uint32_t y)
	{
		auto camera = (RTCamera*)(s_Scene->ActivedCamera.get());
		
		glm::vec3 direction;

		if (Config.Antilias)
		{
			glm::vec2 xy = { x + Random::Float() - 0.5f, y + Random::Float() - 0.5f };
			glm::vec2 coord = xy * camera->GetPixelSize() * 2.0f - 1.0f;
			glm::vec4 target = camera->GetInverseProjection() * glm::vec4(coord, -1, 1);
			direction = camera->GetInverseView() * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0);
		}
		else
		{
			direction = camera->GetRayDirection(x, y);
		}

        Ray ray(camera->Position, direction);
		HitPayload payload = s_Scene->TraceRay(ray);

		glm::vec3 radiance(0.0f);
		for (int i = 0; i < Config.SPP; i++)
		{
			if (payload.Hit)
			{
				auto& material = *payload.Object->Material;
				auto& vertex = payload.Vertex;
				//return { material.Albedo, 1.0f };
				//return { vertex.Normal, 1.0f };
				//return logo->Sample(vertex.Texcoord);

				if (!Config.Env)
				{
					radiance += payload.Object->Emissive() ? GetIrradiance(material, vertex.Position, camera->Position, false)
												  : Shade(payload, ray.Direction);
				}
				else
				{
					//glm::vec3 R = glm::reflect(direction, vertex.Normal);
					//glm::vec3 irradiance = s_Scene->GetEnvironment(R);
					//auto brdf = PBR::BRDF(material, vertex.Normal, -direction, R);
					//radiance += brdf * irradiance * std::max(glm::dot(R, vertex.Normal), 0.0f);
					//radiance += brdf;
				}
			}
			else
				radiance += s_Scene->GetEnvironment(direction);
		}
		radiance /= (float)Config.SPP;
		return { radiance, 1.0f };
	}

	void RTRenderer::TraceScene()
	{
		if (Config.Accumulate) Frame++;
		else Frame = 0;
		auto width = GetImage()->GetWidth();
		auto height = GetImage()->GetHeight();
		std::for_each(std::execution::par, s_VerticalIter.begin(), s_VerticalIter.end(),
			[width, height](uint32_t y)
			{
				std::for_each(std::execution::par, s_HorizontalIter.begin(), s_HorizontalIter.end(),
					[width, height, y](uint32_t x)
					{
						glm::vec2 coord = { (float)x / (float)width, (float)y / (float)height };
						s_Data[(y * width + x) * STRIDE] = RayGen(coord, x, y);
					});
			});
		s_CurrentFramebuffer->GetColorAttachments()[0]->SetData(s_Data.data(), (uint32_t)s_Data.size() * sizeof(PixelFormat));
	}

	void RTRenderer::Denoise()
	{
		s_FinalFramebuffer->Bind();
		RenderCommand::SetClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		RenderCommand::Clear();
		Renderer::BeginScene(glm::mat4(1.0f));
		{
			denoiseShader->Bind();
			denoiseShader->SetTexture("u_PreviousImage", s_PreviousFramebuffer->GetColorAttachments()[0], 1);
			denoiseShader->SetTexture("u_CurrentImage", s_CurrentFramebuffer->GetColorAttachments()[0], 2);
			denoiseShader->SetTexture("u_PositionImage", s_GFrameuffer->GetColorAttachments()[0], 3);
			denoiseShader->SetTexture("u_NormalImage", s_GFrameuffer->GetColorAttachments()[1], 4);
			denoiseShader->SetTexture("u_IDImage", s_GFrameuffer->GetColorAttachments()[2], 5);
			denoiseShader->SetInt("Width", GetImage()->GetWidth());
			denoiseShader->SetInt("Height", GetImage()->GetHeight());
			denoiseShader->SetInt("Filter", Config.Denoise);
			denoiseShader->SetFloat("Frame", Frame);
			denoiseShader->SetInt("Accumulate", Config.Accumulate);
			Renderer::Submit(quadVA);
		}
		Renderer::EndScene();
		s_FinalFramebuffer->Unbind();
		std::swap(s_PreviousFramebuffer, s_FinalFramebuffer);
	}

	void RTRenderer::GBufferPass(const glm::mat4& vp)
	{
		s_GFrameuffer->Bind();
		RenderCommand::SetClearColor(0.0f, 0.0f, 1.0f, 0.0f);
		RenderCommand::Clear();
		Renderer::BeginScene(vp);
		{
			gbufferShader->Bind();
			gbufferShader->SetMat4("u_ViewProjection", vp);
			for (const auto& object : s_Scene->Objects)
			{
				if (!object->Active()) continue;
				gbufferShader->SetMat4("u_Model", object->Transform.ModelMatrix);
				gbufferShader->SetMat3("u_NormalMatrix", object->Transform.NormalMatrix);
				gbufferShader->SetInt("u_ObjectID", object->ID);
				//UME_INFO(object->ID);
				Renderer::Submit(object->GetVertexArray());
			}
		}
		Renderer::EndScene;
		s_GFrameuffer->Unbind();
	}
}