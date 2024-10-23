#include "LKpch.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

	LEntity::LEntity(entt::entity InHandle, TObjectPtr<LScene> InScene)
		: m_EntityHandle(InHandle)
		, m_Scene(InScene)
	{
	}

	/// REMOVE
	LEntity::LEntity(entt::entity InHandle, LScene* InScene)
		: m_EntityHandle(InHandle)
		, m_Scene(InScene)
	{
	}

	LEntity LEntity::GetParent() const
	{
		return m_Scene->TryGetEntityWithUUID(GetParentUUID());
	}

	bool LEntity::IsAncestorOf(LEntity Entity) const
	{
		const std::vector<UUID>& Children = GetChildren();
		if (Children.empty())
		{
			return false;
		}

		for (const UUID& Child : Children)
		{
			if (Child == Entity.GetUUID())
			{
				return true;
			}
		}

		for (const UUID& Child : Children)
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
		return ((m_EntityHandle != entt::null)
				&& m_Scene && (m_Scene->m_Registry.valid(m_EntityHandle)));
	}

}