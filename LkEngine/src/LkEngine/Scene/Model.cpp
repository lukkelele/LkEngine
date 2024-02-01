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
        //meshImporter.Load(scene->mRootNode, scene, this);
        meshImporter.Load(scene, this);
    }

    void Model::Draw(Ref<Shader> shader)
    {
        for (unsigned int i = 0; i < m_Meshes.size(); i++)
        {
            m_Meshes[i].Draw();
        }
    }

    void Model::LoadModel(const std::string& path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        // Check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
        {
            //LK_CORE_ERROR("ERROR::ASSIMP:: {}", importer.GetErrorString());
            //return;
            LK_CORE_ASSERT(false, "Assimp error, {}", importer.GetErrorString());
        }

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
        {
            ProcessNode(node->mChildren[i], scene);
        }
    }

    Mesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture_> textures;
    
        // Walk through each of the mesh's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex{};
            glm::vec3 vector{};

            // Positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            // Normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }

            // Texture Coordinates
            if(mesh->mTextureCoords[0]) 
            {
                glm::vec2 vec;

                // Assume texture with 2 coordinates
                vec.x = mesh->mTextureCoords[0][i].x; 
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;

                // Tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;

                // Bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
            else
            {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }
    
            vertices.push_back(vertex);
        }

        // Walk through each of the mesh's faces and retrieve the corresponding vertex indices
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];

            // Retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);        
        }

        // Process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];    

        // We assume a convention for sampler names in the shaders. 
        // Each diffuse texture should be named as 'u_Diffuse' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        // Same applies to other texture as the following list summarizes:
        // Diffuse: u_Diffuse<N>
        // Specular: u_Specular<N>
        // Normal: u_Normal<N>
        // Height: u_Height<N>
    
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

    // FIXME:
    // Checks all material textures of a given type and loads the textures if they're not loaded yet
    // the required info is returned as a Texture struct
    std::vector<Texture_> Model::LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
    {
        std::vector<Ref<Texture>> modelTextures;
        std::vector<Texture_> textures;
        auto textureLib = TextureLibrary::Get();

        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString textureFilename;
            mat->GetTexture(type, i, &textureFilename);
            LK_CORE_DEBUG_TAG("Model", "Assimp texture loaded -> {}", textureFilename.C_Str());

            // Check if it exists in library
            //if (Ref<Texture> loadedTexture = textureLib->TryToGetTextureWithFilename(File::ExtractFilenameWithoutExtension(textureFilename.C_Str())))
            Ref<Texture> loadedTexture = textureLib->TryToGetTextureWithFilename(textureFilename.C_Str());
            if (loadedTexture)
            {
                // Load it since it already exists
                LK_CORE_TRACE_TAG("Model", "Found texture with name {}", loadedTexture->GetName());
                modelTextures.push_back(loadedTexture);
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
            texture.type = typeName;
            texture.path = textureFilename.C_Str();
            textures.push_back(texture);
            m_TexturesLoaded.push_back(texture);  // Store it as texture loaded for entire model
        }

        LK_WARN("MODEL, RETRIEVED A TOTAL OF {} ", modelTextures.size());
        return textures;
    }

}