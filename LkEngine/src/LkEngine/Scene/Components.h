#pragma once
#include "LkEngine/Core/UUID.h"
#include "LkEngine/Renderer/VertexArray.h"
#include "LkEngine/Renderer/Shader.h"
#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/Scene/SceneCamera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>


namespace LkEngine{

	class Material;
	class Mesh;
	class RigidBody;

	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	struct TagComponent
	{
		std::string tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: tag(tag) {}
	};

	struct TransformComponent
	{
		glm::vec3 translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
		glm::quat rotation = { 1.0f, 0.0f, 0.0f, 0.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent& other) = default;
		TransformComponent(const glm::vec3& translation)
			: translation(translation) {}

		glm::mat4 GetTransform() const
		{
			return glm::translate(glm::mat4(1.0f), translation)
				 * glm::toMat4(rotation)
				 * glm::scale(glm::mat4(1.0f), scale);
		}

	};

	struct CameraComponent
	{
		enum class Type { Null = -1, FirstPerson, ThirdPerson, Ortographic };
		Type projectionType;
		SceneCamera camera;

		CameraComponent() = default;
		CameraComponent(const CameraComponent& other) = default;

		operator SceneCamera& () { return camera; }
		operator const SceneCamera& () const { return camera; }
	};


	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents =
		ComponentGroup<IDComponent, TagComponent, Mesh>;
		//ComponentGroup<IDComponent, TagComponent, Mesh, BoxColliderShape>;

}
