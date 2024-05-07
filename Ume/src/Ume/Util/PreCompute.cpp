#include "umepch.h"
#include "PreCompute.h"
#include "Ume/Renderer/Framebuffer.h"
#include "Ume/Renderer/Renderer.h"
#include "Ume/Renderer/Shader.h"
#include "Ume/Renderer/RenderCommand.h"
namespace Ume
{
	static Ref<Shader> irradianceShader = nullptr;

	Ref<Texture2D> PreCompute::GetIrradiance(const Ref<Texture2D>& radiance)
	{
		if (!irradianceShader)
		{
			irradianceShader = Shader::Create("assets/shaders/PreComputeIrradiance.glsl");
		}

		TextureSpecification sp;
		sp.Format = ImageFormat::RGBA16F;
		FramebufferDescription description;
		description.Width = radiance->GetWidth();
		description.Height = radiance->GetHeight();
		description.ColorAttachments = { sp };

		auto framebuffer = Framebuffer::Create(description);

		framebuffer->Bind();
		RenderCommand::SetClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		RenderCommand::Clear();
		irradianceShader->Bind();
		irradianceShader->SetTexture("u_Radiance", radiance, 0);
		Renderer::DrawQuad();
		framebuffer->Unbind();

		return framebuffer->GetColorAttachments()[0];
	}
}