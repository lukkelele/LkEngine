#include "LKpch.h"
#include "Mesh.h"

#include "LkEngine/Platform/OpenGL/LkOpenGL.h"


namespace LkEngine {

    Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture_> textureIDs)
    {
        m_Vertices = vertices;
        m_TextureIDs = textureIDs;
    
		m_VertexBuffer = VertexBuffer::Create(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));
		m_VertexBuffer->SetLayout({
            { "a_Pos",          ShaderDataType::Float3   },
            { "a_Normal",       ShaderDataType::Float3   },
            { "a_TexCoord",     ShaderDataType::Float2   },
            { "a_Tangent",      ShaderDataType::Float3   },
            { "a_Bitangent",    ShaderDataType::Float3   },
            { "a_BoneIDs",      ShaderDataType::Int4     },
            { "a_Weights",      ShaderDataType::Float4   },
		});
        Ref<IndexBuffer> IB = IndexBuffer::Create(indices.data(), indices.size() * sizeof(unsigned int));
        m_VertexBuffer->SetIndexBuffer(IB);
    }

    Mesh::~Mesh()
    {
    }

    //void Mesh::Draw(Ref<Shader>& shader) 
    void Mesh::Draw()
    {
        // Bind appropriate m_TextureIDs
        unsigned int diffuseNr  = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr   = 1;
        unsigned int heightNr   = 1;

        for (unsigned int i = 0; i < m_TextureIDs.size(); i++)
        {
            // retrieve texture number (the N in diffuse_textureN)
            std::string number;
            std::string name = m_TextureIDs[i].type;

            if (name == "u_Diffuse")       number = std::to_string(diffuseNr++);  // Diffuse
            else if (name == "u_Specular") number = std::to_string(specularNr++); // Specular
            else if (name == "u_Normal")   number = std::to_string(normalNr++);   // Normal
            else if (name == "u_Height")   number = std::to_string(heightNr++);   // Height
    
            // Now set the sampler to the correct texture unit and bind texture
            //shader->Bind();
            //shader->Set(std::string(name + number), i);

            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            glBindTexture(GL_TEXTURE_2D, m_TextureIDs[i].id);
        }
        
        m_VertexBuffer->Bind();
        glDrawElements(GL_TRIANGLES, m_VertexBuffer->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, 0);

        glActiveTexture(GL_TEXTURE0);
    }

}