#pragma once

#include "LkEngine/Core/Layer.h"
#include "LkEngine/Renderer/Color.h"

#include <glm/glm.hpp>


namespace LkEngine {

    // Forward declaration
    class Scene;
    class Entity;

    class DebugLayer : public Layer
    {
    public:
        DebugLayer();
        ~DebugLayer() = default;

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(float ts) override;
        void OnImGuiRender() override;

    public:
        inline static int s_DebugEntities = 0;
    };

    namespace Debug {
        static void PrintVec2(const glm::vec2& vec, const std::string& message = "")
        {
            printf("[DEBUG] Vec2 (%f, %f): %s\n", vec.x, vec.y, message.c_str());
        }

        static void PrintVec3(const glm::vec3& vec, const std::string& message = "")
        {
            printf("[DEBUG] Vec3 (%f, %f, %f): %s\n", vec.x, vec.y, vec.z, message.c_str());
        }

        Entity CreateDebugSprite(Scene& scene, 
                                 const glm::vec2& size = { 100.0f, 100.0f },
                                 const glm::vec2& pos = { 0.0f, 0.0f },
                                 const glm::vec4& color = Color::Generate());

        Entity CreateDebugSprite(Scene& scene, 
                                 const glm::vec2& size = { 100.0f, 100.0f },
                                 const glm::vec3& pos = { 0.0f, 0.0f, 0.0f },
                                 const glm::vec4& color = Color::Generate());


    }

}