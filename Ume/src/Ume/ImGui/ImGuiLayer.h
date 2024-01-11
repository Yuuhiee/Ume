#pragma once

#include "Ume/Core/Layer.h"
#include "Ume/Events/MouseEvent.h"
#include "Ume/Events/KeyEvent.h"
#include "Ume/Events/ApplicationEvent.h"

namespace Ume
{
	class UME_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnDetach() override;
		virtual void OnAttach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
	private:
		float m_Time = 0.0f;
	};

}
