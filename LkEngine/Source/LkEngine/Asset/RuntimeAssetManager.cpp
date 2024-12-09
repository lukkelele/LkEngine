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

	void LRuntimeAssetManager::Initialize()
	{
		LObject::Initialize();
		LK_CORE_TRACE_TAG("RuntimeAssetManager", "Loading materials");
		LoadMaterials();

		/* Load all primitive shapes. */
		LK_CORE_TRACE_TAG("RuntimeAssetManager", "Loading primitive shapes");
		LoadPrimitiveShapes();

		/* Load textures into cache. */
		TextureLibrary.GetTextures2D(RuntimeCache.Textures2D);
	}

	void LRuntimeAssetManager::AddMemoryOnlyAsset(TObjectPtr<LAsset> asset)
	{
		MemoryAssets[asset->Handle] = asset;
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
		LK_CORE_DEBUG_TAG("RuntimeAssetManager", "Importing asset from: {}", InFilePath.string());
        LAssimpMeshImporter MeshImporter = LAssimpMeshImporter(InFilePath);

        TObjectPtr<LMeshSource> MeshSource = MeshImporter.ImportToMeshSource();
		TObjectPtr<LMesh> Mesh = TObjectPtr<LMesh>::Create(MeshSource);

		m_LoadedAssets[Mesh->Handle] = Mesh;
		LK_CORE_TRACE_TAG("RuntimeAssetManager", "Loaded mesh {} with {} vertices and {}", 
						  Mesh->Handle, MeshSource->GetVertices().size(), MeshSource->GetIndices().size());

		return m_LoadedAssets[Mesh->Handle];
    }

	TObjectPtr<LAsset> LRuntimeAssetManager::GetAsset(const FAssetHandle AssetHandle)
    {
		if (IsMemoryAsset(AssetHandle))
		{
			return MemoryAssets[AssetHandle];
		}

		FAssetMetadata& Metadata = GetMetadataInternal(AssetHandle);
		if (!Metadata.IsValid())
		{
			LK_CORE_WARN_TAG("RuntimeAssetManager", "Metadata is invalid for {}", AssetHandle);
			return nullptr;
		}

		TObjectPtr<LAsset> Asset = nullptr;
		if (!Metadata.IsDataLoaded)
		{
			if (!Metadata.IsDataLoaded)
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

    const FAssetMetadata& LRuntimeAssetManager::GetMetadata(const FAssetHandle Handle)
	{
		if (AssetRegistry.Contains(Handle))
		{
			return AssetRegistry[Handle];
		}

		return NullMetadata;
	}

    FAssetMetadata& LRuntimeAssetManager::GetMetadataInternal(const FAssetHandle Handle)
	{
		if (AssetRegistry.Contains(Handle))
		{
	    	return AssetRegistry[Handle];
		}

	    return NullMetadata; 
	}

    FAssetHandle LRuntimeAssetManager::GetAssetHandleFromFilePath(const std::filesystem::path& FilePath)
	{
		return GetMetadata(FilePath).Handle;
	}

    const FAssetMetadata& LRuntimeAssetManager::GetMetadata(const TObjectPtr<LAsset>& Asset)
	{
		return GetMetadata(Asset->Handle);
	}

	const FAssetMetadata& LRuntimeAssetManager::GetMetadata(const std::filesystem::path& FilePath)
	{
		LK_CORE_ASSERT(AssetRegistry.Count() > 0, "AssetRegistry is empty");

		const std::filesystem::path RelativePath = GetRelativePath(FilePath);
		for (auto& [Handle, Metadata] : AssetRegistry)
		{
			if (Metadata.FilePath == RelativePath)
			{
				return Metadata;
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
		#if 0 
			TObjectPtr<LMaterial> AsphaltMaterial = MaterialLibrary.GetMaterial("Basic_Asphalt");
			AsphaltMaterial->SetTexture(TextureLibrary.GetWhiteTexture());
			TObjectPtr<LMaterialAsset> AsphaltMaterialAsset = TObjectPtr<LMaterialAsset>::Create(AsphaltMaterial);
			MemoryAssets[AsphaltMaterialAsset->Handle] = AsphaltMaterialAsset;

			FAssetMetadata Metadata;
			Metadata.Handle = AsphaltMaterialAsset->Handle;
			Metadata.Type = EAssetType::Material;
			AssetRegistry[AsphaltMaterialAsset->Handle] = Metadata;
		#endif
		}

		// Rubber
		{
		#if 0 
			TObjectPtr<LMaterial> RubberMaterial = MaterialLibrary.GetMaterial("Basic_Rubber");
			RubberMaterial->SetTexture(TextureLibrary.GetWhiteTexture());

			TObjectPtr<LMaterialAsset> RubberMaterialAsset = TObjectPtr<LMaterialAsset>::Create(RubberMaterial);
			MemoryAssets[RubberMaterialAsset->Handle] = RubberMaterialAsset;

			FAssetMetadata Metadata;
			Metadata.Handle = RubberMaterialAsset->Handle;
			Metadata.Type = EAssetType::Material;
			AssetRegistry[RubberMaterialAsset->Handle] = Metadata;
		#endif
		}
	}

    void LRuntimeAssetManager::LoadPrimitiveShapes()
    {
		LK_CORE_INFO_TAG("RuntimeAssetManager", "Creating primitive shapes");

		if (TObjectPtr<LScene> CurrentScene = LScene::GetActiveScene())
		{
			/* Create cube. */
			std::filesystem::path FilePath("Assets/Meshes/Cube.gltf");
			LK_CORE_DEBUG_TAG("RuntimeAssetManager", "Importing cube from: '{}'", FilePath.string());
			LEntity CubeEntity = CurrentScene->CreateEntity("Cube");
			TObjectPtr<LMesh> Cube = ImportAsset<LMesh>(FilePath.string());
			LMeshComponent& MeshComponent = CubeEntity.AddComponent<LMeshComponent>(Cube->Handle);

			/* Setup the vertexbuffer layout. */
			TObjectPtr<LMeshSource> CubeSource = Cube->GetMeshSource();
			CubeSource->m_VertexBuffer->SetLayout({
			    { "a_Position",   EShaderDataType::Float3 },
			    { "a_Normal",     EShaderDataType::Float3 },
			    { "a_Binormal",   EShaderDataType::Float3 },
			    { "a_Tangent",    EShaderDataType::Float3 },
			    { "a_TexCoord",   EShaderDataType::Float2 },
			});
			CubeSource->m_VertexBuffer->SetIndexBuffer(CubeSource->m_IndexBuffer);

			/* Cube metadata in the asset registry. */
			FAssetMetadata CubeMetadata;
			CubeMetadata.Handle = Cube->Handle;
			CubeMetadata.Type = EAssetType::Mesh;
			CubeMetadata.FilePath = FilePath;
			CubeMetadata.IsDataLoaded = true;
			AssetRegistry[Cube->Handle] = CubeMetadata;

			/* Assign the Cube with the 'BaseMaterial' */
			LTextureLibrary& TextureLibrary = LTextureLibrary::Get();

		#if 0
			TObjectPtr<LMaterialAsset> BaseMaterialAsset = LMaterialLibrary::Get().GetMaterial(BASE_MATERIAL);
			TObjectPtr<LMaterial> BaseMaterial = BaseMaterialAsset->As<LMaterial>();
		#else
			TObjectPtr<LMaterialAsset> BaseMaterialAsset = LMaterialLibrary::Get().GetMaterial(BASE_MATERIAL);
			TObjectPtr<LMaterial> BaseMaterial = BaseMaterialAsset->GetMaterial();
		#endif
			//TObjectPtr<LMaterial> BasMatRef = BaseMaterial;
			LK_CORE_VERIFY(BaseMaterial, "BaseMaterial is not valid");

			/* TODO: Fix the retrieval of the texture here, should not be a raw string. */
            TObjectPtr<LTexture2D> WoodTexture = TextureLibrary.GetTexture("wood-container_512x512");
			LK_CORE_VERIFY(WoodTexture);
			BaseMaterial->SetTexture(WoodTexture.As<LTexture>());

			//TObjectPtr<LMaterialAsset> BaseMaterialAsset = TObjectPtr<LMaterialAsset>::Create(BaseMaterial);

			Cube->Materials->SetMaterial(0, BaseMaterialAsset->Handle);
			LK_CORE_VERIFY(BaseMaterial->GetTexture(""), "BaseMaterial texture is not valid");

			/* Base material metadata in the asset registry. */
			FAssetMetadata BaseMaterialAssetMetadata;
			BaseMaterialAssetMetadata.Handle = BaseMaterialAsset->Handle;
			BaseMaterialAssetMetadata.Type = EAssetType::Material;
			BaseMaterialAssetMetadata.IsDataLoaded = true;
			AssetRegistry[BaseMaterialAsset->Handle] = BaseMaterialAssetMetadata;

			MemoryAssets[Cube->Handle] = Cube;
			MemoryAssets[BaseMaterialAsset->Handle] = BaseMaterialAsset;

			DebugCube = Cube;

			LK_CORE_DEBUG_TAG("RuntimeAssetManager", "Cube created with handle {}", Cube->Handle);
		}
		else
		{
			LK_CORE_WARN_TAG("RuntimeAssetManager", "No active scene");
			LK_CORE_ASSERT(false);
		}
			
    }


}
