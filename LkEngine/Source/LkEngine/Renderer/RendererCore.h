#pragma once

#include "LkEngine/Core/CoreTypes.h"
#include "LkEngine/Core/Math/MathLibrary.h"


namespace LkEngine {

    struct FQuadVertex
	{
		glm::vec3 Position{};
		glm::vec4 Color{};
		glm::vec2 TexCoord{};
		float TexIndex{};
		float TexArray{};
		float TilingFactor{};
    };

    struct FLineVertex
    {
        glm::vec3 Position{};
        glm::vec4 Color{};
    };

    struct FRendererSpecification
    {
        glm::vec4 InitialClearColor = { 0.30f, 0.45f, 0.10f, 1.0f };
        bool bSwapChainTarget = false;
    };

    struct FRenderer2DSpecification
    {
        uint32_t MaxQuads = 10000;
        uint32_t MaxLines = 1000;
        bool bSwapChainTarget = false;

        uint8_t ArrayTexturesCount = 1;
    };

}
