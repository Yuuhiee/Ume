#pragma once

#ifdef UME_PLATFORM_WINDOWS

extern Ume::Application* Ume::CreateApplication();

int main(int argc, char** argv)
{
	Ume::Log::Init();

	UME_TRACE("START");

	auto app = Ume::CreateApplication();
	app->Run();
	delete app;

	return 0;
}

#endif // UME_PLATFORM_WINDOWS
