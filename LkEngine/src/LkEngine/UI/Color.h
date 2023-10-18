#pragma once
#include <glm/glm.hpp>


namespace LkEngine::UI {


    class Color
    {
    public:
        Color() = default;
        ~Color() = default;


    public:
        static glm::vec4 Red;
        static glm::vec4 Green;
        static glm::vec4 Blue;


    };

    glm::vec4 Color::Red   = { 1.0f, 0.0f, 0.0f, 1.0f };
    glm::vec4 Color::Green = { 0.0f, 1.0f, 0.0f, 1.0f };
    glm::vec4 Color::Blue  = { 0.0f, 0.0f, 1.0f, 1.0f };

}