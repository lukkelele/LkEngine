#include "LKpch.h"
#include "LkEngine/Renderer/Color.h"


namespace LkEngine::Color {

    std::mt19937 Color::RNG(std::time(0));
    std::uniform_int_distribution<int> Color::Dist(0, 100);

    glm::vec4 Generate(float alpha)
    {
        glm::vec4 color;
        Randomize(color, alpha);
        return color;
    }

    void Randomize(glm::vec4& color, float alpha)
    {
	    color.x = (float)(Dist(RNG) * 0.01f);
	    color.y = (float)(Dist(RNG) * 0.01f);
	    color.z = (float)(Dist(RNG) * 0.01f);
        color.w = alpha;
    }

    // RGBA Colors
    glm::vec4 Color::RGBA::Transparent = { 1.0f, 1.0f, 1.0f, 0.0f };
    glm::vec4 Color::RGBA::White       = { 1.0f, 1.0f, 1.0f, 1.0f };
    glm::vec4 Color::RGBA::Black       = { 0.0f, 0.0f, 0.0f, 1.0f };
    glm::vec4 Color::RGBA::Gray        = { 0.3f, 0.3f, 0.3f, 1.0f };
    glm::vec4 Color::RGBA::Red         = { 1.0f, 0.0f, 0.0f, 1.0f };
    glm::vec4 Color::RGBA::Green       = { 0.0f, 1.0f, 0.0f, 1.0f };
    glm::vec4 Color::RGBA::Blue        = { 0.0f, 0.0f, 1.0f, 1.0f };

}