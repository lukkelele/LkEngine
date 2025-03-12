#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "LkEngine/Renderer/Pipeline.h"
#include "LkEngine/Renderer/RenderPass.h"
#include "LkEngine/Scene/Scene.h"

#include "LkEngine/Renderer/UniformBufferSet.h"

namespace LkEngine {

	class LMaterial;
	class LMesh;
	class LStaticMesh;
	class LRenderer;
	class LRenderer2D;
	class LRenderCommandBuffer;
	class LVertexBuffer;

	struct FSceneRendererSpecification
	{
		uint32_t ShadowCascades = 4;
	};

	struct FSceneRendererFlushData
	{
		uint32_t StaticMeshDrawListSize = 0;
		uint32_t StaticMeshTransformMapSize = 0;

		uint32_t MeshDrawListSize = 0;
		uint32_t MeshTransformMapSize = 0;
	};

    class LSceneRenderer : public LObject
    {
	public:
		LK_DECLARE_MULTICAST_DELEGATE(FOnDrawListFlush, const FSceneRendererFlushData&);
    public:
		LSceneRenderer(TObjectPtr<LScene> InScene, 
					   const FSceneRendererSpecification& InSpecification = FSceneRendererSpecification());
		~LSceneRenderer() = default;

		void Initialize();
		void Destroy();

		void BeginScene(const LSceneCamera& Camera);
		void EndScene();

		void SetScene(TObjectPtr<LScene> InScene);

		void SubmitStaticMesh(TObjectPtr<LStaticMesh> StaticMesh, 
							  TObjectPtr<LMaterialTable> MaterialTable, 
							  const glm::mat4& Transform,
							  TObjectPtr<LMaterialAsset> MaterialOverride = nullptr);

		void SubmitMesh(TObjectPtr<LMesh> Mesh, 
						const uint32_t SubmeshIndex, 
						TObjectPtr<LMaterialTable> MaterialTable, 
						const glm::mat4& Transform,
						TObjectPtr<LMaterialAsset> MaterialOverride = nullptr);

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

	public:
		FOnDrawListFlush OnDrawListFlush;
	private:
		enum class EMeshRigged
		{
			IsRigged,
			NotRigged
		};

		/**
		 * FMeshKey
		 */
		struct FMeshKey
		{
			FAssetHandle MeshHandle = 0;
			FAssetHandle MaterialHandle = 0;
			uint32_t SubmeshIndex = 0;
			bool bIsSelected = false;

			FMeshKey(const FAssetHandle InMeshHandle, 
					 const FAssetHandle InMaterialHandle, 
					 const uint32_t InSubmeshIndex, 
					 const bool IsSelected)
				: MeshHandle(InMeshHandle)
				, MaterialHandle(InMaterialHandle)
				, SubmeshIndex(InSubmeshIndex)
				, bIsSelected(IsSelected)
			{
			}

			FORCEINLINE bool operator<(const FMeshKey& Other) const
			{
				if (MeshHandle < Other.MeshHandle)         return true;
				if (MeshHandle > Other.MeshHandle)         return false;
				if (SubmeshIndex < Other.SubmeshIndex)     return true;
				if (SubmeshIndex > Other.SubmeshIndex)     return false;
				if (MaterialHandle < Other.MaterialHandle) return true;
				if (MaterialHandle > Other.MaterialHandle) return false;
				return (bIsSelected < Other.bIsSelected);
			}
		};

		/**
		 * FDrawCommand
		 */
		template<typename MeshType = LMesh>
		struct FDrawCommand
		{
			TObjectPtr<MeshType> Mesh{};
			uint32_t SubmeshIndex{};
			TObjectPtr<LMaterialTable> MaterialTable{};
			TObjectPtr<LMaterial> OverrideMaterial{};

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
		TObjectPtr<LScene> Scene;

		/** @fixme: FIXME */
		TObjectPtr<LRenderCommandBuffer> CommandBuffer;
		TObjectPtr<LRenderer2D> Renderer2D;

		struct FSceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};
		FSceneData Data{};

		TObjectPtr<LPipeline> GeometryPipeline;
		TObjectPtr<LRenderPass> GeometryPass;
		TObjectPtr<LUniformBufferSet> UBSCamera;

		std::map<FMeshKey, FDrawCommand<LMesh>> MeshDrawList{};
		std::map<FMeshKey, FTransformMapData> MeshTransformMap;

		std::map<FMeshKey, FDrawCommand<LStaticMesh>> StaticMeshDrawList{};
		std::map<FMeshKey, FTransformMapData> StaticMeshTransformMap;

		friend class LSceneManagerPanel;

		LCLASS(LSceneRenderer)
    };

}
