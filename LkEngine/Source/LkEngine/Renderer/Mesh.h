#pragma once

#include "LkEngine/Asset/Asset.h"
#include "LkEngine/Asset/MaterialAsset.h"

#include "VertexBuffer.h"
#include "Texture.h"
#include "Material.h"

#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "LkEngine/Serialization/FileStream.h"


namespace LkEngine {

    struct Vertex
	{
		glm::vec3 Position{};
		glm::vec3 Normal{};
		glm::vec3 Tangent{};
		glm::vec3 Binormal{};
		glm::vec2 Texcoord{};
	};

	struct BoneInfo
	{
		glm::mat4 SubMeshInverseTransform;
		glm::mat4 InverseBindPose;
		uint32_t SubMeshIndex;
		uint32_t BoneIndex;

		BoneInfo() = default;
		BoneInfo(glm::mat4 subMeshInverseTransform, glm::mat4 inverseBindPose, uint32_t subMeshIndex, uint32_t boneIndex)
			: SubMeshInverseTransform(subMeshInverseTransform)
			, InverseBindPose(inverseBindPose)
			, SubMeshIndex(subMeshIndex)
			, BoneIndex(boneIndex) {}

		static void Serialize(StreamWriter* serializer, const BoneInfo& Instance)
		{
			serializer->WriteRaw(Instance);
		}

		static void Deserialize(StreamReader* deserializer, BoneInfo& Instance)
		{
			deserializer->ReadRaw(Instance);
		}
	};

	struct BoneInfluence
	{
		uint32_t BoneInfoIndices[4] = { 0, 0, 0, 0 };
		float Weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

		void AddBoneData(uint32_t boneInfoIndex, float weight)
		{
			if (weight < 0.0f || weight > 1.0f)
			{
				LK_CORE_WARN("Vertex bone weight is out of range. We will clamp it to [0, 1] (BoneID={0}, Weight={1})", boneInfoIndex, weight);
				weight = std::clamp(weight, 0.0f, 1.0f);
			}
			if (weight > 0.0f)
			{
				for (size_t i = 0; i < 4; i++)
				{
					if (Weights[i] == 0.0f)
					{
						BoneInfoIndices[i] = boneInfoIndex;
						Weights[i] = weight;
						return;
					}
				}

				LK_CORE_WARN("Vertex has more than four bones affecting it, "
							 "extra bone influences will be discarded (BoneID={0}, Weight={1})", boneInfoIndex, weight);
			}
		}

		void NormalizeWeights()
		{
			float sumWeights = 0.0f;
			for (size_t i = 0; i < 4; i++)
			{
				sumWeights += Weights[i];
			}
			if (sumWeights > 0.0f)
			{
				for (size_t i = 0; i < 4; i++)
				{
					Weights[i] /= sumWeights;
				}
			}
		}

		static void Serialize(StreamWriter* serializer, const BoneInfluence& Instance)
		{
			serializer->WriteRaw(Instance);
		}

		static void Deserialize(StreamReader* deserializer, BoneInfluence& Instance)
		{
			deserializer->ReadRaw(Instance);
		}
	};

	static const int NumAttributes = 5;

	struct Index
	{
		uint32_t V1, V2, V3;
	};
	static_assert(sizeof(Index) == 3 * sizeof(uint32_t));

