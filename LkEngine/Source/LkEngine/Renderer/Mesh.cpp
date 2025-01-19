#include "LKpch.h"
#include "Mesh.h"

#include "LkEngine/Renderer/MaterialLibrary.h"

#include "LkEngine/Asset/AssetManager.h"


namespace LkEngine {

	LMeshSource::LMeshSource()
	{
		LASSET_REGISTER();

		Handle = {};

		Submeshes.clear();

		LK_CORE_TRACE_TAG("MeshSource", "Created MeshSource ({})  Vertices={}  Indices={}", Handle, Vertices.size(), Indices.size());
	}

	LMeshSource::LMeshSource(const std::vector<FVertex>& InVertices, const std::vector<FIndex>& InIndices, const glm::mat4& InTransform)
		: Vertices(InVertices)
		, Indices(InIndices)
	{
		LASSET_REGISTER();

		Handle = {};
		Submeshes.clear();

		LSubmesh Submesh{};
		Submesh.BaseVertex = 0;
		Submesh.BaseIndex = 0;
		Submesh.IndexCount = (uint32_t)(InIndices.size() * 3u);
		Submesh.Transform = InTransform;
		Submeshes.push_back(Submesh);

		/* Create Vertex and index buffers. */
		VertexBuffer = LVertexBuffer::Create(
			Vertices.data(), 
			(uint32_t)(Vertices.size() * sizeof(FVertex))
		);

		IndexBuffer = LIndexBuffer::Create(
			Indices.data(), 
			(uint32_t)(Indices.size() * sizeof(FIndex))
		);

		LK_CORE_ASSERT(Submesh.IndexCount == IndexBuffer->GetCount(), "IndexBuffer count mismatch");
		LK_CORE_TRACE_TAG("MeshSource", "Created MeshSource ({})  Vertices={}  Indices={}", Handle, Vertices.size(), Indices.size());
	}

	LMeshSource::LMeshSource(const std::vector<FVertex>& InVertices, const std::vector<FIndex>& InIndices, const std::vector<LSubmesh>& InSubmeshes)
		: Vertices(InVertices)
		, Indices(InIndices)
		, Submeshes(InSubmeshes)
	{
		LASSET_REGISTER();

		Handle = {};

		/* Create Vertex and index buffers. */
		VertexBuffer = LVertexBuffer::Create(
			Vertices.data(), 
			(uint32_t)(Vertices.size() * sizeof(FVertex))
		);

		IndexBuffer = LIndexBuffer::Create(
			Indices.data(), 
			(uint32_t)(Indices.size() * sizeof(FIndex))
		);

		LK_CORE_TRACE_TAG("MeshSource", "Created MeshSource ({})  Vertices={}  Indices={}", Handle, Vertices.size(), Indices.size());
	}

	void LMeshSource::DumpVertexBuffer()
	{
		LK_CORE_INFO("------------------------------------------------------");
		LK_CORE_INFO("[Mesh: {}]", FilePath.string());
		LK_CORE_INFO("Vertex Buffer Dump");
		for (size_t i = 0; i < Vertices.size(); i++)
		{
			const FVertex& Vertex = Vertices[i];
			LK_CORE_INFO("Vertex: {0}", i);
			LK_CORE_INFO("Position: {0}, {1}, {2}", Vertex.Position.x, Vertex.Position.y, Vertex.Position.z);
			LK_CORE_INFO("Normal: {0}, {1}, {2}", Vertex.Normal.x, Vertex.Normal.y, Vertex.Normal.z);
			LK_CORE_INFO("Binormal: {0}, {1}, {2}", Vertex.Binormal.x, Vertex.Binormal.y, Vertex.Binormal.z);
			LK_CORE_INFO("Tangent: {0}, {1}, {2}", Vertex.Tangent.x, Vertex.Tangent.y, Vertex.Tangent.z);
			LK_CORE_INFO("Texcoord: {0}, {1}", Vertex.Texcoord.x, Vertex.Texcoord.y);
			LK_CORE_INFO("--");
		}
		LK_CORE_INFO("------------------------------------------------------");
	}

