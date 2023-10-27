#include "LKpch.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}

	void Entity::OnUpdate(float ts)
	{
		if (!HasComponent<MeshComponent>() || !HasComponent<TransformComponent>())
			return;

		auto& mesh = GetComponent<MeshComponent>();
	}


}