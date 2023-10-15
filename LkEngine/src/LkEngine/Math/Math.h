#pragma once
#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif
#include <glm/glm.hpp>
#include <glm/ext/matrix_common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/norm.hpp>


namespace LkEngine::Math {

    glm::vec3 Scale(glm::vec3& vec, float scale_factor);
    glm::mat4 TransformMatrix(glm::vec3& translation, glm::quat& rotation, glm::vec3 scale);
    glm::vec4 ConvertToNDC(glm::vec3& world_pos, glm::mat4& mvp);
    glm::vec2 ConvertToWorldCoords(glm::vec2 ndc_coords, int window_width, int window_height);

}
