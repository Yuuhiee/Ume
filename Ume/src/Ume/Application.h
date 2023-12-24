#pragma once

#include "Core.h"

namespace Ume {

	class UME_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}

