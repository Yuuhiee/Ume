#include <Ume.h>

class Sandbox : public Ume::Application
{
public:
	Sandbox() {}
	~Sandbox() {}
};

Ume::Application* Ume::CreateApplication()
{
	return new Sandbox();
}
