#include "LKpch.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

	LEntity::LEntity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle)
		, m_Scene(scene)
	{
	}

	LEntity LEntity::GetParent() const
	{
		return m_Scene->TryGetEntityWithUUID(GetParentUUID());
	}

	bool LEntity::IsAncestorOf(LEntity Entity) const
	{
		const auto& Children = GetChildren();
		if (Children.empty())
		{
			return false;
		}

		for (UUID Child : Children)
		{
			if (Child == Entity.GetUUID())
			{
				return true;
			}
		}

		for (const UUID Child : Children)
		{
			if (m_Scene->GetEntityWithUUID(Child).IsAncestorOf(Entity))
			{
				return true;
			}
		}
		return false;
	}

	LEntity::operator bool() const 
	{ 
		return (m_EntityHandle != entt::null) && m_Scene && m_Scene->m_Registry.valid(m_EntityHandle); 
	}

}