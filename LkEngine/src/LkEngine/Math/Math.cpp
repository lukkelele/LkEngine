#include "LKpch.h"
#include "LkEngine/Math/Math.h"
#include <imgui/imgui_internal.h>
#include "LkEngine/UI/DockSpace.h"
#include "LkEngine/Scene/Components/TransformComponent.h"


namespace LkEngine::Math {

    glm::vec3 Scale(glm::vec3& vec, float scale_factor)
    {
        return (vec * scale_factor) / glm::length(vec);
    }

	glm::mat4 TransformMatrix(glm::vec3& translation, glm::quat& rotation, const glm::vec3& scale) 
	{
		return glm::translate(glm::mat4(1.0f), translation)
			 * glm::toMat4(rotation)
			 * glm::scale(glm::mat4(1.0f), scale);
	}

	glm::mat4 TransformMatrix2D(const glm::vec3& translation, float rot, const glm::vec3& scale)
	{
		glm::mat4 transform = glm::mat4(1.0f);  // initialize to identity matrix
		transform = glm::translate(transform, translation);
		transform = glm::rotate(transform, glm::radians(rot), glm::vec3(0.0f, 0.0f, 1.0f));
		transform = glm::scale(transform, scale);
		return transform;
	}

	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale)
	{
		using namespace glm;
		using T = float;

		mat4 localMatrix(transform);

		if (epsilonEqual(localMatrix[3][3], static_cast<T>(0), epsilon<T>()))
			return false;

		// Next take care of translation (easy).
		translation = vec3(localMatrix[3]);
		localMatrix[3] = vec4(0, 0, 0, localMatrix[3].w);

		vec3 Row[3] = {};
		// Now get scale and shear.
		for (length_t i = 0; i < 3; ++i)
			for (length_t j = 0; j < 3; ++j)
				Row[i][j] = localMatrix[i][j];

		// Compute X scale factor and normalize first row.
		scale.x = length(Row[0]);
		Row[0] = LkEngine::Math::Scale(Row[0], static_cast<T>(1));

		// Now, compute Y scale and normalize 2nd row.
		scale.y = length(Row[1]);
		Row[1] = LkEngine::Math::Scale(Row[1], static_cast<T>(1));

		// Next, get Z scale and normalize 3rd row.
		scale.z = length(Row[2]);
		Row[2] = LkEngine::Math::Scale(Row[2], static_cast<T>(1));

		// Rotation as quaternion
		int i, j, k = 0;
		T root, trace = Row[0].x + Row[1].y + Row[2].z;
		if (trace > static_cast<T>(0))
		{
			root = sqrt(trace + static_cast<T>(1));
			rotation.w = static_cast<T>(0.5) * root;
			root = static_cast<T>(0.5) / root;
			rotation.x = root * (Row[1].z - Row[2].y);
			rotation.y = root * (Row[2].x - Row[0].z);
			rotation.z = root * (Row[0].y - Row[1].x);
		} // End if > 0
		else
		{
			static int Next[3] = { 1, 2, 0 };
			i = 0;
			if (Row[1].y > Row[0].x) i = 1;
			if (Row[2].z > Row[i][i]) i = 2;
			j = Next[i];
			k = Next[j];

			root = sqrt(Row[i][i] - Row[j][j] - Row[k][k] + static_cast<T>(1.0));

			rotation[i] = static_cast<T>(0.5) * root;
			root = static_cast<T>(0.5) / root;
			rotation[j] = root * (Row[i][j] + Row[j][i]);
			rotation[k] = root * (Row[i][k] + Row[k][i]);
			rotation.w = root * (Row[j][k] - Row[k][j]);
		} // End if <= 0

		return true;
	}

	glm::vec4 ConvertToNDC(glm::vec3& world_pos, glm::mat4& mvp)
	{
		glm::vec4 ndc(0.0f);
		return ndc;
	}

	glm::vec2 ConvertToWorldCoords(glm::vec2 ndc_coords, int window_width, int window_height)
	{
		glm::vec4 converted_coords(
			((float)ndc_coords.x / (float)window_width - 0.5f) * 2.0f,  // [0,  width] -> [-1,1]
			((float)ndc_coords.y / (float)window_height - 0.5f) * 2.0f, // [0, height] -> [-1,1]
			-1.0, // The near plane maps to Z=-1
			1.0f
		);
		return glm::vec2(converted_coords.x, converted_coords.y);
	}


    glm::vec2 WorldToPos(const glm::vec3& world_pos, const glm::mat4& mat)
    {
        //vec_t trans;
        //trans.TransformPoint(worldPos, mat);
        //trans *= 0.5f / trans.w;
        //trans += makeVect(0.5f, 0.5f);
        //trans.y = 1.f - trans.y;
        //trans.x *= size.x;
        //trans.y *= size.y;
        //trans.x += position.x;
        //trans.y += position.y;
        //return ImVec2(trans.x, trans.y);
        return { 0, 0 };
    }

    glm::vec2 ScreenToWorld(const glm::vec2& screenCoords, const glm::mat4& inverseProjectionMatrix, const glm::mat4& inverseViewMatrix)
    {
        return glm::vec2();
    }

    glm::vec2 ScreenToWorld2D(const glm::vec2& screenCoords, const glm::mat4& inverseProjectionMatrix, const glm::mat4& inverseViewMatrix)
    {
        // Convert screen coordinates to normalized device coordinates
        auto& io = ImGui::GetIO();
        auto window = ImGui::DockBuilderGetNode(LkEngine::DockSpace::RenderWindowDockID);
        int width = window->Size.x;
        int height = window->Size.y;
        glm::vec4 clipCoords = glm::vec4(
            2.0f * screenCoords.x / width - 1.0f,
            1.0f - 2.0f * screenCoords.y / height, 
            0.0f, 
            1.0f
        );

        // Multiply by inverse projection matrix
        glm::vec4 eyeCoords = inverseProjectionMatrix * clipCoords;
        eyeCoords.z = -1.0f; // Point into the scene
        eyeCoords.w = 0.0f;

        // Convert to world coordinates
        glm::vec4 worldCoords = inverseViewMatrix * eyeCoords;
        return glm::vec2(worldCoords.x, worldCoords.y);
    }


    glm::vec2 ScreenToWorld2D(const glm::vec2& ndc, const glm::mat4& inv_proj, const TransformComponent& transform)
    {
        // Convert 2D NDC to homogeneous clip coordinates
        glm::vec4 clipCoords(ndc.x, ndc.y, 0.0f, 1.0f);

        // Convert from clip space to eye/camera space using inverse projection
        glm::vec4 eyeCoords = inv_proj * clipCoords;

        // Convert from eye/camera space to world space using inverse transform
        glm::vec4 worldCoords = transform.GetInvTransform() * eyeCoords;

        return glm::vec2(worldCoords.x, worldCoords.y);
    }

    float Get2DRotationFromQuaternion(const glm::quat& quat)
    {
        glm::quat norm_quat = glm::normalize(quat);

        // Compute the angle of rotation around the Z-axis
        float angle = std::atan2(
            2.0f * (norm_quat.w * norm_quat.z + norm_quat.x * norm_quat.y),
            1.0f - 2.0f * (norm_quat.y * norm_quat.y + norm_quat.z * norm_quat.z)
        );

        // Convert angle from radians to degrees if needed
        float angle_deg = glm::degrees(angle);

        return angle_deg;
    }



}