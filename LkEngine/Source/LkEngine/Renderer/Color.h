#pragma once

#include "LkEngine/Core/Math/Math.h"


namespace LkEngine::Color {

    extern std::mt19937 RNG;
    extern std::uniform_int_distribution<int> Dist;

    glm::vec4 Generate(float alpha = 1.0f);
    void Randomize(glm::vec4& color, float alpha = 1.0f);

    namespace RGBA 
    {
        extern glm::vec4 Transparent;
        extern glm::vec4 White;
        extern glm::vec4 Black;
        extern glm::vec4 Gray;
        extern glm::vec4 Red;
        extern glm::vec4 Green;
        extern glm::vec4 Blue;
    }

}