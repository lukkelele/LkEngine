#include "LKpch.h"
#include "RuntimeAssetManager.h"

#include "AssetManager.h"

#include "LkEngine/Project/Project.h"
#include "LkEngine/Scene/Entity.h"

#include "LkEngine/Renderer/Renderer.h"

namespace LkEngine {

	static FAssetMetadata NullMetadata{};

	/** 
	 * FRuntimeCache 
	 *
	 *  Cached and loaded assets.
	 */
	struct FRuntimeCache
	{
		std::vector<TTexture2DPair> Textures2D{};
	};

	static FRuntimeCache RuntimeCache{};


	LRuntimeAssetManager::LRuntimeAssetManager()
		: TextureLibrary(LTextureLibrary::Get())
	{
		LCLASS_REGISTER();
	}

	void LRuntimeAssetManager::Initialize(const EInitFlag AssetInitialization)
	{
		LObject::Initialize();
		if (AssetInitialization == EInitFlag::NoInit)
		{
			/* Do nothing. */
			LK_CORE_DEBUG_TAG("RuntimeAssetManager", "Skipping initialization of LMaterialLibrary");
		}
		else if (AssetInitialization == EInitFlag::True)
		{
			LoadMaterials();

			/* Load all primitive shapes. */
			LoadPrimitiveShapes();

			/* Load textures into cache. */
			TextureLibrary.GetTextures2D(RuntimeCache.Textures2D);
		}
	}

	void LRuntimeAssetManager::AddMemoryOnlyAsset(TObjectPtr<LAsset> asset)
	{
		m_MemoryAssets[asset->Handle] = asset;
	}

	bool LRuntimeAssetManager::ReloadData(FAssetHandle AssetHandle)
	{
		LK_CORE_ASSERT(false, "Not implemented");
		return true;
	}

	bool LRuntimeAssetManager::IsAssetHandleValid(FAssetHandle AssetHandle)
	{
		if (AssetHandle == 0)
		{
			return false;
		}

		return IsMemoryAsset(AssetHandle);
	}

	EAssetType LRuntimeAssetManager::GetAssetType(FAssetHandle AssetHandle)
	{
		if (TObjectPtr<LAsset> Asset = GetAsset(AssetHandle))
		{
			return Asset->GetAssetType();
		}

		return EAssetType::None;
	}

	template<typename T>
	TObjectPtr<T> LRuntimeAssetManager::ImportAsset(const std::filesystem::path InFilePath)
	{
		LK_UNUSED(InFilePath);
	}
	
	template<>
    TObjectPtr<LMesh> LRuntimeAssetManager::ImportAsset<LMesh>(const std::filesystem::path InFilePath)
    {
        LAssimpMeshImporter MeshImporter = LAssimpMeshImporter(InFilePath);

        TObjectPtr<LMeshSource> MeshSource = MeshImporter.ImportToMeshSource();
		TObjectPtr<LMesh> Mesh = TObjectPtr<LMesh>::Create(MeshSource);

		m_LoadedAssets[Mesh->Handle] = Mesh;
		LK_CORE_TRACE_TAG("RuntimeAssetManager", "Loaded mesh with {} vertices, {} indices and a handle of '{}'", 
						  MeshSource->GetVertices().size(), MeshSource->GetIndices().size(), Mesh->Handle);

		return m_LoadedAssets[Mesh->Handle];
    }

	TObjectPtr<LAsset> LRuntimeAssetManager::GetAsset(const FAssetHandle AssetHandle)
    {
		if (IsMemoryAsset(AssetHandle))
		{
			return m_MemoryAssets[AssetHandle];
		}

		FAssetMetadata& metadata = GetMetadataInternal(AssetHandle);
		if (!metadata.IsValid())
		{
			LK_CORE_WARN_TAG("RuntimeAssetManager", "Metadata is invalid for {}", AssetHandle);
			return nullptr;
		}

		TObjectPtr<LAsset> Asset = nullptr;
		if (!metadata.IsDataLoaded)
		{
			if (!metadata.IsDataLoaded)
			{
				return nullptr;
			}

			m_LoadedAssets[AssetHandle] = Asset;
		}
		else
		{
			Asset = m_LoadedAssets[AssetHandle];
		}

		return Asset;
    }

