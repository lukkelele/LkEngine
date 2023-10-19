#pragma once
#include "LkEngine/Core/Base.h"
#include <glm/glm.hpp>


namespace LkEngine {

    struct SpriteComponent
    {
        std::string FilePath;
        glm::vec2 Size;
        glm::vec4 Color; // Should be tintcolor or something like that in the future, 
                         // so multiple attribute aren't identical across components

        SpriteComponent(const std::string& filepath, const glm::vec2& size, const glm::vec4& color)
            : FilePath(filepath)
            , Size(size)
            , Color(color) 
        {}

    };

}