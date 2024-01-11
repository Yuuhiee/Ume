#pragma once

#include "Ume/Core/Timestep.h"
#include "Ume/Events/Event.h"
#include "Ume/Renderer/Camera.h"

namespace Ume
{
	class CameraController
	{
	public:
		virtual ~CameraController() = default;

		virtual void OnUpdate(Timestep ts) = 0;
		virtual void OnEvent(Event& e) = 0;
		virtual const Camera& GetCamera() const = 0;
	};
}

