#pragma once
#include "LkEngine/Core/Base.h"
#include "LkEngine/Core/UUID.h"
#include "LkEngine/Renderer/VertexArray.h"
#include <glm/glm.hpp>
#include "LkEngine/Renderer/Shader.h"


namespace LkEngine {

    struct Vertex
    {
        glm::vec3 Pos;
        glm::vec3 Normal;
        glm::vec2 UV;
    };

    struct MeshComponent
    {
        // TODO: Move these and color to something else, for reuse of VAO, VBO and IBO's
        s_ptr<VertexArray> VAO = nullptr;
        s_ptr<VertexBuffer> VBO = nullptr;
        s_ptr<IndexBuffer> IBO = nullptr;
        s_ptr<Shader> BaseShader = nullptr;
        glm::vec4 Color = { 0.30f , 0.630f, 0.80f, 1.0f };

        std::vector<Vertex> Vertices;
        std::vector<unsigned int> Indices;

        MeshComponent() 
            : Vertices({})
        {
        }

        ~MeshComponent()
        {
            Vertices.clear();
            Indices.clear();
        }

        void AddVertex(const glm::vec2& pos, const glm::vec2& uv) { }
        void AddVertex(const glm::vec3 pos, const glm::vec2& uv) { }
        void AddIndex(unsigned int index) { Indices.push_back(index); }
    };

}