#pragma once

#include <Ume.h>

#include "glm/glm.hpp"

class Sandbox3D : public Ume::Layer
{
public:
	Sandbox3D();
	~Sandbox3D() = default;

	virtual void OnUpdate(Ume::Timestep ts) override;
	virtual void OnEvent(Ume::Event& e) override;
	virtual void OnImGuiRender() override;
private:
	Ume::PerspectiveCameraController m_PerspectiveCameraController;
	Ume::Ref<Ume::Shader> m_TextureShader;
	Ume::Ref<Ume::VertexArray> m_CubeVA;
	Ume::Ref<Ume::Texture2D> m_DefaultTexture;
	Ume::Ref<Ume::Texture2D> m_LogoTexture;
	glm::vec3 m_CubeColorTint = { 1.0f, 1.0f, 1.0f };
};
