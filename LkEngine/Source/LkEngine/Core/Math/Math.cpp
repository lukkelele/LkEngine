#include "LKpch.h"
#include "Math.h"

#include "LkEngine/Scene/Components.h"
#include "LkEngine/Core/Window.h"

namespace LkEngine::Math {

	glm::mat4 TransformMatrix2D(const glm::vec3& translation, float rot, const glm::vec3& scale)
	{
		glm::mat4 transform = glm::mat4(1.0f);  // initialize to identity matrix
		transform = glm::translate(transform, translation);
		transform = glm::rotate(transform, glm::radians(rot), glm::vec3(0.0f, 0.0f, 1.0f));
		transform = glm::scale(transform, scale);

		return transform;
	}

	float Get2DRotationFromQuaternion(const glm::quat& quat)
	{
		glm::quat norm_quat = glm::normalize(quat);

		// Compute the angle of Rotation around the Z-axis
		float angle = std::atan2(
			2.0f * (norm_quat.w * norm_quat.z + norm_quat.x * norm_quat.y),
			1.0f - 2.0f * (norm_quat.y * norm_quat.y + norm_quat.z * norm_quat.z)
		);

		// Convert angle from radians to degrees if needed
		float angle_deg = glm::degrees(angle);

		return angle_deg;
	}

}
