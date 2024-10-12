#include "LKpch.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

#if 1
	LEntity::LEntity(entt::entity InHandle, LScene* InScene)
		: m_EntityHandle(InHandle)
		, m_Scene(InScene)
		//, m_Scene(TObjectPtr<LScene>(InScene))
	{
	}
#endif

#if 1
	LEntity::LEntity(entt::entity InHandle, TObjectPtr<LScene> InScene)
		: m_EntityHandle(InHandle)
		, m_Scene(InScene)
	{
	}
#endif

#if 0
	LEntity::LEntity(const entt::entity InHandle, const LScene* InScene)
		: m_EntityHandle(InHandle)
		, m_Scene(InScene)
	{
	}
#endif

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