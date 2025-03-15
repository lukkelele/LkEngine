#pragma once

#include <filesystem>

#include "LkEngine/Renderer/SceneRenderer.h"
#include "LkEngine/Renderer/Mesh.h"

#include "LkEngine/Serialization/FileStream.h"

namespace LkEngine {

	class LAssimpMeshImporter
	{
	public:
		LAssimpMeshImporter(const std::filesystem::path& InFilePath);
		LAssimpMeshImporter() = delete;
		~LAssimpMeshImporter() = default;

		bool IsValid() const;

		TObjectPtr<LMeshSource> ImportToMeshSource();

	private:
		void TraverseNodes(TObjectPtr<LMeshSource> InMeshSource,
						   void* AssimpNode,
						   const uint32_t InNodeIndex,
						   const glm::mat4& InParentTransform = glm::mat4(1.0f),
						   const uint32_t InLevel = 0);

	private:
		const std::filesystem::path FilePath;
	};

}
