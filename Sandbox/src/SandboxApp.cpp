#include <Ume.h>
#include "Ume/Core/EntryPoint.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui.h"

#include "Sandbox2D.h"
#include "Sandbox3D.h"

using namespace Ume;

class Sandbox : public Ume::Application
{
public:
	Sandbox()
	{
		PushLayer(new Sandbox2D());
	}
	~Sandbox() {}
};

Ume::Application* Ume::CreateApplication()
{
	return new Sandbox();
}
