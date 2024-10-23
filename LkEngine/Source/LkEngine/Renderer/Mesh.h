#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "LkEngine/Asset/Asset.h"
#include "LkEngine/Asset/MaterialAsset.h"

#include "LkEngine/Serialization/FileStream.h"

#include "VertexBuffer.h"
#include "Texture.h"
#include "Material.h"

#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


namespace LkEngine {

    /**
     * FVertex
     */
    struct FVertex
	{
		glm::vec3 Position{};
		glm::vec3 Normal{};
		glm::vec3 Tangent{};
		glm::vec3 Binormal{};
		glm::vec2 Texcoord{};
	};

	/**
	 * FBoneInfo
	 */
	struct FBoneInfo
	{
		glm::mat4 SubMeshInverseTransform;
		glm::mat4 InverseBindPose;
		uint32_t SubMeshIndex;
		uint32_t BoneIndex;

		FBoneInfo() = default;
		FBoneInfo(const glm::mat4& InSubMeshInverseTransform, 
				  const glm::mat4& InInverseBindPose, 
				  const uint32_t InSubMeshIndex, 
				  const uint32_t InBoneIndex)
			: SubMeshInverseTransform(InSubMeshInverseTransform)
			, InverseBindPose(InInverseBindPose)
			, SubMeshIndex(InSubMeshIndex)
			, BoneIndex(InBoneIndex) 
		{
		}

		FORCEINLINE static void Serialize(StreamWriter* Serializer, const FBoneInfo& BoneInfo)
		{
			Serializer->WriteRaw(BoneInfo);
		}

		FORCEINLINE static void Deserialize(StreamReader* Deserializer, FBoneInfo& BoneInfo)
		{
			Deserializer->ReadRaw(BoneInfo);
		}
	};

	/**
	 * FBoneInfluence
	 */
	struct FBoneInfluence
	{
		uint32_t BoneInfoIndices[4] = { 0, 0, 0, 0 };
		float Weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

		FORCEINLINE void AddBoneData(const uint32_t BoneInfoIndex, float Weight)
		{
			if ((Weight < 0.0f) || (Weight > 1.0f))
			{
				LK_CORE_WARN_TAG("BoneInfluence", "Vertex bone weight is out of range, clamping it to [0, 1] (BoneID={}, Weight={})", 
								 BoneInfoIndex, Weight);
				Weight = std::clamp(Weight, 0.0f, 1.0f);
			}

			if (Weight > 0.0f)
			{
				for (size_t i = 0; i < 4; i++)
				{
					if (Weights[i] == 0.0f)
					{
						BoneInfoIndices[i] = BoneInfoIndex;
						Weights[i] = Weight;

						return;
					}
				}

				LK_CORE_WARN_TAG("BoneInfluence", "Vertex has more than four bones affecting it, " 
								 "extra bone influences will be discarded (BoneID={}, Weight={})", 
								 BoneInfoIndex, Weight);
			}
		}

		FORCEINLINE void NormalizeWeights()
		{
			float WeightSum = 0.0f;

			for (size_t i = 0; i < 4; i++)
			{
				WeightSum += Weights[i];
			}

			if (WeightSum > 0.0f)
			{
				for (size_t i = 0; i < 4; i++)
				{
					Weights[i] /= WeightSum;
				}
			}
		}

		static void Serialize(StreamWriter* serializer, const FBoneInfluence& BoneInfluence)
		{
			serializer->WriteRaw(BoneInfluence);
		}

		static void Deserialize(StreamReader* Deserializer, FBoneInfluence& BoneInfluence)
		{
			Deserializer->ReadRaw(BoneInfluence);
		}
	};

	static const int NumAttributes = 5;

	/**
	 * FIndex
	 */
	struct FIndex
	{
		uint32_t V1{};
		uint32_t V2{};
		uint32_t V3{};
	};
	static_assert(sizeof(FIndex) == (3 * sizeof(uint32_t)));

	/**
	 * FTriangle
	 */
	struct FTriangle
	{
		FVertex V0;
		FVertex V1; 
		FVertex V2;

		explicit FTriangle(const FVertex& InV0, 
						   const FVertex& InV1, 
						   const FVertex& InV2)
			: V0(InV0)
			, V1(InV1)
			, V2(InV2) {}
		FTriangle() = delete;
	};

