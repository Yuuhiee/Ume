#pragma once

#include <Ume.h>

#include "glm/glm.hpp"
#include "ParticleSystem.h"

class Sandbox2D : public Ume::Layer
{
public:
	Sandbox2D();
	~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnUpdate(Ume::Timestep ts) override;
	virtual void OnEvent(Ume::Event& e) override;
	virtual void OnImGuiRender() override;
private:
	Ume::OrthographicCameraController m_CameraController;
	Ume::Ref<Ume::Texture2D> m_Texture;
	Ume::Ref<Ume::Texture2D> m_SpiriteSheet;
	Ume::Ref<Ume::Framebuffer> m_Framebuffer;
	ParticleSystem m_ParticleSystem;
	ParticleProps m_Particle;
};