    const FAssetMetadata& LRuntimeAssetManager::GetMetadata(FAssetHandle handle)
	{
		if (AssetRegistry.Contains(handle))
		{
			return AssetRegistry[handle];
		}

		return NullMetadata;
	}

    FAssetMetadata& LRuntimeAssetManager::GetMetadataInternal(FAssetHandle handle)
	{
		if (AssetRegistry.Contains(handle))
		{
	    	return AssetRegistry[handle];
		}

	    return NullMetadata; 
	}

    FAssetHandle LRuntimeAssetManager::GetAssetHandleFromFilePath(const std::filesystem::path& FilePath)
	{
		return GetMetadata(FilePath).Handle;
	}

    const FAssetMetadata& LRuntimeAssetManager::GetMetadata(const TObjectPtr<LAsset>& asset)
	{
		return GetMetadata(asset->Handle);
	}

	const FAssetMetadata& LRuntimeAssetManager::GetMetadata(const std::filesystem::path& FilePath)
	{
		LK_CORE_ASSERT(AssetRegistry.Count() > 0, "AssetRegistry is empty!");

		const std::filesystem::path RelativePath = GetRelativePath(FilePath);
		for (auto& [handle, metadata] : AssetRegistry)
		{
			if (metadata.FilePath == RelativePath)
			{
				return metadata;
			}
		}

		return NullMetadata;
	}

	std::filesystem::path LRuntimeAssetManager::GetRelativePath(const std::filesystem::path& FilePath)
	{
		/* Return rvalue so we trigger move semantics for performance. */
		return FilePath.lexically_normal();
	}

	void LRuntimeAssetManager::WriteRegistryToFile()
	{
	}

	EAssetType LRuntimeAssetManager::GetAssetTypeFromExtension(const std::string& extension)
	{
		LK_CORE_ASSERT(false, "Not implemented yet!");
		return EAssetType::None;
	}

	EAssetType LRuntimeAssetManager::GetAssetTypeFromPath(const std::filesystem::path& path)
	{
		LK_CORE_ASSERT(false, "Not implemented yet!");
		return GetAssetTypeFromExtension(path.extension().string());
	}

	const std::vector<TTexture2DPair>& LRuntimeAssetManager::GetTextures2D() const
	{
		return RuntimeCache.Textures2D;
	}

	int LRuntimeAssetManager::GetTextures2D(std::vector<TTexture2DPair>& TextureContainer)
	{
		return TextureLibrary.GetTextures2D(TextureContainer);
	}

	void LRuntimeAssetManager::LoadMaterials()
	{
		LMaterialLibrary& MaterialLibrary = LMaterialLibrary::Get();

		// Asphalt
		{
			TObjectPtr<LMaterial> AsphaltMaterial = MaterialLibrary.GetMaterial("Basic_Asphalt");
			AsphaltMaterial->SetTexture(TextureLibrary.GetWhiteTexture());
			TObjectPtr<LMaterialAsset> AsphaltMaterialAsset = TObjectPtr<LMaterialAsset>::Create(AsphaltMaterial);
			m_MemoryAssets[AsphaltMaterialAsset->Handle] = AsphaltMaterialAsset;

			FAssetMetadata metadata;
			metadata.Handle = AsphaltMaterialAsset->Handle;
			metadata.Type = EAssetType::Material;
			AssetRegistry[AsphaltMaterialAsset->Handle] = metadata;
		}

		// Rubber
		{
			TObjectPtr<LMaterial> RubberMaterial = MaterialLibrary.GetMaterial("Basic_Rubber");
			RubberMaterial->SetTexture(TextureLibrary.GetWhiteTexture());

			TObjectPtr<LMaterialAsset> RubberMaterialAsset = TObjectPtr<LMaterialAsset>::Create(RubberMaterial);
			m_MemoryAssets[RubberMaterialAsset->Handle] = RubberMaterialAsset;

			FAssetMetadata metadata;
			metadata.Handle = RubberMaterialAsset->Handle;
			metadata.Type = EAssetType::Material;
			AssetRegistry[RubberMaterialAsset->Handle] = metadata;
		}
	}