	struct FAABB
	{
		glm::vec3 Min;
		glm::vec3 Max;

		FAABB() 
			: Min(0.0f)
			, Max(0.0f)
		{
		}

		FAABB(const glm::vec3& InMin, 
			 const glm::vec3& InMax) 
			: Min(InMin)
			, Max(InMax)
		{
		}
	};

	class LSubmesh
	{
	public:
		uint32_t BaseVertex{};
		uint32_t BaseIndex{};
		uint32_t MaterialIndex{};
		uint32_t IndexCount{};
		uint32_t VertexCount{};

		/** World transform.*/
		glm::mat4 Transform{ 1.0f };

		/** Local transform.*/
		glm::mat4 LocalTransform{ 1.0f };

		FAABB BoundingBox;

		std::string NodeName{};
		std::string MeshName{};
		bool bIsRigged = false;

		FORCEINLINE static void Serialize(StreamWriter* Serializer, const LSubmesh& Submesh)
		{
			Serializer->WriteRaw(Submesh.BaseVertex);
			Serializer->WriteRaw(Submesh.BaseIndex);
			Serializer->WriteRaw(Submesh.MaterialIndex);
			Serializer->WriteRaw(Submesh.IndexCount);
			Serializer->WriteRaw(Submesh.VertexCount);
			Serializer->WriteRaw(Submesh.Transform);
			Serializer->WriteRaw(Submesh.LocalTransform);
			Serializer->WriteRaw(Submesh.BoundingBox);
			Serializer->WriteString(Submesh.NodeName);
			Serializer->WriteString(Submesh.MeshName);
			Serializer->WriteRaw(Submesh.bIsRigged);
		}

		FORCEINLINE static void Deserialize(StreamReader* Deserializer, LSubmesh& Submesh)
		{
			Deserializer->ReadRaw(Submesh.BaseVertex);
			Deserializer->ReadRaw(Submesh.BaseIndex);
			Deserializer->ReadRaw(Submesh.MaterialIndex);
			Deserializer->ReadRaw(Submesh.IndexCount);
			Deserializer->ReadRaw(Submesh.VertexCount);
			Deserializer->ReadRaw(Submesh.Transform);
			Deserializer->ReadRaw(Submesh.LocalTransform);
			Deserializer->ReadRaw(Submesh.BoundingBox);
			Deserializer->ReadString(Submesh.NodeName);
			Deserializer->ReadString(Submesh.MeshName);
			Deserializer->ReadRaw(Submesh.bIsRigged);
		}
	};

	struct FMeshNode
	{
		uint32_t Parent = 0xFFFFFFFF;
		std::vector<uint32_t> Children{};
		std::vector<uint32_t> Submeshes{};

		std::string Name{};
		glm::mat4 LocalTransform{};

		FORCEINLINE bool IsRoot() const
		{
			return (Parent == 0xFFFFFFFF);
		}

		static void Serialize(StreamWriter* Serializer, const FMeshNode& MeshNode)
		{
			LK_VERIFY(Serializer);
			Serializer->WriteRaw(MeshNode.Parent);
			Serializer->WriteArray(MeshNode.Children);
			Serializer->WriteArray(MeshNode.Submeshes);
			Serializer->WriteString(MeshNode.Name);
			Serializer->WriteRaw(MeshNode.LocalTransform);
		}

		static void Deserialize(StreamReader* Deserializer, FMeshNode& MeshNode)
		{
			LK_VERIFY(Deserializer);
			Deserializer->ReadRaw(MeshNode.Parent);
			Deserializer->ReadArray(MeshNode.Children);
			Deserializer->ReadArray(MeshNode.Submeshes);
			Deserializer->ReadString(MeshNode.Name);
			Deserializer->ReadRaw(MeshNode.LocalTransform);
		}
	};

	/**
	 * LMeshSource
	 */
	class LMeshSource : public LAsset
	{
	public:
		LMeshSource() = default;
		LMeshSource(const std::vector<FVertex>& InVertices, 
					const std::vector<FIndex>& InIndices, 
					const glm::mat4& InTransform);
		LMeshSource(const std::vector<FVertex>& InVertices, 
					const std::vector<FIndex>& InIndices, 
					const std::vector<LSubmesh>& InSubmeshes);
		virtual ~LMeshSource() = default;

