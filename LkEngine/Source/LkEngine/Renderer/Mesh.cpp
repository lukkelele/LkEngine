#include "LKpch.h"
#include "Mesh.h"

#include "LkEngine/Asset/AssetManager.h"


namespace LkEngine {

    MeshSource::MeshSource(const std::vector<Vertex>& vertices, 
						   const std::vector<Index>& indices, 
						   const glm::mat4& transform)
		: m_Vertices(vertices)
		, m_Indices(indices)
	{
		Handle = {};

		Submesh submesh;
		submesh.BaseVertex = 0;
		submesh.BaseIndex = 0;
		submesh.IndexCount = (uint32_t)indices.size() * 3u;
		submesh.Transform = transform;
		m_Submeshes.push_back(submesh);

		m_VertexBuffer = LVertexBuffer::Create(m_Vertices.data(), 
											   (uint32_t)(m_Vertices.size() * sizeof(Vertex)));
		m_IndexBuffer = LIndexBuffer::Create(m_Indices.data(), 
											 (uint32_t)(m_Indices.size() * sizeof(Index)));
	}

	MeshSource::MeshSource(const std::vector<Vertex>& vertices, 
						   const std::vector<Index>& indices, 
						   const std::vector<Submesh>& submeshes)
		: m_Vertices(vertices)
		, m_Indices(indices)
		, m_Submeshes(submeshes)
	{
		Handle = {};

		m_VertexBuffer = LVertexBuffer::Create(m_Vertices.data(), 
											   (uint32_t)(m_Vertices.size() * sizeof(Vertex)));

		m_IndexBuffer = LIndexBuffer::Create(m_Indices.data(), (uint32_t)(m_Indices.size() * sizeof(Index)));
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

	void MeshSource::DumpVertexBuffer()
	{
		// TODO: Convert to ImGui
		LK_CORE_TRACE("------------------------------------------------------");
		LK_CORE_TRACE("Vertex Buffer Dump");
		LK_CORE_TRACE("Mesh: {0}", m_FilePath);
		for (size_t i = 0; i < m_Vertices.size(); i++)
		{
			auto& vertex = m_Vertices[i];
			LK_CORE_TRACE("Vertex: {0}", i);
			LK_CORE_TRACE("Position: {0}, {1}, {2}", vertex.Position.x, vertex.Position.y, vertex.Position.z);
			LK_CORE_TRACE("Normal: {0}, {1}, {2}", vertex.Normal.x, vertex.Normal.y, vertex.Normal.z);
			LK_CORE_TRACE("Binormal: {0}, {1}, {2}", vertex.Binormal.x, vertex.Binormal.y, vertex.Binormal.z);
			LK_CORE_TRACE("Tangent: {0}, {1}, {2}", vertex.Tangent.x, vertex.Tangent.y, vertex.Tangent.z);
			LK_CORE_TRACE("Texcoord: {0}, {1}", vertex.Texcoord.x, vertex.Texcoord.y);
			LK_CORE_TRACE("--");
		}
		LK_CORE_TRACE("------------------------------------------------------");
	}


	Mesh::Mesh(TObjectPtr<MeshSource> meshSource)
		: m_MeshSource(meshSource)
	{
		/* Generate a new asset handle. */
		Handle = {};

		SetSubmeshes({});

		const std::vector<TObjectPtr<LMaterial>>& meshMaterials = meshSource->GetMaterials();
		Materials = TObjectPtr<LMaterialTable>::Create((uint32_t)meshMaterials.size());

		LK_CORE_DEBUG_TAG("Mesh", "Created new mesh ({}) with {} materials!", Handle, (uint32_t)meshMaterials.size());
		for (size_t i = 0; i < meshMaterials.size(); i++)
		{
			TObjectPtr<LMaterialAsset> materialAsset = TObjectPtr<LMaterialAsset>::Create(meshMaterials[i]);
			Materials->SetMaterial((uint32_t)i, materialAsset->Handle);
			LK_CORE_TRACE_TAG("Mesh", "Setting material indexed {}", i);
		}
	}

	Mesh::Mesh(TObjectPtr<MeshSource> meshSource, const std::vector<uint32_t>& submeshes)
		: m_MeshSource(meshSource)
	{
		Handle = {};
		SetSubmeshes(submeshes);

		const auto& meshMaterials = meshSource->GetMaterials();
		Materials = TObjectPtr<LMaterialTable>::Create((uint32_t)meshMaterials.size());

		LK_CORE_DEBUG_TAG("Mesh", "Created new mesh ({}) with {} materials!", Handle, (uint32_t)meshMaterials.size());
		for (size_t i = 0; i < meshMaterials.size(); i++)
		{
			TObjectPtr<LMaterialAsset> materialAsset = TObjectPtr<LMaterialAsset>::Create(meshMaterials[i]);
			Materials->SetMaterial((uint32_t)i, materialAsset->Handle);
			LK_CORE_TRACE_TAG("Mesh", "Setting material indexed {}", i);
		}
	}

	Mesh::Mesh(const Ref<Mesh>& other)
		: m_MeshSource(other->m_MeshSource)
		, Materials(other->Materials)
	{
		SetSubmeshes(other->m_Submeshes);
	}

	Mesh::~Mesh()
	{
	}

	void Mesh::SetSubmeshes(const std::vector<uint32_t>& submeshes)
	{
		if (!submeshes.empty())
		{
			m_Submeshes = submeshes;
		}
		else
		{
			const auto& submeshes = m_MeshSource->GetSubmeshes();
			m_Submeshes.resize(submeshes.size());
			for (uint32_t i = 0; i < submeshes.size(); i++)
				m_Submeshes[i] = i;
		}
	}

	TObjectPtr<LMaterial> Mesh::GetMaterial(const int Index)
	{
		/* TODO: Validate index */
		FAssetHandle asset = Materials->GetMaterialHandle(Index);
		return LAssetManager::GetAsset<LMaterialAsset>(asset)->GetMaterial();
	}


	//-----------------------------------------------------
	// StaticMesh 
	//-----------------------------------------------------
	StaticMesh::StaticMesh(TObjectPtr<MeshSource> meshSource)
		: m_MeshSource(meshSource)
	{
		// Generate a new asset handle
		Handle = {};

		SetSubmeshes({});

		const auto& meshMaterials = meshSource->GetMaterials();
		uint32_t numMaterials = static_cast<uint32_t>(meshMaterials.size());
		Materials = TObjectPtr<LMaterialTable>::Create(numMaterials);
		for (uint32_t i = 0; i < numMaterials; i++)
		{
			Materials->SetMaterial(i, LAssetManager::CreateMemoryOnlyAsset<LMaterialAsset>(meshMaterials[i]));
		}
	}

	StaticMesh::StaticMesh(TObjectPtr<MeshSource> meshSource, const std::vector<uint32_t>& submeshes)
		: m_MeshSource(meshSource)
	{
		// Generate a new asset handle
		Handle = {};

		SetSubmeshes(submeshes);

		const auto& meshMaterials = meshSource->GetMaterials();
		uint32_t numMaterials = static_cast<uint32_t>(meshMaterials.size());
		Materials = TObjectPtr<LMaterialTable>::Create(numMaterials);
		for (uint32_t i = 0; i < numMaterials; i++)
		{
			Materials->SetMaterial(i, LAssetManager::CreateMemoryOnlyAsset<LMaterialAsset>(meshMaterials[i]));
		}
	}

	StaticMesh::StaticMesh(const Ref<StaticMesh>& other)
		: m_MeshSource(other->m_MeshSource)
		, Materials(other->Materials)
	{
		SetSubmeshes(other->m_Submeshes);
	}

	StaticMesh::~StaticMesh()
	{
	}

	void StaticMesh::SetSubmeshes(const std::vector<uint32_t>& submeshes)
	{
		if (!submeshes.empty())
		{
			m_Submeshes = submeshes;
		}
		else
		{
			const auto& submeshes = m_MeshSource->GetSubmeshes();
			m_Submeshes.resize(submeshes.size());
			for (uint32_t i = 0; i < submeshes.size(); i++)
			{
				m_Submeshes[i] = i;
			}
		}
	}


}