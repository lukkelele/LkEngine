#pragma once

//#include <cstdint>
//#include <glm/glm.hpp>


namespace LkEngine {

    struct RendererSpecification
    {
        glm::vec4 InitialClearColor = { 0.30f, 0.45f, 0.10f, 1.0f };
        bool SwapChainTarget = false;
    };

    struct Renderer2DSpecification
    {
        uint32_t MaxQuads = 10000;
        uint32_t MaxLines = 1000;
        bool SwapChainTarget = false;
    };

}