#include "umepch.h"
#include "ViewportLayer.h"
#include "Viewport.h"
#include "Ume/Core/Input.h"

namespace Ume
{
	void ViewportLayer::OnImGuiRender()
	{
		for (auto& viewportRef : ViewportManager::GetViewports())
		{
			auto viewport = viewportRef.lock();
			if (viewport)
			{
				viewport->Render();
			}
		}
	}

	void ViewportLayer::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseButtonPressedEvent>(UME_BIND_EVENT_FN(ViewportLayer::OnMouseButtonPressed));
	}

	bool ViewportLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		auto buttons = QueryPressedButtons();
		for (auto& viewportRef : ViewportManager::GetViewports())
		{
			auto viewport = viewportRef.lock();
			if (viewport)
			{
				viewport->Render();
			}
		}
		return false;
	}

	MouseButtonFlags ViewportLayer::QueryPressedButtons()
	{
		int buttons[] = { UME_MOUSE_BUTTON_1, UME_MOUSE_BUTTON_2, UME_MOUSE_BUTTON_3 };
		MouseButtonFlags pressedButtons;
		for (auto button : buttons)
		{
			if (Input::IsMouseButtonPressed(button))
			{
				pressedButtons |= MouseButtonCodeToFlagBit(button);
			}
		}
		return pressedButtons;
	}

}