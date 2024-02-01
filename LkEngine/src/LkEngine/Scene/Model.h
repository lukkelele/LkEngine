#pragma once

#include "LkEngine/Core/Base.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <stb_image/stb_image.h>

#include "Mesh.h"


namespace LkEngine {

    class Model : public RefCounted 
    {   
    public:
        Model();
        Model(const std::string& path, bool gamma = false);
        Model(const Model&) = default;

        void Draw(Ref<Shader> shader);

        std::vector<Texture_> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
        
    private:
        void LoadModel(const std::string& path);
        void ProcessNode(aiNode* node, const aiScene* scene);
        Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

    private:
        std::vector<Mesh> m_Meshes;
        std::vector<Texture_> m_TexturesLoaded;	// Stores all the textures loaded so far
        std::string m_Directory;
        bool m_GammaCorrection;

        friend class ModelLoader;
        friend class MeshImporter;
    };
  
}

