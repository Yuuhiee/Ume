#pragma once

#ifdef UME_PLATFORM_WINDOWS

extern Ume::Application* Ume::CreateApplication();

int main(int argc, char** argv)
{
	std::cout << "Ume Engine start!" << std::endl;
	auto app = Ume::CreateApplication();
	app->Run();
	delete app;
}

#endif // UME_PLATFORM_WINDOWS
