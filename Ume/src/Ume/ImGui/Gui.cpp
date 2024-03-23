#include "umepch.h"
#include "Gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "Ume/Core/Application.h"
#include "Ume/Renderer/RayTracing/AssetManager.h"

namespace Ume
{
	void GUI::ShowDockSpace(bool* p_open)
	{
		static bool opt_fullscreen_persistant = true;
		static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;
		bool opt_fullscreen = opt_fullscreen_persistant;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (opt_flags & ImGuiDockNodeFlags_PassthruDockspace)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", p_open, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Dockspace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

				if (ImGui::MenuItem("Exit")) Application::Get().Close();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::End();
	}

	void GUI::ShowDockSpace(bool* p_open, std::function<void(void)> func)
	{
		static bool opt_fullscreen_persistant = true;
		static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;
		bool opt_fullscreen = opt_fullscreen_persistant;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (opt_flags & ImGuiDockNodeFlags_PassthruDockspace)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", p_open, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Dockspace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			//func();
		}

		ImGui::End();
	}

	bool GUI::Vec3Controller(const std::string& label, glm::vec3& values, float step, float resetValue, float labelWidth)
	{
		bool updated = false;
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, labelWidth);
		ImGui::Text("%s", label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float  lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		if (ImGui::Button("X", buttonSize))
		{
			values.x = resetValue;
			updated = true;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		if (ImGui::DragFloat("##X", &values.x, step, 0.0f, 0.0f, "%.2f"))
			updated = true;

		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.45f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.55f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.45f, 0.2f, 1.0f });
		if (ImGui::Button("Y", buttonSize))
		{
			values.y = resetValue;
			updated = true;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##Y", &values.y, step, 0.0f, 0.0f, "%.2f"))
			updated = true;
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		if (ImGui::Button("Z", buttonSize))
		{
			values.z = resetValue;
			updated = true;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		if (ImGui::DragFloat("##Z", &values.z, step, 0.0f, 0.0f, "%.2f"))
			updated = true;
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);
		ImGui::PopID();
		ImGui::Spacing();
		return updated;
	}

	bool GUI::ImagePicker(Ref<Texture2D>& texture, bool showSRGB)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
		ImGui::Image(texture ? (void*)texture->GetRendererID() : (void*)AssetManager::GetTexture(0)->GetRendererID(), ImVec2(64, 64), { 0, 1 }, { 1, 0 });
		ImGui::PopStyleVar();
		//if (ImGui::IsItemHovered())
		//{
		//	if (texture)
		//	{
		//		ImGui::BeginTooltip();
		//		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		//		ImGui::TextUnformatted(texture->GetFilePath().c_str());
		//		ImGui::PopTextWrapPos();
		//		ImGui::Image((void*)texture->GetRendererID(), ImVec2(384, 384));
		//		ImGui::EndTooltip();
		//	}
		//}
		bool sRGB = texture ? texture->Specification().SRGB : true;
		if (ImGui::IsItemClicked())
		{
			std::string filename = Application::Get().OpenFile("");
			if (filename != "")
			{
				TextureSpecification sp;
				if (showSRGB) sp.SRGB = sRGB;
				texture = Texture2D::Create(filename, sp);
				return true;
			}
		}
		if (showSRGB)
		{
			ImGui::SameLine();
			if (ImGui::Checkbox("sRGB", &sRGB))
			{
				if (texture)
				{
					TextureSpecification sp;
					sp.SRGB = sRGB;
					texture = Texture2D::Create(texture->GetFilePath(), sp);
				}
			}
		}
		return false;
	}
}