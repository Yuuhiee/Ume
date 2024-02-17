#pragma once

#ifdef UME_PLATFORM_WINDOWS

extern Ume::Application* Ume::CreateApplication();

int main(int argc, char** argv)
{
	Ume::Log::Init();

	UME_PROFILE_BEGIN_SESSION("Startup", "temp/Startup.json");
	auto app = Ume::CreateApplication();
	UME_PROFILE_END_SESSION();

	UME_PROFILE_BEGIN_SESSION("Runtime", "temp/Runtime.json");
	app->Run();
	UME_PROFILE_END_SESSION();

	UME_PROFILE_BEGIN_SESSION("Shutdown", "temp/Shutdown.json");
	delete app;
	UME_PROFILE_END_SESSION();

	return 0;
}

#endif // UME_PLATFORM_WINDOWS