    void LRuntimeAssetManager::LoadPrimitiveShapes()
    {
		LK_CORE_INFO_TAG("RuntimeAssetManager", "Creating primitive shapes");

		TObjectPtr<LProject> project = LProject::Current();
		LMaterialLibrary& MaterialLibrary = LMaterialLibrary::Get();

		LK_CORE_WARN_TAG("RuntimeAssetManager", "Loading primitive shapes for project {}", project->GetName());
		/* Cube. */
		{
			std::filesystem::path FilePath("Assets/Meshes/Cube.gltf");
			LEntity CubeEntity = LProject::Current()->GetScene()->CreateEntity("Cube");
			TObjectPtr<LMesh> Cube = ImportAsset<LMesh>(FilePath.string());
			LMeshComponent& MeshComponent = CubeEntity.AddComponent<LMeshComponent>(Cube->Handle);

			/* Assign the Cube with the 'BaseMaterial' */
			LTextureLibrary& TextureLibrary = LTextureLibrary::Get();
			TObjectPtr<LMaterial> baseMaterial = MaterialLibrary.GetBaseMaterial();
            baseMaterial->SetTexture(TextureLibrary.GetTexture("wood-container_512x512"));
			TObjectPtr<LMaterialAsset> BaseMaterialAsset = TObjectPtr<LMaterialAsset>::Create(baseMaterial);

			Cube->Materials->SetMaterial(0, BaseMaterialAsset->Handle);
			LK_CORE_ASSERT(baseMaterial->GetTexture(""), "BaseMaterial texture is nullptr");

			FAssetMetadata CubeMetadata;
			CubeMetadata.Handle = Cube->Handle;
			CubeMetadata.Type = EAssetType::Mesh;
			CubeMetadata.FilePath = FilePath;
			CubeMetadata.IsDataLoaded = true;
			AssetRegistry[Cube->Handle] = CubeMetadata;

			FAssetMetadata baseMaterialFAssetMetadata;
			baseMaterialFAssetMetadata.Handle = BaseMaterialAsset->Handle;
			baseMaterialFAssetMetadata.Type = EAssetType::Material;
			baseMaterialFAssetMetadata.IsDataLoaded = true;
			AssetRegistry[BaseMaterialAsset->Handle] = baseMaterialFAssetMetadata;

			TObjectPtr<LMeshSource> CubeSource = Cube->GetMeshSource();
			CubeSource->m_VertexBuffer->SetLayout({
			    { "a_Position",   EShaderDataType::Float3 },
			    { "a_Normal",     EShaderDataType::Float3 },
			    { "a_Binormal",   EShaderDataType::Float3 },
			    { "a_Tangent",    EShaderDataType::Float3 },
			    { "a_TexCoord",   EShaderDataType::Float2 },
			});
			CubeSource->m_VertexBuffer->SetIndexBuffer(CubeSource->m_IndexBuffer);

			m_MemoryAssets[Cube->Handle] = Cube;
			m_MemoryAssets[BaseMaterialAsset->Handle] = BaseMaterialAsset;

			/* Test out the retrieval of the texture before continuing. */
			{
				const FAssetHandle CubeMaterialHandle = Cube->GetMaterials()->GetMaterialHandle(0);
				TObjectPtr<LMaterial> CubeMaterial = LAssetManager::GetAsset<LMaterialAsset>(CubeMaterialHandle)->GetMaterial();
				LK_CORE_VERIFY(CubeMaterial, "Retrieved material for Cube is nullptr!");
			}

			LK_CORE_DEBUG_TAG("RuntimeAssetManager", "Cube created with handle {}", Cube->Handle);
		}
    }


}