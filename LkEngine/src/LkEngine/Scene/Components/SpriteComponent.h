#pragma once
#include "LkEngine/Core/Base.h"
#include <glm/glm.hpp>


namespace LkEngine {

    struct SpriteComponent
    {
        std::string FilePath;
        glm::vec2 Size;
        glm::vec4 Color; 

        SpriteComponent(const std::string& filepath, 
                        const glm::vec2& size = { 50.f, 50.f }, 
                        const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f })
            : FilePath(filepath)
            , Size(size)
            , Color(color) 
        {
        }

        SpriteComponent(const glm::vec2& size = { 50.f, 50.f },
                        const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f })
            : FilePath("")
            , Size(size)
            , Color(color) 
        {
        }

        float GetWidth() const { return Size.x; }
        float GetHeight() const { return Size.y; }
        glm::vec2 GetSize() const { return Size; }

    };

}