	/*----------------------------------------------------------------------------------------*/

	LMesh::LMesh(TObjectPtr<LMeshSource> InMeshSource)
		: MeshSource(InMeshSource)
	{
		LASSET_REGISTER();

		Handle = {};

		LK_CORE_VERIFY(MeshSource->Handle == InMeshSource->Handle);
		SetSubmeshes({});

		MaterialTable = TObjectPtr<LMaterialTable>::Create(static_cast<uint32_t>(MeshSource->GetMaterials().size()));

		auto& MaterialMap = MaterialTable->GetMaterials();
		for (uint32_t i = 0; i < static_cast<uint32_t>(MaterialMap.size()); i++)
		{
			TObjectPtr<LMaterialAsset> MaterialAsset = TObjectPtr<LMaterialAsset>::Create(MaterialMap[i]);
			MaterialTable->SetMaterial(i, MaterialAsset->Handle);
		}

		/* Load base material if no other materials exist. */
		if (MaterialTable->GetMaterialCount() == 0)
		{
			LK_CORE_WARN_TAG("Mesh", "Material table is empty, setting base material as first entry");
			TObjectPtr<LMaterialAsset> BaseMatAsset = LMaterialLibrary::Get().GetMaterial(BASE_MATERIAL);
			LK_CORE_VERIFY(BaseMatAsset, "Base material asset is not valid");
			LK_CORE_VERIFY(BaseMatAsset->GetMaterial(), "BaseMaterialAsset does not contain a material");
			MaterialTable->SetMaterial(0, BaseMatAsset->Handle);
		}

		LK_CORE_TRACE_TAG("Mesh", "Created new mesh ({}) with {} materials", Handle, MaterialTable->GetMaterials().size());
	}

	LMesh::LMesh(TObjectPtr<LMeshSource> InMeshSource, const std::vector<uint32_t>& InSubmeshes)
		: MeshSource(InMeshSource)
	{
		LASSET_REGISTER();

		Handle = {};
		SetSubmeshes(InSubmeshes);

		MaterialTable = TObjectPtr<LMaterialTable>::Create(static_cast<uint32_t>(MeshSource->GetMaterials().size()));

		auto& MaterialMap = MaterialTable->GetMaterials();
		for (uint32_t i = 0; i < static_cast<uint32_t>(MaterialMap.size()); i++)
		{
			TObjectPtr<LMaterialAsset> MaterialAsset = TObjectPtr<LMaterialAsset>::Create(MaterialMap[i]);
			MaterialTable->SetMaterial(i, MaterialAsset->Handle);
		}

		/* Load base material if no other materials exist. */
		if (MaterialTable->GetMaterialCount() == 0)
		{
			LK_CORE_WARN_TAG("Mesh", "Material table is empty, setting base material as first entry");
			TObjectPtr<LMaterialAsset> BaseMatAsset = LMaterialLibrary::Get().GetMaterial(BASE_MATERIAL);
			LK_CORE_VERIFY(BaseMatAsset, "Base material asset is not valid");
			LK_CORE_VERIFY(BaseMatAsset->GetMaterial(), "BaseMaterialAsset does not contain a material");
			MaterialTable->SetMaterial(0, BaseMatAsset->Handle);
		}

		LK_CORE_TRACE_TAG("Mesh", "Created new mesh ({}) with {} materials", Handle, MaterialTable->GetMaterials().size());
	}

	LMesh::LMesh(const TObjectPtr<LMesh>& Other)
		: MeshSource(Other->MeshSource)
		, MaterialTable(Other->MaterialTable)
	{
		LASSET_REGISTER();
		
		SetSubmeshes(Other->Submeshes);

		LK_CORE_TRACE_TAG("Mesh", "Created mesh ({}) from other mesh", Handle);
	}

