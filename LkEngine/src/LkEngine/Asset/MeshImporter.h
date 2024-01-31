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

		//std::vector<Mesh> Load(aiNode* mesh, const aiScene* scene, Model& model);
		void Load(aiNode* mesh, const aiScene* scene, Model& model);
		std::vector<Mesh> Load(std::filesystem::path filepath);

		void ProcessNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes);
		Mesh Process(aiMesh* mesh, const aiScene* scene);

		static void LoadVertices(aiMesh* mesh, std::vector<Vertex>& vertices);
		static void LoadIndices(aiMesh* mesh, std::vector<unsigned int>& indices);
		static void LoadTextures(aiMesh* mesh, const aiScene* scene, std::vector<Texture_>& textures, Model& model);

	private:
		Model* m_ModelRef;
	};

    namespace GLUtils {
        unsigned int TextureFromFile(const char *path, const std::string& m_Directory, bool gamma = false);
    }

}