#pragma once
#include "Ume/Core/Layer.h"
#include "Ume/Events/MouseEvent.h"
#include "Ume/Core/MouseButtonCodes.h"

namespace Ume
{
	class ViewportLayer : public Layer
	{
	public:
		ViewportLayer() : Layer("Viewports") {}
		virtual ~ViewportLayer() {}

		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& e) override;
	private:
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		MouseButtonFlags QueryPressedButtons();
	};
}
