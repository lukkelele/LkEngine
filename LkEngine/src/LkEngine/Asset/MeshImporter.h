#pragma once

#include "Asset.h"

#include "LkEngine/Renderer/VertexBuffer.h"
#include "LkEngine/Renderer/Texture.h"

#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


namespace LkEngine {

	struct Mesh
	{
		std::vector<Vertex> Vertices;
		std::vector<unsigned int> Indices;
		std::vector<RendererID> TextureHandles;
		std::string DebugName;

		Mesh() = default;
		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<RendererID> textureHandles)
			: Vertices(vertices)
			, Indices(indices)
			, TextureHandles(textureHandles) {}
	};

	class MeshImporter
	{
	public:
		MeshImporter();
		~MeshImporter() = default;

		Mesh Load(std::filesystem::path filepath);

	private:
		void ProcessNode(aiNode* node, const aiScene* scene);
		Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

		std::vector<Ref<Texture>> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

	private:
		std::vector<Mesh> m_Meshes{};

	};

}