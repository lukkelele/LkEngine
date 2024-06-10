#pragma once

#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "LkEngine/Renderer/Mesh.h"

#include "LkEngine/Serialization/FileStream.h"


namespace LkEngine {

	class AssimpMeshImporter
	{
	public:
		AssimpMeshImporter(const std::filesystem::path& path);
		~AssimpMeshImporter() = default;

		Ref<MeshSource> ImportToMeshSource();

	private:
		void TraverseNodes(Ref<MeshSource> meshSource, void* assimpNode, uint32_t nodeIndex, const glm::mat4& parentTransform = glm::mat4(1.0f), uint32_t level = 0);

	private:
		const std::filesystem::path m_Path;
	};

}