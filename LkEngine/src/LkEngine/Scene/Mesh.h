#pragma once

#include "LkEngine/Renderer/VertexBuffer.h"
#include "LkEngine/Renderer/Shader.h"


namespace LkEngine {

    struct Texture_ 
    {
        unsigned int id;
        std::string type;
        std::string path;
    };

    class Texture;
    
    class Mesh
    {
    public:  
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture_> textureIDs);
        ~Mesh();

        void Draw(Ref<Shader>& shader);
    
    private:
        Ref<VertexBuffer> m_VertexBuffer;
        std::vector<Vertex> m_Vertices;

        std::vector<Texture_> m_TextureIDs;

        std::vector<Ref<Texture>> m_Textures{};
    };

}
