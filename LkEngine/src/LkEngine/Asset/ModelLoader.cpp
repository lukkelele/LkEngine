#include "LKpch.h"
#include "ModelLoader.h"

#include "MeshImporter.h"


namespace LkEngine {

	ModelLoader::ModelLoader(std::filesystem::path filepath)
	{
	}

	Model ModelLoader::Load(std::filesystem::path filepath)
	{
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filepath.string(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        LK_CORE_ASSERT(scene || scene->mFlags && AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode, "Assimp error, {}", importer.GetErrorString());

        //std::string directory = filepath.string().substr(0, filepath.string().find_last_of('/'));
        //ProcessNode(scene->mRootNode, scene);

        //Model model = Model();
        //std::vector<Mesh> meshes;
        //meshImporter.ProcessNode(scene->mRootNode, scene, meshes);
        //std::vector<Mesh> meshes = meshImporter.Load(scene->mRootNode, scene, model);

        Model model;
        MeshImporter meshImporter;
        // Load meshes to model
        meshImporter.Load(scene->mRootNode, scene, model);

        return model;
	}

    Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene *scene)
    {
        MeshImporter meshImporter;

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture_> textures;

        meshImporter.LoadVertices(mesh, vertices);
        meshImporter.LoadIndices(mesh, indices);

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
    std::vector<Texture_> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
    {
        std::vector<Texture_> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);

            // Check if texture was loaded before and if so, continue to next iteration
            bool skip = false;
            for (unsigned int j = 0; j < m_TexturesLoaded.size(); j++)
            {
                if (std::strcmp(m_TexturesLoaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(m_TexturesLoaded[j]);
                    skip = true; // Texture with the same filepath has already been loaded, continue to next ono
                    break;
                }
            }

            if (!skip)
            {
                Texture_ texture;
                texture.id = GLUtils::TextureFromFile(str.C_Str(), m_Directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                m_TexturesLoaded.push_back(texture);  // Store it as texture loaded for entire model
            }
        }
        return textures;
    }


}