		std::vector<LSubmesh>& GetSubmeshes() { return m_Submeshes; }
		const std::vector<LSubmesh>& GetSubmeshes() const { return m_Submeshes; }

		const std::vector<FVertex>& GetVertices() const { return m_Vertices; }
		const std::vector<FIndex>& GetIndices() const { return m_Indices; }

		bool IsSubmeshRigged(uint32_t submeshIndex) const { return m_Submeshes[submeshIndex].bIsRigged; }
		const std::vector<FBoneInfluence>& GetBoneInfluences() const { return m_BoneInfluences; }

		FORCEINLINE std::vector<TObjectPtr<LMaterial>>& GetMaterials() 
		{ 
			return m_Materials; 
		}

		FORCEINLINE const std::vector<TObjectPtr<LMaterial>>& GetMaterials() const 
		{ 
			return m_Materials; 
		}

		FORCEINLINE const std::filesystem::path& GetFilePath() const 
		{ 
			return m_FilePath;
		}

		FORCEINLINE const std::vector<FTriangle> GetTriangleCache(const uint32_t Index) const 
		{ 
			LK_VERIFY(m_TriangleCache.contains(Index));
			return m_TriangleCache.at(Index); 
		}

		TObjectPtr<LVertexBuffer> GetVertexBuffer() { return m_VertexBuffer; }
		TObjectPtr<LVertexBuffer> GetBoneInfluenceBuffer() { return m_BoneInfluenceBuffer; }
		TObjectPtr<LIndexBuffer> GetIndexBuffer() { return m_IndexBuffer; }

		static EAssetType GetStaticType() { return EAssetType::MeshSource; }

		//const AABB& GetBoundingBox() const { return m_BoundingBox; }

		FORCEINLINE const FMeshNode& GetRootNode() const 
		{ 
			LK_CORE_ASSERT(m_Nodes.size() > 0, "No root node exists");
			return m_Nodes[0]; 
		}

		const std::vector<FMeshNode>& GetNodes() const 
		{ 
			return m_Nodes; 
		}

		/**
		 * Dump vertex buffer info to log.
		 */
		void DumpVertexBuffer();

		/* Not implemented yet. */
	#if 0
		FORCEINLINE bool HasSkeleton() const 
		{ 
			return static_cast<bool>(m_Skeleton); 
		}

		FORCEINLINE const LSkeleton& GetSkeleton() const 
		{ 
			LK_CORE_ASSERT(m_Skeleton, "Attempted to access null skeleton"); 
			return *m_Skeleton; 
		}

		bool IsCompatibleSkeleton(const uint32_t animationIndex, const LSkeleton& Skeleton) const;

		std::vector<std::string> GetAnimationNames() const;

		const LAnimation* GetAnimation(const uint32_t AnimationIndex, 
									   const LSkeleton& Skeleton, 
									   const bool IsMaskedRootMotion, 
									   const glm::vec3& RootTranslationMask, 
									   const float RootRotationMask) const;
	#endif

	private:
		std::vector<LSubmesh> m_Submeshes{};

		TObjectPtr<LVertexBuffer> m_VertexBuffer{};
		TObjectPtr<LVertexBuffer> m_BoneInfluenceBuffer{};
		TObjectPtr<LIndexBuffer> m_IndexBuffer{};

		std::vector<FVertex> m_Vertices{};
		std::vector<FIndex> m_Indices{};

		std::vector<FBoneInfluence> m_BoneInfluences{};
		std::vector<FBoneInfo> m_BoneInfo{};

		/* Not implemented yet. */
	#if 0
		/* Skeleton. */
		mutable TObjectPtr<LSkeleton> m_Skeleton;

		/* Animations. */
		mutable std::vector<TObjectPtr<LAnimation>> Animations;
		std::vector<std::string> m_AnimationNames;
	#endif

		std::vector<TObjectPtr<LMaterial>> m_Materials{};
		std::unordered_map<uint32_t, std::vector<FTriangle>> m_TriangleCache;
		std::vector<FMeshNode> m_Nodes{};
		FAABB m_BoundingBox{};

		std::filesystem::path m_FilePath{};

		friend class LScene;
		friend class LSceneRenderer;
		friend class LRenderer;
		friend class LOpenGLRenderer;
		friend class LMesh;
		friend class LAssimpMeshImporter;
		friend class LAssetManager;
		friend class LRuntimeAssetManager;

