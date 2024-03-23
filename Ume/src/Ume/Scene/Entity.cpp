#include "umepch.h"
#include "Entity.h"

namespace Ume
{

	Entity::Entity(EntityHandle handle, ECS* scene)
		: m_Handle(handle), m_Scene(scene)
	{

	}
}