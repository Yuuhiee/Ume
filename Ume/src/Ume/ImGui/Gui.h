#pragma once

#include "glm/glm.hpp"
#include "Ume/Renderer/Texture.h"

namespace Ume
{
	class GUI
	{
	public:
		static void ShowDockSpace(bool* p_open);
		static void ShowDockSpace(bool* p_open, std::function<void(void)> func);
		static bool Vec3Controller(const std::string& label, glm::vec3& values, float step = 0.1f, float resetValue = 0.0f, float labelWidth = 80.0f);

		template<typename Enum>
		static bool Selector(const char* name, Enum& value, const std::vector<const char*>& items)
		{
			bool updated = false;
			int selectedIndex = (int)value;
			if (ImGui::BeginCombo(name, items[selectedIndex], ImGuiComboFlags_None))
			{
				int i = 0;
				for (const auto& item : items)
				{
					bool isSelected = i == selectedIndex;
					if (ImGui::Selectable(item, isSelected))
					{
						value = (Enum)i;
						updated = true;
					}
					if (isSelected)
						ImGui::SetItemDefaultFocus();
					i++;
				}
				ImGui::EndCombo();
			}
			return updated;
		}

		static bool ImagePicker(Ref<Texture2D>& texture, TextureSpecification sp = {});
	};
}