#pragma once
#include "LkEngine/Core/Base.h"
#include "LkEngine/Core/UUID.h"
#include "LkEngine/Renderer/VertexArray.h"
#include <glm/glm.hpp>


namespace LkEngine {

    struct Vertex
    {
        glm::vec3 Pos;
        glm::vec3 Normal;
        glm::vec2 UV;

        Vertex(glm::vec3 pos, glm::vec2 uv) : Pos(pos), UV(uv)
        {
        }
    };

    struct MeshComponent
    {
        MeshComponent() 
            : Vertices({})
        {
        }

        ~MeshComponent()
        {
            Vertices.clear();
            Indices.clear();
        }

        void AddVertex(const glm::vec2& pos, const glm::vec2& uv) { Vertices.push_back(Vertex(glm::vec3(pos, 0.0f), uv)); }
        void AddVertex(const glm::vec3 pos, const glm::vec2& uv)  { Vertices.push_back(Vertex(pos, uv)); }
        void AddIndex(unsigned int index) { Indices.push_back(index); }

        std::vector<Vertex> Vertices;
        std::vector<unsigned int> Indices;

        // TODO: Move these and color to something else, for reuse of VAO, VBO and IBO's
        s_ptr<VertexArray> VAO = nullptr;
        s_ptr<VertexBuffer> VBO = nullptr;
        s_ptr<IndexBuffer> IBO = nullptr;
        s_ptr<Shader> Shader = nullptr;
        glm::vec4 Color = { 0.30f , 0.630f, 0.80f, 1.0f };
    };

}