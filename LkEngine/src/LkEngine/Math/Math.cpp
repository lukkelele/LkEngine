#include "LKpch.h"
#include "LkEngine/Math/Math.h"


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


}