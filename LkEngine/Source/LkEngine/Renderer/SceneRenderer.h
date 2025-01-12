#pragma once

#include "LkEngine/Scene/Scene.h"

#include "LkEngine/Renderer/UniformBufferSet.h"


namespace LkEngine {

	class LMesh;
	class LScene;
	class LPipeline;
	class LRenderer;
	class LRenderPass;
	class LRenderer2D;
	class LRenderCommandBuffer;
	class LVertexBuffer;

#if 0
	struct FSceneRendererCamera
	{
		TObjectPtr<LCamera> Camera{};
		glm::mat4 ViewMatrix = glm::mat4(1.0f);
	};
#endif

	struct FSceneRendererSpecification
	{
		uint32_t ShadowCascades = 4;
	};

    class LSceneRenderer : public LObject
    {
    public:
		LSceneRenderer(TObjectPtr<LScene> InScene, 
					   const FSceneRendererSpecification& InSpecification = FSceneRendererSpecification());
		~LSceneRenderer() = default;

		void Initialize();
		void Destroy();

		void BeginScene(const LSceneCamera& Camera);
		void EndScene();

		void SetScene(TObjectPtr<LScene> InScene);

		void SubmitMesh(TObjectPtr<LMesh> Mesh, 
						const uint32_t SubmeshIndex, 
						TObjectPtr<LMaterialTable> MaterialTable, 
						const glm::mat4& Transform);

		struct FStatistics
		{
			uint32_t DrawCalls = 0;
			uint32_t Meshes = 0;
			uint32_t Instances = 0;
			float TotalGpuTime = 0.0f;
		};

	private:
		void PreRender();
		void FlushDrawList();

	private:
		/**
		 * @struct FMeshKey
		 */
		struct FMeshKey
		{
			LUUID MeshHandle = 0;
			LUUID MaterialHandle = 0;
			uint32_t SubmeshIndex = 0;
			bool bIsSelected = false;

			FMeshKey(const LUUID InMeshHandle, 
					 const LUUID InMaterialHandle, 
					 const uint32_t InSubmeshIndex, 
					 const bool IsSelected)
				: MeshHandle(InMeshHandle)
				, MaterialHandle(InMaterialHandle)
				, SubmeshIndex(InSubmeshIndex)
				, bIsSelected(IsSelected)
			{
			}

			bool operator<(const FMeshKey& Other) const
			{
				if (MeshHandle < Other.MeshHandle)         return true;
				if (MeshHandle > Other.MeshHandle)         return false;
				if (SubmeshIndex < Other.SubmeshIndex)     return true;
				if (SubmeshIndex > Other.SubmeshIndex)     return false;
				if (MaterialHandle < Other.MaterialHandle) return true;
				if (MaterialHandle > Other.MaterialHandle) return false;
				return bIsSelected < Other.bIsSelected;
			}
		};

		/**
		 * @struct FDrawCommand
		 */
		struct FDrawCommand
		{
			TObjectPtr<LMesh> Mesh{};
			uint32_t SubmeshIndex{};
			TObjectPtr<LMaterialTable> MaterialTable;
			TObjectPtr<LMaterial> OverrideMaterial;

			uint32_t InstanceCount = 0;
			uint32_t InstanceOffset = 0;
			bool bIsRigged = false; /* TODO: Replace with enum EMeshRigged. */
		};

		struct FTransformVertexData
		{
			glm::vec4 MRow[3];
		};

		struct FTransformBuffer
		{
			TObjectPtr<LVertexBuffer> Buffer{};
			FTransformVertexData* Data = nullptr;
		};

		struct FTransformMapData
		{
			std::vector<FTransformVertexData> Transforms{};
			uint32_t TransformOffset = 0;
		};

	private:
		FSceneRendererSpecification Specification{};
		TObjectPtr<LScene> Scene{};

		TObjectPtr<LRenderCommandBuffer> CommandBuffer{};
		TObjectPtr<LRenderer2D> Renderer2D{};

		struct FSceneData
		{
		};
		FSceneData Data{};

		TObjectPtr<LPipeline> GeometryPipeline;
		TObjectPtr<LRenderPass> GeometryPass;
		TObjectPtr<LUniformBufferSet> UBSCamera;

		std::map<FMeshKey, FDrawCommand> DrawList{};
		std::map<FMeshKey, FTransformMapData> MeshTransformMap;

		LCLASS(LSceneRenderer)
    };

}
