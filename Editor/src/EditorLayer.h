#pragma once

#include <Ume.h>

#include "glm/glm.hpp"
#include "ParticleSystem.h"

namespace Ume
{
	class EditorLayer : public Ume::Layer
	{
	public:
		EditorLayer();
		~EditorLayer() = default;

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
		glm::uvec2 m_ViewportSize = { 0, 0 };
	};

}