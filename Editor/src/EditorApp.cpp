#include <Ume.h>
#include "Ume/Core/EntryPoint.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui.h"

#include "EditorLayer.h"

namespace Ume
{
	class UmeEditor : public Application
	{
	public:
		UmeEditor()
			: Application("Ume Editor")
		{
			PushLayer(new EditorLayer());
		}
		~UmeEditor() {}
	};

	Application* CreateApplication()
	{
		return new UmeEditor();
	}
}
