#include "LKpch.h"
#include "LkEngine/Renderer/Color.h"


namespace LkEngine {

    bool Color::m_Initialized = false;
    std::mt19937 Color::RNG(std::time(0));
    std::uniform_int_distribution<int> Color::Dist(0, 100);

    glm::vec4 Color::Red   = { 1.0f, 0.0f, 0.0f, 1.0f };
    glm::vec4 Color::Green = { 0.0f, 1.0f, 0.0f, 1.0f };
    glm::vec4 Color::Blue  = { 0.0f, 0.0f, 1.0f, 1.0f };

    void Color::Init()
    {
        //RNG = std::mt19937(std::time(0));
        //Dist = std::uniform_int_distribution(0, 100);
        m_Initialized = true;
    }

    void Color::Randomize(glm::vec4& color)
    {
        static bool initialized = false;
        if (!initialized)
        {
            Color::Init();
            initialized = true;
        }
	    color.x = (float)(Dist(RNG) * 0.01f);
	    color.y = (float)(Dist(RNG) * 0.01f);
	    color.z = (float)(Dist(RNG) * 0.01f);
        color.w = 1.0f;
    }

    glm::vec4 Color::Generate()
    {
        glm::vec4 color;
        Randomize(color);
        return color;
    }


}