	void LMesh::SetSubmeshes(const std::vector<uint32_t>& InSubmeshes)
	{
		/// TODO: Set dirty
		if (!InSubmeshes.empty())
		{
			Submeshes = InSubmeshes;
		}
		else
		{
			const std::vector<LSubmesh>& SourceSubmeshes = MeshSource->GetSubmeshes();
			Submeshes.resize(SourceSubmeshes.size());
			for (uint32_t Index = 0; Index < static_cast<uint32_t>(Submeshes.size()); Index++)
			{
				Submeshes[Index] = Index;
			}
		}
	}

	TObjectPtr<LMaterial> LMesh::GetMaterial(const uint32_t Index)
	{
		LK_CORE_ASSERT(Index >= 0, "Index is less than 0");
		LK_CORE_VERIFY(MaterialTable->HasMaterial(Index), "Index {} is not valid", Index);

		const FAssetHandle Handle = MaterialTable->GetMaterial(Index);
		//auto Asset = LAssetManager::GetAsset<LMaterialAsset>(AssetHandle)->GetMaterial();
		TObjectPtr<LMaterialAsset> Asset = LAssetManager::GetAsset<LMaterialAsset>(Handle);
		TObjectPtr<LMaterial> Material = Asset->GetMaterial();
		//return Asset.As<LMaterial>();

		return Material;
	}

	/*----------------------------------------------------------------------------------------*/

	LStaticMesh::LStaticMesh(TObjectPtr<LMeshSource> InMeshSource)
		: MeshSource(InMeshSource)
	{
		LASSET_REGISTER();

		Handle = {};

		SetSubmeshes({});

		const std::vector<TObjectPtr<LMaterial>>& MeshMaterials = InMeshSource->GetMaterials();
		const uint32_t NumMaterials = static_cast<uint32_t>(MeshMaterials.size());
		MaterialTable = TObjectPtr<LMaterialTable>::Create(NumMaterials);
		for (uint32_t i = 0; i < NumMaterials; i++)
		{
			MaterialTable->SetMaterial(i, LAssetManager::CreateMemoryOnlyAsset<LMaterialAsset>(MeshMaterials[i]));
		}
	}

	LStaticMesh::LStaticMesh(TObjectPtr<LMeshSource> InMeshSource, const std::vector<uint32_t>& InSubmeshes)
		: MeshSource(InMeshSource)
	{
		LASSET_REGISTER();

		Handle = {};

		SetSubmeshes(InSubmeshes);

		const std::vector<TObjectPtr<LMaterial>>& MeshMaterials = MeshSource->GetMaterials();
		const uint32_t NumMaterials = static_cast<uint32_t>(MeshMaterials.size());
		MaterialTable = TObjectPtr<LMaterialTable>::Create(NumMaterials);
		for (uint32_t i = 0; i < NumMaterials; i++)
		{
			MaterialTable->SetMaterial(i, LAssetManager::CreateMemoryOnlyAsset<LMaterialAsset>(MeshMaterials[i]));
			LK_CORE_DEBUG_TAG("StaticMesh", "Setting material {} (memory-only asset): {}", i, MeshMaterials[i]->GetName());;
		}

		LK_CORE_TRACE_TAG("StaticMesh", "Created ({}) with {} submeshes", InMeshSource->GetFilePath().filename().string(), InSubmeshes.size());
	}

	LStaticMesh::LStaticMesh(const TObjectPtr<LStaticMesh>& Other)
		: MeshSource(Other->MeshSource)
		, MaterialTable(Other->MaterialTable)
	{
		LASSET_REGISTER();

		SetSubmeshes(Other->Submeshes);
	}

	void LStaticMesh::SetSubmeshes(const std::vector<uint32_t>& InSubmeshes)
	{
		/// TODO: Set dirty
		if (!InSubmeshes.empty())
		{
			Submeshes = InSubmeshes;
		}
		else
		{
			const std::vector<LSubmesh>& SourceSubmeshes = MeshSource->GetSubmeshes();
			Submeshes.resize(SourceSubmeshes.size());
			for (uint32_t Index = 0; Index < static_cast<uint32_t>(Submeshes.size()); Index++)
			{
				Submeshes[Index] = Index;
			}
		}
	}

}
