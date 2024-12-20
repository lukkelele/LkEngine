#include "LKpch.h"
#include "Mesh.h"

#include "LkEngine/Asset/AssetManager.h"


namespace LkEngine {

	static std::string LevelToSpaces(uint32_t level)
	{
		std::string result = "";
		for (uint32_t i = 0; i < level; i++)
		{
			result += "--";
		}

		return result;
	}

	LMeshSource::LMeshSource()
	{
		LASSET_REGISTER();
	}

	LMeshSource::LMeshSource(const std::vector<FVertex>& InVertices, const std::vector<FIndex>& InIndices, const glm::mat4& InTransform)
		: Vertices(InVertices)
		, Indices(InIndices)
	{
		LASSET_REGISTER();
		Handle = {};

		LSubmesh Submesh{};
		Submesh.BaseVertex = 0;
		Submesh.BaseIndex = 0;
		Submesh.IndexCount = static_cast<uint32_t>(InIndices.size() * 3u);
		Submesh.Transform = InTransform;
		Submeshes.push_back(Submesh);

		/* Create Vertex and index buffers. */
		VertexBuffer = LVertexBuffer::Create(Vertices.data(), static_cast<uint32_t>(Vertices.size() * sizeof(FVertex)));
		IndexBuffer = LIndexBuffer::Create(Indices.data(), static_cast<uint32_t>(Indices.size() * sizeof(FIndex)));
		LK_CORE_ASSERT(Submesh.IndexCount == IndexBuffer->GetCount(), "IndexBuffer count mismatch");
	}

	LMeshSource::LMeshSource(const std::vector<FVertex>& InVertices, const std::vector<FIndex>& InIndices, const std::vector<LSubmesh>& InSubmeshes)
		: Vertices(InVertices)
		, Indices(InIndices)
		, Submeshes(InSubmeshes)
	{
		LASSET_REGISTER();
		Handle = {};

		/* Create Vertex and index buffers. */
		VertexBuffer = LVertexBuffer::Create(Vertices.data(), static_cast<uint64_t>(Vertices.size() * sizeof(FVertex)));
		IndexBuffer = LIndexBuffer::Create(Indices.data(), static_cast<uint64_t>(Indices.size() * sizeof(FIndex)));
	}

	void LMeshSource::DumpVertexBuffer()
	{
		LK_CORE_TRACE("------------------------------------------------------");
		LK_CORE_TRACE("Vertex Buffer Dump");
		LK_CORE_TRACE("Mesh: {0}", m_FilePath.string());
		for (size_t i = 0; i < Vertices.size(); i++)
		{
			const FVertex& Vertex = Vertices[i];
			LK_CORE_TRACE("Vertex: {0}", i);
			LK_CORE_TRACE("Position: {0}, {1}, {2}", Vertex.Position.x, Vertex.Position.y, Vertex.Position.z);
			LK_CORE_TRACE("Normal: {0}, {1}, {2}", Vertex.Normal.x, Vertex.Normal.y, Vertex.Normal.z);
			LK_CORE_TRACE("Binormal: {0}, {1}, {2}", Vertex.Binormal.x, Vertex.Binormal.y, Vertex.Binormal.z);
			LK_CORE_TRACE("Tangent: {0}, {1}, {2}", Vertex.Tangent.x, Vertex.Tangent.y, Vertex.Tangent.z);
			LK_CORE_TRACE("Texcoord: {0}, {1}", Vertex.Texcoord.x, Vertex.Texcoord.y);
			LK_CORE_TRACE("--");
		}
		LK_CORE_TRACE("------------------------------------------------------");
	}


	LMesh::LMesh(TObjectPtr<LMeshSource> InMeshSource)
		: MeshSource(InMeshSource)
	{
		LASSET_REGISTER();
		Handle = {};

		SetSubmeshes({});

		const std::vector<TObjectPtr<LMaterial>>& MeshMaterials = MeshSource->GetMaterials();
		Materials = TObjectPtr<LMaterialTable>::Create(static_cast<uint32_t>(MeshMaterials.size()));

		LK_CORE_DEBUG_TAG("Mesh", "Created new mesh ({}) with {} materials", Handle, MeshMaterials.size());
		for (uint32_t i = 0; i < static_cast<uint32_t>(MeshMaterials.size()); i++)
		{
			TObjectPtr<LMaterialAsset> MaterialAsset = TObjectPtr<LMaterialAsset>::Create(MeshMaterials[i]);
			Materials->SetMaterial(i, MaterialAsset->Handle);
			LK_CORE_TRACE_TAG("Mesh", "Setting material indexed {}", i);
		}
	}

