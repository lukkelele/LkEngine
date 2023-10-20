#pragma once
#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

    class Renderer2D
    {
    public:
        Renderer2D() = default;
        ~Renderer2D() = default;


        static void DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color);
    };

}