#pragma once

#include "Asset.h"

#include "LkEngine/Renderer/VertexBuffer.h"
#include "LkEngine/Renderer/Texture.h"

#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "LkEngine/Scene/Mesh.h"


namespace LkEngine {

	class Model;

	class MeshImporter
	{
	public:
		MeshImporter() = default;
		~MeshImporter() = default;

		void Load(std::filesystem::path filepath, Model* model);
		void Load(const aiScene* scene, Ref<Model>& model);
		void Load(const aiScene* scene, Model* model);
		std::vector<Mesh> Load(std::filesystem::path filepath);

		void ProcessNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes);
		Mesh Process(aiMesh* mesh, const aiScene* scene);

		static void LoadVertices(aiMesh* mesh, std::vector<Vertex>& vertices);
		static void LoadIndices(aiMesh* mesh, std::vector<unsigned int>& indices);
		void LoadTextures(aiMesh* mesh, const aiScene* scene, std::vector<Texture_>& textures);

	private:
		Model* m_ModelRef = nullptr;
	};

	namespace Utils {

		static TextureUniformType GetTextureUniformType(const aiTextureType type)
		{
			switch (type)
			{
				case aiTextureType_DIFFUSE:             return TextureUniformType::Diffuse;
				case aiTextureType_SPECULAR:            return TextureUniformType::Specular;
				case aiTextureType_HEIGHT:              return TextureUniformType::Height;
				case aiTextureType_AMBIENT:             return TextureUniformType::Normal;
				case aiTextureType_EMISSIVE:            return TextureUniformType::Emissive;
				case aiTextureType_DIFFUSE_ROUGHNESS:   return TextureUniformType::DiffuseRoughness;
			}
			LK_CORE_ASSERT(false, "Could not determine the texture type, {}", (int)type);
		}

	}

    namespace GLUtils {
        unsigned int TextureFromFile(const char *path, const std::string& m_Directory, bool gamma = false);
    }

}