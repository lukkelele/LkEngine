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
	TObjectPtr<T> LRuntimeAssetManager::ImportAsset(std::filesystem::path AssetFilepath)
	{
		LK_UNUSED(AssetFilepath);
	}
	
	template<>
    TObjectPtr<Mesh> LRuntimeAssetManager::ImportAsset<Mesh>(std::filesystem::path filepath)
    {
        AssimpMeshImporter meshImporter = AssimpMeshImporter(filepath);

        TObjectPtr<MeshSource> source = meshImporter.ImportToMeshSource();
		TObjectPtr<Mesh> mesh = TObjectPtr<Mesh>::Create(source);

		m_LoadedAssets[mesh->Handle] = mesh;
		LK_CORE_TRACE_TAG("RuntimeAssetManager", "Loaded mesh with {} vertices, {} indices and a handle of '{}'", 
						  source->GetVertices().size(), source->GetIndices().size(), mesh->Handle);

		return m_LoadedAssets[mesh->Handle];
    }

	TObjectPtr<LAsset> LRuntimeAssetManager::GetAsset(FAssetHandle AssetHandle)
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

    FAssetHandle LRuntimeAssetManager::GetAssetHandleFromFilePath(const std::filesystem::path& filepath)
	{
		return GetMetadata(filepath).Handle;
	}

    const FAssetMetadata& LRuntimeAssetManager::GetMetadata(const TObjectPtr<LAsset>& asset)
	{
		return GetMetadata(asset->Handle);
	}

	const FAssetMetadata& LRuntimeAssetManager::GetMetadata(const std::filesystem::path& filepath)
	{
		LK_CORE_ASSERT(AssetRegistry.Count() > 0, "AssetRegistry is empty!");

		const std::filesystem::path RelativePath = GetRelativePath(filepath);
		for (auto& [handle, metadata] : AssetRegistry)
		{
			if (metadata.FilePath == RelativePath)
			{
				return metadata;
			}
		}

		return NullMetadata;
	}

	std::filesystem::path LRuntimeAssetManager::GetRelativePath(const std::filesystem::path& filepath)
	{
		/* Return rvalue so we trigger move semantics for performance. */
		return filepath.lexically_normal();
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
		// Cube
		{
			std::filesystem::path filepath("Assets/Meshes/Cube.gltf");
			LEntity cubeEntity = LProject::Current()->GetScene()->CreateEntity("Cube");
			TObjectPtr<Mesh> cube = ImportAsset<Mesh>(filepath.string());
			LMeshComponent& MeshComponent = cubeEntity.AddComponent<LMeshComponent>(cube->Handle);

			LTextureLibrary& TextureLibrary = LTextureLibrary::Get();
			// Assign the cube with the 'BaseMaterial'
			TObjectPtr<LMaterial> baseMaterial = MaterialLibrary.GetBaseMaterial();
            baseMaterial->SetTexture(TextureLibrary.GetTexture("wood-container_512x512"));
			TObjectPtr<LMaterialAsset> baseMatAsset = TObjectPtr<LMaterialAsset>::Create(baseMaterial);

			cube->Materials->SetMaterial(0, baseMatAsset->Handle);
			LK_CORE_ASSERT(baseMaterial->GetTexture(""), "BaseMaterial texture is nullptr");

			FAssetMetadata cubeMetadata;
			cubeMetadata.Handle = cube->Handle;
			cubeMetadata.Type = EAssetType::Mesh;
			cubeMetadata.FilePath = filepath;
			cubeMetadata.IsDataLoaded = true;
			AssetRegistry[cube->Handle] = cubeMetadata;

			FAssetMetadata baseMaterialFAssetMetadata;
			baseMaterialFAssetMetadata.Handle = baseMatAsset->Handle;
			baseMaterialFAssetMetadata.Type = EAssetType::Material;
			baseMaterialFAssetMetadata.IsDataLoaded = true;
			AssetRegistry[baseMatAsset->Handle] = baseMaterialFAssetMetadata;

			TObjectPtr<MeshSource> cubeSource = cube->GetMeshSource();
			cubeSource->m_VertexBuffer->SetLayout({
			    { "a_Position",      ShaderDataType::Float3  },
			    { "a_Normal",        ShaderDataType::Float3  },
			    { "a_Binormal",      ShaderDataType::Float3  },
			    { "a_Tangent",       ShaderDataType::Float3  },
			    { "a_Texcoord",      ShaderDataType::Float2  },
			});
			cubeSource->m_VertexBuffer->SetIndexBuffer(cubeSource->m_IndexBuffer);

			m_MemoryAssets[cube->Handle] = cube;
			m_MemoryAssets[baseMatAsset->Handle] = baseMatAsset;

			// Test out the retrieval of the texture before continuing
			{
				FAssetHandle cubeMaterialHandle = cube->GetMaterials()->GetMaterialHandle(0);
				TObjectPtr<LMaterial> cubeMaterial = LAssetManager::GetAsset<LMaterialAsset>(cube->GetMaterials()->GetMaterialHandle(0))->GetMaterial();
				LK_CORE_VERIFY(cubeMaterial, "Retrieved material for CUBE is nullptr!");
			}

			LK_CORE_DEBUG_TAG("RuntimeAssetManager", "Cube created with handle {}", cube->Handle);
		}
    }


}