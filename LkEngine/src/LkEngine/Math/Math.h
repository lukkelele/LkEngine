#pragma once
#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif
#include <glm/glm.hpp>
#include <glm/ext/matrix_common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/norm.hpp>
#include <imgui/imgui.h>

namespace LkEngine {
    struct TransformComponent;
}

namespace LkEngine::Math {

    glm::vec3 Scale(glm::vec3& vec, float scale_factor);
    glm::mat4 TransformMatrix(glm::vec3& translation, glm::quat& rotation, glm::vec3& scale);
    glm::mat4 TransformMatrix2D(const glm::vec3& translation, float rot, const glm::vec3& scale);
    glm::vec4 ConvertToNDC(glm::vec3& world_pos, glm::mat4& mvp);
    glm::vec2 ConvertToWorldCoords(glm::vec2 ndc_coords, int window_width, int window_height);
    glm::vec2 WorldToPos(const glm::vec3& world_pos, const glm::mat4& mat);

    glm::vec2 ScreenToWorld2D(const glm::vec2& screenCoords, const glm::mat4& inverseProjectionMatrix, const glm::mat4& inverseViewMatrix);
    glm::vec2 ScreenToWorld(const glm::vec2& screenCoords, const glm::mat4& inverseProjectionMatrix, const glm::mat4& inverseViewMatrix);
    glm::vec2 ScreenToWorld2D(const glm::vec2& ndc, const glm::mat4& inv_proj, const TransformComponent& transform);

    float Get2DRotationFromQuaternion(const glm::quat& quat);

    bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::quat& rot, glm::vec3& scale);


}
