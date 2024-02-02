#include "LKpch.h"
#include "Model.h"

#include "LkEngine/Platform/OpenGL/LkOpenGL.h"

#include "LkEngine/Asset/MeshImporter.h"

#include "LkEngine/Renderer/TextureLibrary.h"


namespace LkEngine {

    Model::Model()
        : m_Meshes({})
        , m_GammaCorrection(false)
    {
    }

    Model::Model(const std::string& path, bool gammaCorrection) 
        : m_GammaCorrection(gammaCorrection)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        MeshImporter meshImporter = MeshImporter();

        m_Directory = path.substr(0, path.find_last_of('/'));
        meshImporter.Load(scene, this);
    }

    void Model::Draw(Ref<Shader>& shader)
    {
        for (unsigned int i = 0; i < m_Meshes.size(); i++)
        {
            m_Meshes[i].Draw(shader);
        }
    }

    void Model::LoadModel(const std::string& path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        // Check for errors
        LK_CORE_ASSERT(!scene || scene->mFlags && AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode, "Assimp error, {}", importer.GetErrorString());

        m_Directory = path.substr(0, path.find_last_of('/'));
        ProcessNode(scene->mRootNode, scene);
    }
    
    // Processes each individual mesh located at the node and repeats this process on its children nodes (if any)
    void Model::ProcessNode(aiNode *node, const aiScene *scene)
    {
        // Process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // The node object only contains indices to index the actual objects in the scene
            // The scene contains all the data, node is just to keep stuff organized (like relations between nodes)
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            m_Meshes.push_back(ProcessMesh(mesh, scene));
        }

        // After we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
            ProcessNode(node->mChildren[i], scene);
    }

    Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture_> textures;
    
        MeshImporter::LoadVertices(mesh, vertices);
        MeshImporter::LoadIndices(mesh, indices);

        // Process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];    

        // 1. Diffuse maps
        {
            std::vector<Texture_> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "u_Diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        }
        // 2. Specular maps
        {
            std::vector<Texture_> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "u_Specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }
        // 3. Normal maps
        {
            std::vector<Texture_> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "u_Normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        }
        // 4. Height maps
        {
            std::vector<Texture_> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "u_Height");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        }

        return Mesh(vertices, indices, textures);
    }

    std::vector<Texture_> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string uniformName)
    {
        std::vector<Ref<Texture>> modelTextures;
        std::vector<Texture_> textures;
        auto textureLib = TextureLibrary::Get();

        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString textureFilename;
            mat->GetTexture(type, i, &textureFilename);

            // Check if it exists in library
            Ref<Texture> loadedTexture = textureLib->TryToGetTextureWithFilename(textureFilename.C_Str());
            if (loadedTexture)
            {
                // Load it since it already exists
                //LK_CORE_TRACE_TAG("Model", "Found texture with name {}", loadedTexture->GetName());
                modelTextures.push_back(loadedTexture);
            }
            // Proceed to create the texture
            else
            {
                TextureSpecification matTextureSpec;
                matTextureSpec.Name = type;
                matTextureSpec.DebugName = type;
                matTextureSpec.UniformType = Utils::GetTextureUniformType(type);

                //TextureUniformType uniformType = Utils::GetTextureUniformType(type);
                Ref<Texture> materialTexture = Texture::Create(matTextureSpec);
                LK_CORE_INFO_TAG("Model", "Created texture: name={}  filepath={}  width={}  height={}", materialTexture->GetName(), materialTexture->GetSpecification().Path, materialTexture->GetWidth(), materialTexture->GetHeight());
                //m_MaterialTextures.insert({ materialTexture, uniformType });
                m_Textures.push_back(materialTexture);

                modelTextures.push_back(materialTexture);
            }

            for (unsigned int j = 0; j < m_TexturesLoaded.size(); j++)
            {
                if (std::strcmp(m_TexturesLoaded[j].path.data(), textureFilename.C_Str()) == 0)
                {
                    textures.push_back(m_TexturesLoaded[j]);
                    continue;
                }
            }

            Texture_ texture;
            texture.id = GLUtils::TextureFromFile(textureFilename.C_Str(), m_Directory);
            texture.type = uniformName;
            texture.path = textureFilename.C_Str();
            textures.push_back(texture);
            m_TexturesLoaded.push_back(texture);  // Store it as texture loaded for entire model
            //LK_CORE_INFO("Texture.Type={}", uniformName);
        }

        //LK_WARN("MODEL, RETRIEVED A TOTAL OF {} ", modelTextures.size());
        return textures;
    }

}