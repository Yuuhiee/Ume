#include "umepch.h"
#include "Viewport.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "Ume/Core/Input.h"

namespace Ume
{
	static std::vector<int> buttons = { UME_MOUSE_BUTTON_1, UME_MOUSE_BUTTON_2, UME_MOUSE_BUTTON_3 };

	Viewport::Viewport(const std::string& title)
		: m_Title(title)
	{
	}

	void Viewport::Render()
	{
		ImGui::Begin(m_Title.c_str());
		{
			auto size = ImGui::GetContentRegionAvail();
			size.x = std::max(size.x, 1.0f);
			size.y = std::max(size.y, 1.0f);
			m_IsFocused = ImGui::IsWindowFocused();
			m_IsHovered = ImGui::IsWindowHovered();

			if (Implemented(CallbackType::Resize))
			{
				glm::uvec2 curSize = { size.x, size.y };
				if (curSize != m_Size)
				{
					m_Size = curSize;
					m_Callbacks[CallbackType::Resize](this, (void*)&size);
				}
			}

			if (m_IsHovered && (Implemented(CallbackType::MouseButtonPress) || Implemented(CallbackType::MouseButtonHold) || Implemented(CallbackType::MousButtonRelease)))
			{
				MouseButtonFlags curState;
				for (int i = 0; i < (int)buttons.size(); i++)
				{
					if (Input::IsMouseButtonPressed(buttons[i]))
					{
						curState |= MouseButtonCodeToFlagBit(buttons[i]);
						//pressedButtons |= (MouseButtonFlagBits)(1 << i);
					}
				}
				static MouseButtonFlags preState;
				{
					MouseButtonFlags changed = preState ^ curState;
					MouseButtonFlags pressed = changed & preState ^ changed;
					MouseButtonFlags released = changed & curState ^ changed;
					MouseButtonFlags holding = preState & curState;

					ImGuiWindow* window = nullptr;
					if ((pressed || holding || released) && (window = ImGui::GetCurrentWindow()))
					{
						ImVec2 globalMousePos = ImGui::GetMousePos();
						float tabBarHeight = ImGui::GetCursorPosY();
						glm::vec2 mousePosition;
						mousePosition = { int(globalMousePos.x - window->Pos.x), int(globalMousePos.y - window->Pos.y - tabBarHeight) };
				
						if (pressed) Call(CallbackType::MouseButtonPress, (void*)&MouseClickInfo(pressed, mousePosition));
						if (holding) Call(CallbackType::MouseButtonHold, (void*)&MouseClickInfo(holding, mousePosition));
						if (released) Call(CallbackType::MousButtonRelease, (void*)&MouseClickInfo(released, mousePosition));
					}
					//UME_INFO("{0}, {1} | {2}, {3}, {4}", preState, curState, pressed, holding, released);
				}
				preState = curState;
			}

			if (Implemented(CallbackType::Render))
			{
				m_Callbacks[CallbackType::Render](this, (void*)&size);
			}
		}
		ImGui::End();
	}

	///////////////
	std::vector<WeakRef<Viewport>> ViewportManager::s_Viewports;

	Ref<Viewport> ViewportManager::CreateViewport(const std::string& name)
	{
		auto viewport = CreateRef<Viewport>(name);
		s_Viewports.push_back(viewport);
		return viewport;
	}
}