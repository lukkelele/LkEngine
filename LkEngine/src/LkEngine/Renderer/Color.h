#pragma once
#include <glm/glm.hpp>
#include <random>
#include <time.h>


namespace LkEngine {

    class Color
    {
    public:
        Color() = default;
        ~Color() = default;

        static void Init();
        static void Randomize(glm::vec4& color);
        static glm::vec4 Generate();

    private:
        static bool m_Initialized;
        static std::mt19937 RNG;
        static std::uniform_int_distribution<int> Dist;

        static glm::vec4 Red;
        static glm::vec4 Green;
        static glm::vec4 Blue;
    };


}