		LASSET(LMeshSource)
	};

	/**
	 * LMesh
	 *
	 *  Mesh asset.
	 */
	class LMesh : public LAsset
	{
	public:
		explicit LMesh(TObjectPtr<LMeshSource> MeshSource);
		LMesh(TObjectPtr<LMeshSource> InMeshSource, const std::vector<uint32_t>& InSubmeshes);
		LMesh(const TObjectPtr<LMesh>& Other);
		virtual ~LMesh() = default;

		std::vector<uint32_t>& GetSubmeshes() { return m_Submeshes; }
		const std::vector<uint32_t>& GetSubmeshes() const { return m_Submeshes; }

		void SetSubmeshes(const std::vector<uint32_t>& submeshes);

		FORCEINLINE TObjectPtr<LMeshSource> GetMeshSource() 
		{ 
			return m_MeshSource; 
		}

		FORCEINLINE TObjectPtr<LMeshSource> GetMeshSource() const 
		{ 
			return m_MeshSource; 
		}

		FORCEINLINE void SetMeshAsset(TObjectPtr<LMeshSource> meshSource) 
		{ 
			m_MeshSource = meshSource; 
		}

		FORCEINLINE TObjectPtr<LMaterialTable> GetMaterials() const 
		{ 
			return Materials; 
		}

		FORCEINLINE FAssetHandle GetMaterialHandle(const int Index) 
		{ 
			return Materials->GetMaterialHandle(Index);
		}

		TObjectPtr<LMaterial> GetMaterial(const int Index = 0);

		static EAssetType GetStaticType() { return EAssetType::Mesh; }

		TObjectPtr<LVertexBuffer> GetVertexBuffer() 
		{ 
			return GetMeshSource()->GetVertexBuffer(); 
		}

		TObjectPtr<LIndexBuffer> GetIndexBuffer() { return GetMeshSource()->GetIndexBuffer(); }

	private:
		TObjectPtr<LMeshSource> m_MeshSource;
		std::vector<uint32_t> m_Submeshes; 

		TObjectPtr<LMaterialTable> Materials;

		friend class LScene;
		friend class LRenderer;
		friend class LAssetManager;
		friend class LRuntimeAssetManager;
		friend class LOpenGLRenderer;
		friend class MeshViewerPanel;

		LASSET(LMesh)
	};

	/**
	 * LStaticMesh
	 */
	class LStaticMesh : public LAsset
	{
	public:
		explicit LStaticMesh(TObjectPtr<LMeshSource> InMeshSource);
		LStaticMesh(TObjectPtr<LMeshSource> InMeshSource, const std::vector<uint32_t>& InSubmeshes);
		LStaticMesh(const TObjectPtr<LStaticMesh>& Other);
		virtual ~LStaticMesh() = default;

		FORCEINLINE std::vector<uint32_t>& GetSubmeshes() 
		{ 
			return m_Submeshes; 
		}

		FORCEINLINE const std::vector<uint32_t>& GetSubmeshes() const 
		{ 
			return m_Submeshes; 
		}

		void SetSubmeshes(const std::vector<uint32_t>& InSubmeshes);

		FORCEINLINE TObjectPtr<LMeshSource> GetMeshSource() 
		{ 
			return m_MeshSource; 
		}

		FORCEINLINE TObjectPtr<LMeshSource> GetMeshSource() const 
		{ 
			return m_MeshSource;
		}

		FORCEINLINE void SetMeshAsset(TObjectPtr<LMeshSource> InMeshAsset) 
		{ 
			/// TODO: Mark dirty
			m_MeshSource = InMeshAsset; 
		}

		TObjectPtr<LMaterialTable> GetMaterials() const 
		{ 
			return Materials; 
		}

		static EAssetType GetStaticType() { return EAssetType::StaticMesh; }

	private:
		TObjectPtr<LMeshSource> m_MeshSource{};
		std::vector<uint32_t> m_Submeshes{};

		// Materials
		TObjectPtr<LMaterialTable> Materials{};

		friend class LScene;
		friend class LRenderer;
		friend class LAssetManager;
		friend class LRuntimeAssetManager;
		friend class LOpenGLRenderer;
		friend class LSceneHierarchyPanel;
		friend class LMeshViewerPanel;

		LASSET(LStaticMesh)
	};


}