#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"
#include "LkEngine/Core/Math/AABB.h"

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

		static void Serialize(StreamWriter* Serializer, const FBoneInfluence& BoneInfluence)
		{
			Serializer->WriteRaw(BoneInfluence);
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
			, V2(InV2)
		{
		}
		FTriangle() = delete;
	};

	class LSubmesh
	{
	public:
		LSubmesh() = default;
		~LSubmesh() = default;

		uint32_t BaseVertex{};
		uint32_t BaseIndex{};
		uint32_t MaterialIndex{};
		uint32_t IndexCount{};
		uint32_t VertexCount{};

		/** World transform.*/
		glm::mat4 Transform{ 1.0f };

		/** Local transform.*/
		glm::mat4 LocalTransform{ 1.0f };

		FAABB BoundingBox{};

		std::string NodeName{};
		std::string MeshName{};
		bool bIsRigged = false;

		static void Serialize(StreamWriter* Serializer, const LSubmesh& Submesh)
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

		static void Deserialize(StreamReader* Deserializer, LSubmesh& Submesh)
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

		std::string ToString() const
		{
			return std::format("[Submesh: {}] VertexCount={}  IndexCount={}  MaterialIndex={}", MeshName, VertexCount, IndexCount, MaterialIndex);
			//return std::format("[Submesh]");
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
		LMeshSource();
		LMeshSource(const std::vector<FVertex>& InVertices, const std::vector<FIndex>& InIndices, const glm::mat4& InTransform);
		LMeshSource(const std::vector<FVertex>& InVertices, const std::vector<FIndex>& InIndices, const std::vector<LSubmesh>& InSubmeshes);
		virtual ~LMeshSource() = default;

		FORCEINLINE std::vector<LSubmesh>& GetSubmeshes() { return Submeshes; }
		FORCEINLINE const std::vector<LSubmesh>& GetSubmeshes() const { return Submeshes; }

		FORCEINLINE const std::vector<FVertex>& GetVertices() const { return Vertices; }
		FORCEINLINE const std::vector<FIndex>& GetIndices() const { return Indices; }

		FORCEINLINE bool IsSubmeshRigged(const uint32_t SubmeshIndex) const 
		{ 
			LK_CORE_ASSERT(!Submeshes.empty() && (SubmeshIndex < Submeshes.size()));
			return Submeshes[SubmeshIndex].bIsRigged; 
		}

		FORCEINLINE const std::vector<FBoneInfluence>& GetBoneInfluences() const 
		{ 
			return BoneInfluences; 
		}

		FORCEINLINE std::vector<TObjectPtr<LMaterial>>& GetMaterials()
		{
			return Materials;
		}

		FORCEINLINE const std::vector<TObjectPtr<LMaterial>>& GetMaterials() const
		{
			return Materials;
		}

		FORCEINLINE const std::filesystem::path& GetFilePath() const
		{
			return FilePath;
		}

		FORCEINLINE const std::vector<FTriangle> GetTriangleCache(const uint32_t Index) const
		{
			LK_VERIFY(TriangleCache.contains(Index));
			return TriangleCache.at(Index);
		}

		FORCEINLINE TObjectPtr<LVertexBuffer> GetVertexBuffer() 
		{ 
			LK_CORE_ASSERT(VertexBuffer);
			return VertexBuffer; 
		}

		FORCEINLINE TObjectPtr<LIndexBuffer> GetIndexBuffer() 
		{ 
			LK_CORE_ASSERT(IndexBuffer);
			return IndexBuffer; 
		}

		FORCEINLINE TObjectPtr<LVertexBuffer> GetBoneInfluenceBuffer() 
		{ 
			return BoneInfluenceBuffer; 
		}

		static EAssetType GetStaticType() { return EAssetType::MeshSource; }
		//const FAABB& GetBoundingBox() const { return m_BoundingBox; }

		FORCEINLINE const std::vector<FMeshNode>& GetNodes() const 
		{ 
			return MeshNodes; 
		}

		FORCEINLINE const FMeshNode& GetRootNode() const
		{
			LK_CORE_ASSERT(!MeshNodes.empty(), "No root node exists");
			return MeshNodes[0];
		}

		/**
		 * Dump vertex buffer info to log.
		 */
		void DumpVertexBuffer();

	private:
		std::vector<LSubmesh> Submeshes{};

		TObjectPtr<LVertexBuffer> VertexBuffer{};
		TObjectPtr<LVertexBuffer> BoneInfluenceBuffer{};
		TObjectPtr<LIndexBuffer> IndexBuffer{};

		std::vector<FVertex> Vertices{};
		std::vector<FIndex> Indices{};

		std::vector<FBoneInfluence> BoneInfluences{};
		std::vector<FBoneInfo> BoneInfo{};

		std::vector<TObjectPtr<LMaterial>> Materials{};
		std::unordered_map<uint32_t, std::vector<FTriangle>> TriangleCache;
		std::vector<FMeshNode> MeshNodes{};
		FAABB BoundingBox{};

		std::filesystem::path FilePath{};

		friend class LScene;
		friend class LSceneRenderer;
		friend class LRenderer;
		friend class LOpenGLRenderer;
		friend class LMesh;
		friend class LAssimpMeshImporter;
		friend class LAssetManager;
		friend class LEditorAssetManager;
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

		std::vector<uint32_t>& GetSubmeshes() { return Submeshes; }
		const std::vector<uint32_t>& GetSubmeshes() const { return Submeshes; }

		void SetSubmeshes(const std::vector<uint32_t>& InSubmeshes);

		TObjectPtr<LMeshSource> GetMeshSource() { return MeshSource; }
		TObjectPtr<LMeshSource> GetMeshSource() const { return MeshSource; }

		void SetMeshAsset(TObjectPtr<LMeshSource> InMeshSource)
		{
			MeshSource = InMeshSource;
		}

		TObjectPtr<LMaterialTable> GetMaterialTable() const { return MaterialTable; }

		static EAssetType GetStaticType() { return EAssetType::Mesh; }

		/* TODO: Remove GetVertexBuffer and GetIndexBuffer. */
		TObjectPtr<LVertexBuffer> GetVertexBuffer() { return MeshSource->GetVertexBuffer(); }
		TObjectPtr<LIndexBuffer> GetIndexBuffer() { return MeshSource->GetIndexBuffer(); }

		TObjectPtr<LMaterial> GetMaterial(const uint32_t Index = 0);

		FAssetHandle GetMaterialHandle(const uint32_t Index) 
		{ 
			LK_CORE_ASSERT(MaterialTable && MaterialTable->HasMaterial(Index));
			return MaterialTable->GetMaterial(Index); 
		}

		const std::string& GetName() const { return Name; }
		void SetName(const std::string& InName) { Name = InName; }

	private:
		TObjectPtr<LMeshSource> MeshSource{};
		std::vector<uint32_t> Submeshes{};
		TObjectPtr<LMaterialTable> MaterialTable{};

		std::string Name{};

		friend class LAssetManager;
		friend class LScene;
		friend class LRenderer;
		friend class LSceneRenderer;
		friend class LRuntimeAssetManager;
		friend class LOpenGLRenderer;

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

		std::vector<uint32_t>& GetSubmeshes() { return Submeshes; } 
		const std::vector<uint32_t>& GetSubmeshes() const { return Submeshes; }

		void SetSubmeshes(const std::vector<uint32_t>& InSubmeshes);

		TObjectPtr<LMeshSource> GetMeshSource() { return MeshSource; }
		TObjectPtr<LMeshSource> GetMeshSource() const { return MeshSource; }

		void SetMeshAsset(TObjectPtr<LMeshSource> InMeshAsset)
		{
			MeshSource = InMeshAsset;
		}

		TObjectPtr<LMaterialTable> GetMaterialTable() const
		{
			LK_CORE_ASSERT(MaterialTable);
			return MaterialTable;
		}

		static EAssetType GetStaticType() { return EAssetType::StaticMesh; }

		void SetName(const std::string& InName) { Name = InName; }

	private:
		TObjectPtr<LMeshSource> MeshSource{};
		std::vector<uint32_t> Submeshes{};

		TObjectPtr<LMaterialTable> MaterialTable{};

		std::string Name{};

		friend class LAssetManager;
		friend class LScene;
		friend class LRenderer;
		friend class LSceneRenderer;
		friend class LRuntimeAssetManager;
		friend class LOpenGLRenderer;
		friend class LSceneHierarchyPanel;
		friend class LMeshViewerPanel;

		LASSET(LStaticMesh)
	};


}
