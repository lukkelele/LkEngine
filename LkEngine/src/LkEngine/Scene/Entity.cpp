#include "LKpch.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}

	Entity Entity::GetParent() const
	{
		return m_Scene->TryGetEntityWithUUID(GetParentUUID());
	}


	Entity::operator bool() const { return (m_EntityHandle != entt::null) && m_Scene && m_Scene->m_Registry.valid(m_EntityHandle); }
}