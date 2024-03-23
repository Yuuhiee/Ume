#pragma once

#include "VertexArray.h"

namespace Ume
{
	enum class PolygonMode
	{
		Point, Line, Fill
	};
	static std::vector<const char*> PolygonModeNames = { "Point", "Line", "Fill" };

	enum class DepthTestFunc
	{
		Never, Less, Equal, Lequal, Greater, NotEqual, Gequal, Always
	};

	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1
		};
	public:
		virtual void Init() = 0;
		virtual void Clear() = 0;
		virtual void SetPolygonMode(PolygonMode mode) = 0;
		virtual void SetClearColor(float r, float g, float b, float a) = 0;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void EnableDepthTest(bool enable) = 0;
		virtual void SetDepthTestFunc(DepthTestFunc func) = 0;

		inline static API GetAPI() { return s_API; }
		static Scope<RendererAPI> Create();
	private:
		static API s_API;
	};
}

