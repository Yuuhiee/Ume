#pragma once

#include <Ume.h>

#include "glm/glm.hpp"
#include "ParticleSystem.h"

namespace Ume
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnEvent(Event& e) override;
		virtual void OnImGuiRender() override;
	private:
		OrthographicCameraController m_CameraController;
		Ref<Texture2D> m_Texture;
		Ref<Texture2D> m_SpiriteSheet;
		Ref<Framebuffer> m_Framebuffer;
		ParticleSystem m_ParticleSystem;
		ParticleProps m_Particle;
		Viewport m_Viewport = { "Viewport" };
		Ref<ECS> m_Scene;
		Entity m_CameraEntity;
		Entity m_SquareEntity;
	};

}