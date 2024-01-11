#pragma once

namespace Ume
{
	class GraphicsContext 
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffer() = 0;
		virtual void Resize(float width, float height) = 0;
	};
}