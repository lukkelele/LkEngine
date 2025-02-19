#include "LKpch.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

	LEntity::LEntity()
	{
		LOBJECT_REGISTER();
	}

	LEntity::LEntity(entt::entity InHandle, TObjectPtr<LScene> InScene)
		: Handle(InHandle)
		, Scene(InScene)
	{
		LOBJECT_REGISTER();
	}

	LEntity LEntity::GetParent() const
	{
		return Scene->TryGetEntityWithUUID(GetParentUUID());
	}

	bool LEntity::IsAncestorOf(LEntity Entity) const
	{
		const std::vector<LUUID>& Children = GetChildren();
		if (Children.empty())
		{
			return false;
		}

		for (const LUUID& Child : Children)
		{
			if (Child == Entity.GetUUID())
			{
				return true;
			}
		}

		for (const LUUID& Child : Children)
		{
			if (Scene->GetEntityWithUUID(Child).IsAncestorOf(Entity))
			{
				return true;
			}
		}

		return false;
	}

	LEntity::operator bool() const 
	{ 
		return ((Handle != entt::null) && Scene && (Scene->Registry.valid(Handle)));
	}

}
