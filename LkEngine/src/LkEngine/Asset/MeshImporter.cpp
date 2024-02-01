#include "LKpch.h"
#include "MeshImporter.h"

#include "LkEngine/Platform/OpenGL/LkOpenGL.h"

#include "LkEngine/Scene/Model.h"


namespace LkEngine {

    void MeshImporter::Load(std::filesystem::path filepath, Model* model)
    {
        m_ModelRef = model;
        model->m_Directory = filepath.string().substr(0, filepath.string().find_last_of('/'));

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filepath.string(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        ProcessNode(scene->mRootNode, scene, model->m_Meshes);
        //Load(scene, model);
    }

    void MeshImporter::Load(const aiScene* scene, Ref<Model>& model)
    {
        Load(scene, model.Raw());
    }

    void MeshImporter::Load(const aiScene* scene, Model* model)
    {
        m_ModelRef = model;
        ProcessNode(scene->mRootNode, scene, model->m_Meshes);
    }

	std::vector<Mesh> MeshImporter::Load(std::filesystem::path filepath)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filepath.string(), aiProcess_Triangulate | aiProcess_FlipUVs);

        LK_CORE_ASSERT(scene || scene->mFlags && AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode, "Assimp scene error!");

        std::vector<Mesh> meshes{};
		ProcessNode(scene->mRootNode, scene, meshes);

		return meshes;
	}

    void MeshImporter::LoadTextures(aiMesh* mesh, const aiScene* scene, std::vector<Texture_>& textures)
    {
        // Process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];    

        // We assume a convention for sampler names in the shaders
        // Each diffuse texture should be named as 'u_Diffuse' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        // Same applies to other texture as the following list summarizes:
        // Diffuse: u_Diffuse<N>
        // Specular: u_Specular<N>
        // Normal: u_Normal<N>
        // Height: u_Height<N>
    
        // 1. Diffuse maps
        {
            std::vector<Texture_> diffuseMaps = m_ModelRef->LoadMaterialTextures(material, aiTextureType_DIFFUSE, "u_Diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        }
        // 2. Specular maps
        {
            std::vector<Texture_> specularMaps = m_ModelRef->LoadMaterialTextures(material, aiTextureType_SPECULAR, "u_Specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }
        // 3. Normal maps
        {
            std::vector<Texture_> normalMaps = m_ModelRef->LoadMaterialTextures(material, aiTextureType_HEIGHT, "u_Normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        }
        // 4. Height maps
        {
            std::vector<Texture_> heightMaps = m_ModelRef->LoadMaterialTextures(material, aiTextureType_AMBIENT, "u_Height");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        }
    }

	Mesh MeshImporter::Process(aiMesh* mesh, const aiScene* scene)
	{
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture_> textures;
    
        // Walk through each of the mesh's faces and retrieve the corresponding vertex indices
        LoadVertices(mesh, vertices);
        LoadIndices(mesh, indices);
        LoadTextures(mesh, scene, textures);

		return Mesh(vertices, indices, textures);
	}

    // Processes each individual mesh located at the node and repeats this process on its children nodes (if any)
    void MeshImporter::ProcessNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes)
    {
        // Process each mesh located at the current node
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // The node object only contains indices to index the actual objects in the scene
            // The scene contains all the data, node is just to keep stuff organized (like relations between nodes)
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(Process(mesh, scene));
        }

        // After we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene, meshes);
        }
    }

    void MeshImporter::LoadVertices(aiMesh* mesh, std::vector<Vertex>& vertices)
    {
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
                glm::vec2 vec{};

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
    }

    void MeshImporter::LoadIndices(aiMesh* mesh, std::vector<unsigned int>& indices)
    {
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);        
        }
    }

    namespace GLUtils {

        unsigned int TextureFromFile(const char* path, const std::string& m_Directory, bool gamma)
        {
            std::string filename = std::string(path);
            filename = m_Directory + '/' + filename;
        
            unsigned int textureID;
            glGenTextures(1, &textureID);
        
            int width, height, nrComponents;
            unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
            if (data)
            {
                GLenum format;
                if (nrComponents == 1)      format = GL_RED;
                else if (nrComponents == 3) format = GL_RGB;
                else if (nrComponents == 4) format = GL_RGBA;
        
                glBindTexture(GL_TEXTURE_2D, textureID);
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
        
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
                stbi_image_free(data);
            }
            else
            {
                LK_CORE_ASSERT(false, "Texture failed to load at path: {}", path);
                stbi_image_free(data);
            }
            return textureID;
        }
    }



}