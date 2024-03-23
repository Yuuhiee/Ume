#pragma once

#include "Ume.h"
#include "glm/glm.hpp"

namespace Ume
{
	class RayTracingLayer : public Layer
	{
	public:
		RayTracingLayer();

		virtual void OnAttach() override;
		virtual void OnUpdate(Timestep timestep) override;
		virtual void OnImGuiRender() override;
		virtual void OnDetach() override {};
		virtual void OnEvent(Event& e) override {};
	private:
		Viewport m_SceneViewport = { "Preview" };
		Viewport m_RenderViewport = { "Render" };
		Ref<ECS> m_ECS;
		Ref<Scene> m_Scene;

		void CreateScene();
		void CreateRenderPasses();
	};
}