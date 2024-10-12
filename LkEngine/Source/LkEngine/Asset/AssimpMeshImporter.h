#pragma once

#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "LkEngine/Renderer/Mesh.h"

#include "LkEngine/Serialization/FileStream.h"


namespace LkEngine {

	/// FIXME
	class AssimpMeshImporter
	{
	public:
		AssimpMeshImporter(const std::filesystem::path& path);
		~AssimpMeshImporter() = default;

		TObjectPtr<MeshSource> ImportToMeshSource();

	private:
		void TraverseNodes(TObjectPtr<MeshSource> meshSource, 
						   void* assimpNode, 
						   const uint32_t nodeIndex, 
						   const glm::mat4& parentTransform = glm::mat4(1.0f), 
						   const uint32_t level = 0);

	private:
		const std::filesystem::path m_Path;
	};

}