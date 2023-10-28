#pragma once
#include "LkEngine/Math/Math.h"


namespace LkEngine {

    struct TransformComponent
    {
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };
		glm::quat Rotation = { 1.0f, 0.0f, 0.0f, 0.0f };
		// TODO: Patch out the use of Rotation2D and just use the Rotation quaternion
		float Rotation2D = 0.0f;

		TransformComponent() = default;
		TransformComponent(const TransformComponent& other) = default;
		TransformComponent(const glm::vec3& Translation)
			: Translation(Translation) {}

		glm::vec3 GetTranslation() const { return Translation; }
		glm::vec3 GetScale() const { return Scale; }
		glm::quat GetRotation() const { return Rotation; }
		float GetRotation2D() const { return Rotation2D; }

		glm::mat4 GetTransform() const
		{
			return glm::translate(glm::mat4(1.0f), Translation)
				* glm::toMat4(Rotation)
				* glm::scale(glm::mat4(1.0f), Scale);
		}

		glm::mat4 GetInvTransform() const
		{
			glm::mat4 inv_translation = glm::translate(glm::mat4(1.0f), -Translation);
			glm::quat inv_rotation = glm::conjugate(Rotation);
			glm::mat4 inv_scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / Scale.x, 1.0f / Scale.y, 1.0f / Scale.z));
			return inv_scale * glm::toMat4(inv_rotation) * inv_translation;
		}

		void SetRotation(float degrees)
		{
			float angleRad = glm::radians(degrees);
			float qw = cos(angleRad / 2.0f);
			float qz = sin(angleRad / 2.0f);
			Rotation = glm::quat(qw, 0.0f, 0.0f, qz);
		}

	};

}