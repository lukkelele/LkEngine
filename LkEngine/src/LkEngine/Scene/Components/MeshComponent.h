#pragma once
#include "LkEngine/Core/Base.h"
#include "LkEngine/Core/UUID.h"
#include "LkEngine/Renderer/VertexArray.h"
#include <glm/glm.hpp>
#include "LkEngine/Renderer/Shader.h"


namespace LkEngine {


    struct MeshComponent
    {
        // TODO: Move these and color to something else, for reuse of VAO, VBO and IBO's
        s_ptr<VertexArray> VAO = nullptr;
        s_ptr<VertexBuffer> VBO = nullptr;
        s_ptr<Shader> BaseShader = nullptr;
        glm::vec4 Color = { 0.30f , 0.630f, 0.80f, 1.0f };
        bool Dragged = false;

        MeshComponent() { }
        ~MeshComponent() { }
    };

}