	struct Triangle
	{
		Vertex V0, V1, V2;

		Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2)
			: V0(v0)
			, V1(v1)
			, V2(v2) {}
	};

	struct AABB
	{
		glm::vec3 Min, Max;

		AABB() 
			: Min(0.0f)
			, Max(0.0f)
		{
		}

		AABB(const glm::vec3& InMin, 
			 const glm::vec3& InMax) 
			: Min(InMin)
			, Max(InMax)
		{
		}
	};

	class Submesh
	{
	public:
		uint32_t BaseVertex;
		uint32_t BaseIndex;
		uint32_t MaterialIndex;
		uint32_t IndexCount;
		uint32_t VertexCount;

		glm::mat4 Transform{ 1.0f }; // World transform
		glm::mat4 LocalTransform{ 1.0f };
		AABB BoundingBox;

		std::string NodeName, MeshName;
		bool IsRigged = false;

		static void Serialize(StreamWriter* Serializer, const Submesh& Instance)
		{
			Serializer->WriteRaw(Instance.BaseVertex);
			Serializer->WriteRaw(Instance.BaseIndex);
			Serializer->WriteRaw(Instance.MaterialIndex);
			Serializer->WriteRaw(Instance.IndexCount);
			Serializer->WriteRaw(Instance.VertexCount);
			Serializer->WriteRaw(Instance.Transform);
			Serializer->WriteRaw(Instance.LocalTransform);
			Serializer->WriteRaw(Instance.BoundingBox);
			Serializer->WriteString(Instance.NodeName);
			Serializer->WriteString(Instance.MeshName);
			Serializer->WriteRaw(Instance.IsRigged);
		}

		static void Deserialize(StreamReader* deserializer, Submesh& Instance)
		{
			deserializer->ReadRaw(Instance.BaseVertex);
			deserializer->ReadRaw(Instance.BaseIndex);
			deserializer->ReadRaw(Instance.MaterialIndex);
			deserializer->ReadRaw(Instance.IndexCount);
			deserializer->ReadRaw(Instance.VertexCount);
			deserializer->ReadRaw(Instance.Transform);
			deserializer->ReadRaw(Instance.LocalTransform);
			deserializer->ReadRaw(Instance.BoundingBox);
			deserializer->ReadString(Instance.NodeName);
			deserializer->ReadString(Instance.MeshName);
			deserializer->ReadRaw(Instance.IsRigged);
		}
	};

	struct MeshNode
	{
		uint32_t Parent = 0xffffffff;
		std::vector<uint32_t> Children;
		std::vector<uint32_t> Submeshes;

		std::string Name;
		glm::mat4 LocalTransform;

		inline bool IsRoot() const { return Parent == 0xffffffff; }

		static void Serialize(StreamWriter* Serializer, const MeshNode& Instance)
		{
			LK_VERIFY(Serializer);
			Serializer->WriteRaw(Instance.Parent);
			Serializer->WriteArray(Instance.Children);
			Serializer->WriteArray(Instance.Submeshes);
			Serializer->WriteString(Instance.Name);
			Serializer->WriteRaw(Instance.LocalTransform);
		}

		static void Deserialize(StreamReader* Deserializer, MeshNode& Instance)
		{
			LK_VERIFY(Deserializer);
			Deserializer->ReadRaw(Instance.Parent);
			Deserializer->ReadArray(Instance.Children);
			Deserializer->ReadArray(Instance.Submeshes);
			Deserializer->ReadString(Instance.Name);
			Deserializer->ReadRaw(Instance.LocalTransform);
		}
	};

	/**
	 * LMeshSource
	 */
	class MeshSource : public LAsset
	{
	public:
		MeshSource() = default;
		MeshSource(const std::vector<Vertex>& vertices, const std::vector<Index>& indices, const glm::mat4& transform);
		MeshSource(const std::vector<Vertex>& vertices, const std::vector<Index>& indices, const std::vector<Submesh>& submeshes);
		virtual ~MeshSource() = default;

		void DumpVertexBuffer();

		std::vector<Submesh>& GetSubmeshes() { return m_Submeshes; }
		const std::vector<Submesh>& GetSubmeshes() const { return m_Submeshes; }

		const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
		const std::vector<Index>& GetIndices() const { return m_Indices; }

		bool IsSubmeshRigged(uint32_t submeshIndex) const { return m_Submeshes[submeshIndex].IsRigged; }
		const std::vector<BoneInfluence>& GetBoneInfluences() const { return m_BoneInfluences; }

		std::vector<TObjectPtr<LMaterial>>& GetMaterials() { return m_Materials; }
		const std::vector<TObjectPtr<LMaterial>>& GetMaterials() const { return m_Materials; }
		const std::string& GetFilePath() const { return m_FilePath; }

		FORCEINLINE const std::vector<Triangle> GetTriangleCache(uint32_t index) const 
		{ 
			return m_TriangleCache.at(index); 
		}

		TObjectPtr<LVertexBuffer> GetVertexBuffer() { return m_VertexBuffer; }
		TObjectPtr<LVertexBuffer> GetBoneInfluenceBuffer() { return m_BoneInfluenceBuffer; }
		TObjectPtr<LIndexBuffer> GetIndexBuffer() { return m_IndexBuffer; }

		static EAssetType GetStaticType() { return EAssetType::MeshSource; }
		//virtual EAssetType GetAssetType() const override { return GetStaticType(); }

		//const AABB& GetBoundingBox() const { return m_BoundingBox; }

		const MeshNode& GetRootNode() const { return m_Nodes[0]; }
		const std::vector<MeshNode>& GetNodes() const { return m_Nodes; }

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
		std::vector<Submesh> m_Submeshes;

		TObjectPtr<LVertexBuffer> m_VertexBuffer;
		TObjectPtr<LVertexBuffer> m_BoneInfluenceBuffer;
		TObjectPtr<LIndexBuffer> m_IndexBuffer;

		std::vector<Vertex> m_Vertices;
		std::vector<Index> m_Indices;

		std::vector<BoneInfluence> m_BoneInfluences;
		std::vector<BoneInfo> m_BoneInfo;

		/* Skeleton. */
		//mutable TObjectPtr<LSkeleton> m_Skeleton;

		/* Animations. */
		//mutable std::vector<TObjectPtr<LAnimation>> Animations;
		//std::vector<std::string> m_AnimationNames;

		std::vector<TObjectPtr<LMaterial>> m_Materials;
		std::unordered_map<uint32_t, std::vector<Triangle>> m_TriangleCache;

		AABB m_BoundingBox;

		std::string m_FilePath{};

		std::vector<MeshNode> m_Nodes;

		// TEMP
		bool m_Runtime = false;

		friend class LScene;
		friend class LSceneRenderer;
		friend class LRenderer;
		friend class OpenGLRenderer;
		friend class LMesh;
		friend class Mesh; /// REMOVE
		friend class AssimpMeshImporter;
		friend class LAssetManager;
		friend class LRuntimeAssetManager;

		LASSET(MeshSource)
	};

	/**
	 * LMesh
	 *
	 *  Mesh asset.
	 */
	class Mesh : public LAsset
	{
	public:
		explicit Mesh(TObjectPtr<MeshSource> meshSource);
		Mesh(TObjectPtr<MeshSource> meshSource, const std::vector<uint32_t>& submeshes);
		Mesh(const Ref<Mesh>& other);
		virtual ~Mesh();

		std::vector<uint32_t>& GetSubmeshes() { return m_Submeshes; }
		const std::vector<uint32_t>& GetSubmeshes() const { return m_Submeshes; }

		void SetSubmeshes(const std::vector<uint32_t>& submeshes);

		FORCEINLINE TObjectPtr<MeshSource> GetMeshSource() 
		{ 
			return m_MeshSource; 
		}

		FORCEINLINE TObjectPtr<MeshSource> GetMeshSource() const 
		{ 
			return m_MeshSource; 
		}

		FORCEINLINE void SetMeshAsset(TObjectPtr<MeshSource> meshSource) 
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

		TObjectPtr<LMaterial> GetMaterial(int idx = 0);

		static EAssetType GetStaticType() { return EAssetType::Mesh; }
		//virtual EAssetType GetAssetType() const override { return GetStaticType(); }

		TObjectPtr<LVertexBuffer> GetVertexBuffer() 
		{ 
			return GetMeshSource()->GetVertexBuffer(); 
		}

		TObjectPtr<LIndexBuffer> GetIndexBuffer() { return GetMeshSource()->GetIndexBuffer(); }

	private:
		TObjectPtr<MeshSource> m_MeshSource;
		std::vector<uint32_t> m_Submeshes; 

		//TObjectPtr<LMaterialTable> m_Materials;
		TObjectPtr<LMaterialTable> Materials;

		friend class LScene;
		friend class LRenderer;
		friend class LAssetManager;
		friend class LRuntimeAssetManager;
		friend class LOpenGLRenderer;
		friend class MeshViewerPanel;

		LASSET(Mesh)
	};

	/**
	 * LStaticMesh
	 */
	class StaticMesh : public LAsset
	{
	public:
		explicit StaticMesh(TObjectPtr<MeshSource> meshSource);
		StaticMesh(TObjectPtr<MeshSource> meshSource, 
				   const std::vector<uint32_t>& submeshes);
		StaticMesh(const Ref<StaticMesh>& Other);
		virtual ~StaticMesh();

		FORCEINLINE std::vector<uint32_t>& GetSubmeshes() { return m_Submeshes; }
		FORCEINLINE const std::vector<uint32_t>& GetSubmeshes() const { return m_Submeshes; }

		void SetSubmeshes(const std::vector<uint32_t>& submeshes);

		FORCEINLINE TObjectPtr<MeshSource> GetMeshSource() 
		{ 
			return m_MeshSource; 
		}

		FORCEINLINE TObjectPtr<MeshSource> GetMeshSource() const 
		{ 
			return m_MeshSource;
		}

		FORCEINLINE void SetMeshAsset(TObjectPtr<MeshSource> meshAsset) 
		{ 
			m_MeshSource = meshAsset; 
		}

		TObjectPtr<LMaterialTable> GetMaterials() const 
		{ 
			return Materials; 
		}

		static EAssetType GetStaticType() { return EAssetType::StaticMesh; }
		//virtual EAssetType GetAssetType() const override { return GetStaticType(); }

	private:
		TObjectPtr<MeshSource> m_MeshSource;
		std::vector<uint32_t> m_Submeshes; 

		// Materials
		TObjectPtr<LMaterialTable> Materials;

		friend class LScene;
		friend class LRenderer;
		friend class LAssetManager;
		friend class LRuntimeAssetManager;
		friend class LOpenGLRenderer;
		friend class LSceneHierarchyPanel;
		friend class LMeshViewerPanel;

		LASSET(StaticMesh)
	};


}