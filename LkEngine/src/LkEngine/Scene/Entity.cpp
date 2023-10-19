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

		auto& transform = GetComponent<TransformComponent>();
		auto& mesh = GetComponent<MeshComponent>();

		mesh.BaseShader->Bind();
		mesh.BaseShader->SetUniformMat4f("u_TransformMatrix", transform.GetTransform());
		mesh.BaseShader->SetUniform4f("u_Color", mesh.Color.x, mesh.Color.y, mesh.Color.z, mesh.Color.w);
	}


}