#include "LKpch.h"
#include "LkEngine/Scene/Entity.h"

namespace LkEngine {

	 Entity::Entity(entt::entity handle, Scene* scene)
	 	: m_EntityHandle(handle), m_Scene(scene)
	 {
	 }

	void Entity::OnUpdate(float ts, glm::mat4 viewProj)
	{
		// Get updates from rigidbody
	#ifdef LK_ENGINE_OLD_RIGIDBODY
		if (HasComponent<Rigidbody>())
		{
			// if the entity has a rigidbody, sync the body with the transform component
			Rigidbody& rigidbody = GetComponent<Rigidbody>();
			TransformComponent& transformComponent = GetComponent<TransformComponent>();

			auto [pos, rot] = rigidbody.GetPosAndRotation();
			glm::vec3 dimensions = rigidbody.GetDimensions();

			auto lv = rigidbody.GetLinearVelocity();

			transformComponent.translation = pos;
			transformComponent.rotation = rot;
			transformComponent.scale = dimensions;

			glm::mat4 model = transformComponent.GetTransform();
			Material& material = GetComponent<Material>();
			material.Bind();
			material.Set("u_ViewProj", viewProj);
			material.Set("u_Model", model);
			material.Set("u_Color", material.GetMaterialColor());
		}
	#endif
	}


}