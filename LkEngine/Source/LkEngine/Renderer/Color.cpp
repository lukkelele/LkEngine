#include "LKpch.h"
#include "LkEngine/Renderer/Color.h"


namespace LkEngine::Color {

    std::mt19937 Color::RNG(std::time(0));
    std::uniform_int_distribution<int> Color::Dist(0, 100);

    void Randomize(glm::vec4& color, float alpha)
    {
	    color.x = (float)(Dist(RNG) * 0.01f);
	    color.y = (float)(Dist(RNG) * 0.01f);
	    color.z = (float)(Dist(RNG) * 0.01f);
        color.w = alpha;
    }

}
