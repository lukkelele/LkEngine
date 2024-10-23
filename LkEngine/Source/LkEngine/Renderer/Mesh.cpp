#include "LKpch.h"
#include "Mesh.h"

#include "LkEngine/Asset/AssetManager.h"


namespace LkEngine {

    LMeshSource::LMeshSource(const std::vector<FVertex>& InVertices, 
							 const std::vector<FIndex>& InIndices, 
							 const glm::mat4& InTransform)
		: m_Vertices(InVertices)
		, m_Indices(InIndices)
	{
		Handle = {};

		LSubmesh Submesh{};
		Submesh.BaseVertex = 0;
		Submesh.BaseIndex = 0;
		Submesh.IndexCount = static_cast<uint32_t>(InIndices.size() * 3u);
		Submesh.Transform = InTransform;
		m_Submeshes.push_back(Submesh);

		/* Create Vertex and index buffers. */
		m_VertexBuffer = LVertexBuffer::Create(m_Vertices.data(), static_cast<uint64_t>(m_Vertices.size() * sizeof(FVertex)));
		m_IndexBuffer = LIndexBuffer::Create(m_Indices.data(), static_cast<uint64_t>(m_Indices.size() * sizeof(FIndex)));
	}

	LMeshSource::LMeshSource(const std::vector<FVertex>& InVertices, 
							 const std::vector<FIndex>& InIndices, 
							 const std::vector<LSubmesh>& InSubmeshes)
		: m_Vertices(InVertices)
		, m_Indices(InIndices)
		, m_Submeshes(InSubmeshes)
	{
		Handle = {};

		/* Create Vertex and index buffers. */
		m_VertexBuffer = LVertexBuffer::Create(m_Vertices.data(), static_cast<uint64_t>(m_Vertices.size() * sizeof(FVertex)));
		m_IndexBuffer = LIndexBuffer::Create(m_Indices.data(), static_cast<uint64_t>(m_Indices.size() * sizeof(FIndex)));
	}

	static std::string LevelToSpaces(uint32_t level)
	{
		std::string result = "";
		for (uint32_t i = 0; i < level; i++)
		{
			result += "--";
		}

		return result;
	}

	void LMeshSource::DumpVertexBuffer()
	{
		LK_CORE_TRACE("------------------------------------------------------");
		LK_CORE_TRACE("Vertex Buffer Dump");
		LK_CORE_TRACE("Mesh: {0}", m_FilePath);
		for (size_t i = 0; i < m_Vertices.size(); i++)
		{
			const FVertex& Vertex = m_Vertices[i];
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
		: m_MeshSource(InMeshSource)
	{
		Handle = {};

		SetSubmeshes({});

		const std::vector<TObjectPtr<LMaterial>>& MeshMaterials = m_MeshSource->GetMaterials();
		Materials = TObjectPtr<LMaterialTable>::Create(static_cast<uint32_t>(MeshMaterials.size()));

		LK_CORE_DEBUG_TAG("Mesh", "Created new mesh ({}) with {} materials!", Handle, static_cast<uint32_t>(MeshMaterials.size()));
		for (uint32_t i = 0; i < static_cast<decltype(i)>(MeshMaterials.size()); i++)
		{
			TObjectPtr<LMaterialAsset> MaterialAsset = TObjectPtr<LMaterialAsset>::Create(MeshMaterials[i]);
			Materials->SetMaterial(i, MaterialAsset->Handle);
			LK_CORE_TRACE_TAG("Mesh", "Setting material indexed {}", i);
		}
	}

	LMesh::LMesh(TObjectPtr<LMeshSource> meshSource, const std::vector<uint32_t>& submeshes)
		: m_MeshSource(meshSource)
	{
		Handle = {};
		SetSubmeshes(submeshes);

		const std::vector<TObjectPtr<LMaterial>>& MeshMaterials = meshSource->GetMaterials();
		Materials = TObjectPtr<LMaterialTable>::Create(static_cast<uint32_t>(MeshMaterials.size()));

		LK_CORE_DEBUG_TAG("Mesh", "Created new mesh ({}) with {} materials", Handle, static_cast<uint32_t>(MeshMaterials.size()));
		for (uint32_t i = 0; i < static_cast<decltype(i)>(MeshMaterials.size()); i++)
		{
			TObjectPtr<LMaterialAsset> MaterialAsset = TObjectPtr<LMaterialAsset>::Create(MeshMaterials[i]);
			Materials->SetMaterial(i, MaterialAsset->Handle);
			LK_CORE_TRACE_TAG("Mesh", "Setting material indexed {}", i);
		}
	}

	LMesh::LMesh(const TObjectPtr<LMesh>& Other)
		: m_MeshSource(Other->m_MeshSource)
		, Materials(Other->Materials)
	{
		SetSubmeshes(Other->m_Submeshes);
	}

	void LMesh::SetSubmeshes(const std::vector<uint32_t>& InSubmeshes)
	{
		if (!InSubmeshes.empty())
		{
			/// TODO: Set dirty
			m_Submeshes = InSubmeshes;
		}
		else
		{
			/// TODO: Set dirty
			const std::vector<LSubmesh>& Submeshes = m_MeshSource->GetSubmeshes();
			m_Submeshes.resize(Submeshes.size());
			for (uint32_t Index = 0; Index < static_cast<decltype(Index)>(Submeshes.size()); Index++)
			{
				m_Submeshes[Index] = Index;
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
		: m_MeshSource(InMeshSource)
	{
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
		: m_MeshSource(meshSource)
	{
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
		: m_MeshSource(Other->m_MeshSource)
		, Materials(Other->Materials)
	{
		SetSubmeshes(Other->m_Submeshes);
	}

	void LStaticMesh::SetSubmeshes(const std::vector<uint32_t>& InSubmeshes)
	{
		if (!InSubmeshes.empty())
		{
			/// TODO: Set dirty
			m_Submeshes = InSubmeshes;
		}
		else
		{
			/// TODO: Set dirty
			const std::vector<LSubmesh>& Submeshes = m_MeshSource->GetSubmeshes();
			m_Submeshes.resize(Submeshes.size());
			for (uint32_t Index = 0; Index < static_cast<decltype(Index)>(Submeshes.size()); Index++)
			{
				m_Submeshes[Index] = Index;
			}
		}
	}


}