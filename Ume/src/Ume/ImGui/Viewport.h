#pragma once
#include "glm/glm.hpp"
#include "Ume/Core/MouseButtonCodes.h"

namespace Ume
{
	class Viewport
	{
	public:
		enum class CallbackType
		{
			Render, Resize,
			MouseButtonPress, MousButtonRelease, MouseButtonHold
		};
		struct MouseClickInfo
		{
			MouseButtonFlags Buttons;
			glm::ivec2 Position;
			MouseClickInfo(const MouseButtonFlags& buttons, const glm::ivec2& position)
				: Buttons(buttons), Position(position) {}
		};
		using Callback = std::function<void(Viewport*, void*)>;

		Viewport(const std::string& title);
		virtual ~Viewport() = default;

		void Render(); 
		void SetCallback(CallbackType type, const Callback& callback) { m_Callbacks[type] = callback; }

		inline bool IsFocused() const { return m_IsFocused; }
		inline bool IsHovered() const { return m_IsHovered; }
		inline const glm::uvec2& GetSize() const { return m_Size; }
	private:
		std::string m_Title;
		glm::uvec2 m_Size = { 0, 0 };
		std::unordered_map<CallbackType, Callback> m_Callbacks;

		bool m_IsFocused = false;
		bool m_IsHovered = false;

		inline bool Implemented(CallbackType type) { return m_Callbacks.count(type); }
		inline void Call(CallbackType type, void* data) { if (Implemented(type)) m_Callbacks[type](this, data); }
	};

	class ViewportManager
	{
	public:
		static Ref<Viewport> CreateViewport(const std::string& name);
		static std::vector<WeakRef<Viewport>>& GetViewports() { return s_Viewports; }
	private:
		static std::vector<WeakRef<Viewport>> s_Viewports;
	};
}

