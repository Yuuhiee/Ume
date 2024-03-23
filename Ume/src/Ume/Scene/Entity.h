#pragma once
#include "Ume/Core/Base.h"
#include "entt.hpp"
#include "Scene.h"

namespace Ume
{
	using EntityHandle = entt::entity;

	class Entity
	{
	public:
		Entity() = default;
		Entity(EntityHandle handle, ECS* scene);

	public:
		operator bool() { return m_Handle != entt::null; }
	public:
		template<typename T>
		inline bool HasComponent()
		{
			return m_Scene->m_Registry.all_of<T>(m_Handle);
		}

		template<typename T, typename... Args>
		inline T& AddComponent(Args&&... args)
		{
			UME_CORE_ASSERT(!HasComponent<T>(), "Component already exists.");
			return m_Scene->m_Registry.emplace<T>(m_Handle, std::forward<Args>(args)...);
		}

		template<typename T>
		inline T& GetComponent()
		{
			UME_CORE_ASSERT(HasComponent<T>(), "Component not exists.");
			return m_Scene->m_Registry.get<T>(m_Handle);
		}

		template<typename T>
		inline void RemoveComponent()
		{
			UME_CORE_ASSERT(HasComponent<T>(), "Component not exists.");
			m_Scene->m_Registry.remove<T>(m_Handle);
		}
	private:
		EntityHandle m_Handle = entt::null;
		ECS* m_Scene = nullptr;
	};
}