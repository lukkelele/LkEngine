#pragma once

#include "LkEngine/Core/Base.h"
#include "LkEngine/Core/UUID.h"
#include "LkEngine/Renderer/VertexBuffer.h"
#include "LkEngine/Renderer/Shader.h"

#include <glm/glm.hpp>


namespace LkEngine {

    // TODO: Redo this entire implementation, is not used rn
    struct MeshComponent
    {
        s_ptr<VertexBuffer> VBO = nullptr;
        s_ptr<Shader> BaseShader = nullptr;
        glm::vec4 Color = { 0.30f , 0.630f, 0.80f, 1.0f };
        bool Dragged = false;

        MeshComponent() { }
        ~MeshComponent() { }
    };

}