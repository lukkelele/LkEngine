#include "LKpch.h"
#include "Math.h"

#include "LkEngine/Scene/Components.h"
#include "LkEngine/Core/Window.h"

#include "LkEngine/Physics/2D/Physics2D.h"


namespace LkEngine::Math {

	glm::mat4 TransformMatrix2D(const glm::vec3& translation, float rot, const glm::vec3& scale)
	{
		glm::mat4 transform = glm::mat4(1.0f);  // initialize to identity matrix
		transform = glm::translate(transform, translation);
		transform = glm::rotate(transform, glm::radians(rot), glm::vec3(0.0f, 0.0f, 1.0f));
		transform = glm::scale(transform, scale);

		return transform;
	}

#if false
	glm::vec2 ScreenToWorld2D(const glm::vec2& ScreenCoordinates, 
							  const glm::mat4& InverseProjectionMatrix, 
							  const glm::mat4& InverseViewMatrix)
	{
		// Convert screen coordinates to normalized device coordinates.

		LWindow& Window = LWindow::Get();
		int Width = Window.GetWidth();
		int Height = Window.GetHeight();
		glm::vec4 ClipCoordinates = glm::vec4(
			2.0f * ScreenCoordinates.x / Width - 1.0f,
			1.0f - 2.0f * ScreenCoordinates.y / Height,
			0.0f,
			1.0f
		);

		// Multiply by inverse projection matrix.
		glm::vec4 EyeCoordinates = InverseProjectionMatrix * ClipCoordinates;
		EyeCoordinates.z = -1.0f; // Point into the scene
		EyeCoordinates.w = 0.0f;

		// Convert to world coordinates
		glm::vec4 WorldCoordinates = InverseViewMatrix * EyeCoordinates;

		return glm::vec2(WorldCoordinates.x, WorldCoordinates.y);
	}

	glm::vec2 ScreenToWorld2D(const glm::vec2& ndc, const glm::mat4& inv_proj, const LTransformComponent& Transform)
	{
		// Convert 2D NDC to homogeneous clip coordinates
		glm::vec4 ClipCoordinates(ndc.x, ndc.y, 0.0f, 1.0f);

		// Convert from clip space to eye/camera space using inverse projection
		glm::vec4 EyeCoordinates = inv_proj * ClipCoordinates;

		// Convert from eye/camera space to world space using inverse transform
		glm::vec4 WorldCoordinates = Transform.GetInvTransform() * EyeCoordinates;

		return glm::vec2(WorldCoordinates.x, WorldCoordinates.y);
	}
#endif

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
