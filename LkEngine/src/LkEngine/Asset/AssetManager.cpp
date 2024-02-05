#include "LKpch.h"
#include "AssetManager.h"

#include "LkEngine/Project/Project.h"
#include "LkEngine/Scene/Entity.h"


namespace LkEngine {

    static AssetMetadata NullMetadata;

    AssetManager::AssetManager()
    {
		LoadPrimitiveShapes();
    }

    AssetManager::~AssetManager()
    {
    }

	template<typename T>
	Ref<T> AssetManager::ImportAsset(std::filesystem::path filepath)
	{
	}
	
	template<>
    Ref<Mesh> AssetManager::ImportAsset<Mesh>(std::filesystem::path filepath)
    {
        AssimpMeshImporter meshImporter = AssimpMeshImporter(filepath);
        Ref<MeshSource> source = meshImporter.ImportToMeshSource();
		Ref<Mesh> mesh = Ref<Mesh>::Create(source);
		m_LoadedAssets[mesh->Handle] = mesh;
		LK_CORE_INFO_TAG("AssetManager", "Loaded mesh with {} vertices, {} indices and a handle of '{}'", source->GetVertices().size(), source->GetIndices().size(), mesh->Handle);

		return m_LoadedAssets[mesh->Handle];
    }

    Ref<Asset> AssetManager::GetAsset(AssetHandle assetHandle)
    {
        if (IsMemoryAsset(assetHandle))
			return m_MemoryAssets[assetHandle];

		auto& metadata = GetMetadataInternal(assetHandle);
		if (!metadata.IsValid())
		{
			LK_CORE_WARN_TAG("AssetManager", "Metadata is invalid for {}", assetHandle);
			return nullptr;
		}

		Ref<Asset> asset = nullptr;
		if (!metadata.IsDataLoaded)
		{
			if (!metadata.IsDataLoaded)
				return nullptr;

			m_LoadedAssets[assetHandle] = asset;
		}
		else
		{
			asset = m_LoadedAssets[assetHandle];
		}

		return asset;
    }

    const AssetMetadata& AssetManager::GetMetadata(AssetHandle handle)
	{
		if (m_AssetRegistry.Contains(handle))
			return m_AssetRegistry[handle];

		return NullMetadata;
	}

    AssetMetadata& AssetManager::GetMetadataInternal(AssetHandle handle)
	{
	    if (m_AssetRegistry.Contains(handle))
	    	return m_AssetRegistry[handle];
	    return NullMetadata; 
	}

    AssetHandle AssetManager::GetAssetHandleFromFilePath(const std::filesystem::path& filepath)
	{
		return GetMetadata(filepath).Handle;
	}

    const AssetMetadata& AssetManager::GetMetadata(const Ref<Asset>& asset)
	{
		return GetMetadata(asset->Handle);
	}

	const AssetMetadata& AssetManager::GetMetadata(const std::filesystem::path& filepath)
	{
		LK_CORE_ASSERT(m_AssetRegistry.Count() > 0, "AssetRegistry is empty!");
		const auto relativePath = GetRelativePath(filepath);
		for (auto& [handle, metadata] : m_AssetRegistry)
		{
			if (metadata.FilePath == relativePath)
				return metadata;
		}
		return NullMetadata;
	}

	std::filesystem::path AssetManager::GetRelativePath(const std::filesystem::path& filepath)
	{
		std::filesystem::path relativePath = filepath.lexically_normal();
		std::string temp = filepath.string();
		LK_CORE_INFO("Returning relative filepath -> {}", relativePath.string());
		return relativePath;
	}

    void AssetManager::LoadPrimitiveShapes()
    {
		LK_CORE_INFO_TAG("AssetManager", "Creating primitive shapes");
		// Cube
		{
			std::filesystem::path filepath("assets/Meshes/Cube.obj");
			Entity cubeEntity = Project::Current()->GetTargetScene()->CreateEntity("Cube");
			Ref<Mesh> cube = ImportAsset<Mesh>(filepath.string());
			m_LoadedAssets[cube->Handle] = cube;

			cubeEntity.AddComponent<MeshComponent>(cube->Handle);

			AssetMetadata cubeMetadata;
			cubeMetadata.Handle = cube->Handle;
			cubeMetadata.Type = AssetType::Mesh;
			cubeMetadata.FilePath = filepath;
			cubeMetadata.IsDataLoaded = true;
			m_AssetRegistry[cube->Handle] = cubeMetadata;

			LK_CORE_ASSERT(cube->GetMeshSource(), "MeshSource is nullptr!");
			LK_CORE_DEBUG_TAG("AssetManager", "Cube created with handle {}", cube->Handle);
		}

    }

	void AssetManager::WriteRegistryToFile()
	{
	}

	AssetType AssetManager::GetAssetTypeFromExtension(const std::string& extension)
	{
		LK_CORE_ASSERT(false, "Not implemented yet!");
		return AssetType::None;
	}

	AssetType AssetManager::GetAssetTypeFromPath(const std::filesystem::path& path)
	{
		LK_CORE_ASSERT(false, "Not implemented yet!");
		return GetAssetTypeFromExtension(path.extension().string());
	}


}