#include "umepch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include "glad/glad.h"
#include <glm/gtc/type_ptr.hpp>

namespace Ume
{
	Ref<Shader> Shader::Create(const std::string& filepath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: UME_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLShader>(filepath);
		}

		UME_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			UME_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLShader>(name, vertexSrc, fragmentSrc);
		}

		UME_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		const auto& name = shader->GetName();
		Add(name, shader);
	}

	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		UME_CORE_ASSERT(!Exists(name), "Shader already exists!");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Load(const std::string& filepath)
	{
		auto& shader = Shader::Create(filepath);
		Add(shader);
	}

	void ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	{
		auto& shader = Shader::Create(filepath);
		Add(name, shader);
	}

	const Ref<Shader>& ShaderLibrary::Get(const std::string& name)
	{
		UME_CORE_ASSERT(Exists(name), "Shader not found!");
		return m_Shaders[name];
	}
}