	LMesh::LMesh(TObjectPtr<LMeshSource> InMeshSource, const std::vector<uint32_t>& InSubmeshes)
		: MeshSource(InMeshSource)
	{
		LASSET_REGISTER();
		Handle = {};
		SetSubmeshes(InSubmeshes);

		const std::vector<TObjectPtr<LMaterial>>& MeshMaterials = InMeshSource->GetMaterials();
		Materials = TObjectPtr<LMaterialTable>::Create(static_cast<uint32_t>(MeshMaterials.size()));

		LK_CORE_DEBUG_TAG("Mesh", "Created new mesh ({}) with {} materials", Handle, MeshMaterials.size());
		for (uint32_t i = 0; i < static_cast<uint32_t>(MeshMaterials.size()); i++)
		{
			TObjectPtr<LMaterialAsset> MaterialAsset = TObjectPtr<LMaterialAsset>::Create(MeshMaterials[i]);
			Materials->SetMaterial(i, MaterialAsset->Handle);
			LK_CORE_TRACE_TAG("Mesh", "Setting material indexed {}", i);
		}
	}

	LMesh::LMesh(const TObjectPtr<LMesh>& Other)
		: MeshSource(Other->MeshSource)
		, Materials(Other->Materials)
	{
		LASSET_REGISTER();
		SetSubmeshes(Other->Submeshes);
	}

	void LMesh::SetSubmeshes(const std::vector<uint32_t>& InSubmeshes)
	{
		if (!InSubmeshes.empty())
		{
			/// TODO: Set dirty
			Submeshes = InSubmeshes;
		}
		else
		{
			/// TODO: Set dirty
			const std::vector<LSubmesh>& SourceSubmeshes = MeshSource->GetSubmeshes();
			Submeshes.resize(SourceSubmeshes.size());
			for (uint32_t Index = 0; Index < static_cast<uint32_t>(Submeshes.size()); Index++)
			{
				Submeshes[Index] = Index;
			}
		}
	}

	TObjectPtr<LMaterial> LMesh::GetMaterial(const int Index)
	{
		/* TODO: Validate index */
		const FAssetHandle AssetHandle = Materials->GetMaterialHandle(Index);
		return LAssetManager::GetAsset<LMaterialAsset>(AssetHandle)->GetMaterial();
	}


	LStaticMesh::LStaticMesh(TObjectPtr<LMeshSource> InMeshSource)
		: MeshSource(InMeshSource)
	{
		LASSET_REGISTER();
		Handle = {};

		SetSubmeshes({});

		const std::vector<TObjectPtr<LMaterial>>& MeshMaterials = InMeshSource->GetMaterials();
		const uint32_t NumMaterials = static_cast<uint32_t>(MeshMaterials.size());
		Materials = TObjectPtr<LMaterialTable>::Create(NumMaterials);
		for (uint32_t i = 0; i < NumMaterials; i++)
		{
			Materials->SetMaterial(i, LAssetManager::CreateMemoryOnlyAsset<LMaterialAsset>(MeshMaterials[i]));
		}
	}

	LStaticMesh::LStaticMesh(TObjectPtr<LMeshSource> meshSource, const std::vector<uint32_t>& submeshes)
		: MeshSource(meshSource)
	{
		LASSET_REGISTER();
		Handle = {};

		SetSubmeshes(submeshes);

		const auto& MeshMaterials = meshSource->GetMaterials();
		uint32_t NumMaterials = static_cast<uint32_t>(MeshMaterials.size());
		Materials = TObjectPtr<LMaterialTable>::Create(NumMaterials);
		for (uint32_t i = 0; i < NumMaterials; i++)
		{
			Materials->SetMaterial(i, LAssetManager::CreateMemoryOnlyAsset<LMaterialAsset>(MeshMaterials[i]));
		}
	}

	LStaticMesh::LStaticMesh(const TObjectPtr<LStaticMesh>& Other)
		: MeshSource(Other->MeshSource)
		, Materials(Other->Materials)
	{
		LASSET_REGISTER();
		SetSubmeshes(Other->Submeshes);
	}

	void LStaticMesh::SetSubmeshes(const std::vector<uint32_t>& InSubmeshes)
	{
		if (!InSubmeshes.empty())
		{
			/// TODO: Set dirty
			Submeshes = InSubmeshes;
		}
		else
		{
			/// TODO: Set dirty
			const std::vector<LSubmesh>& SourceSubmeshes = MeshSource->GetSubmeshes();
			Submeshes.resize(SourceSubmeshes.size());
			for (uint32_t Index = 0; Index < static_cast<uint32_t>(Submeshes.size()); Index++)
			{
				Submeshes[Index] = Index;
			}
		}
	}


}
