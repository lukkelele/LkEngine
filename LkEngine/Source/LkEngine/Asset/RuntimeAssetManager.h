#pragma once

#include "AssetManagerBase.h"


namespace LkEngine {

    class RuntimeAssetManager : public AssetManagerBase
    {
    public:
        RuntimeAssetManager();
        ~RuntimeAssetManager();

		void Init();

		template<typename T>
		Ref<T> ImportAsset(std::filesystem::path filepath);

        Ref<Asset> GetAsset(AssetHandle asset);
		AssetType GetAssetType(AssetHandle assetHandle);
        bool IsMemoryAsset(AssetHandle handle) { return m_MemoryAssets.contains(handle); }

		bool ReloadData(AssetHandle assetHandle);
		void AddMemoryOnlyAsset(Ref<Asset> asset);
		bool IsAssetHandleValid(AssetHandle assetHandle);

        const AssetMetadata& GetMetadata(AssetHandle handle);
        const AssetMetadata& GetMetadata(const Ref<Asset>& asset);
        const AssetMetadata& GetMetadata(const std::filesystem::path& filepath);
        AssetMetadata& GetMetadataInternal(AssetHandle handle);

        AssetHandle GetAssetHandleFromFilePath(const std::filesystem::path& filepath);
		AssetType GetAssetTypeFromExtension(const std::string& extension);
		AssetType GetAssetTypeFromPath(const std::filesystem::path& path);

        std::filesystem::path GetRelativePath(const std::filesystem::path& filepath);
		void WriteRegistryToFile();

		template<typename T>
		Ref<T> GetAsset(const std::string& filepath)
		{
			Ref<Asset> asset = GetAsset(GetAssetHandleFromFilePath(filepath));
			return asset.As<T>();
		}

		template<typename T, typename... Args>
		Ref<T> CreateNewAsset(const std::string& filename, const std::string& directoryPath, Args&&... args)
		{
			static_assert(std::is_base_of<Asset, T>::value, "CreateNewAsset only works for types derived from Asset");

			AssetMetadata metadata;
			metadata.Handle = AssetHandle();
			if (directoryPath.empty() || directoryPath == ".")
				metadata.FilePath = filename;
			else
				metadata.FilePath = GetRelativePath(directoryPath + "/" + filename);
			metadata.IsDataLoaded = true;
			metadata.Type = T::GetStaticType();

			m_AssetRegistry[metadata.Handle] = metadata;

			Ref<T> asset = Ref<T>::Create(std::forward<Args>(args)...);
			asset->Handle = metadata.Handle;
			m_LoadedAssets[asset->Handle] = asset;

			return asset;
		}

		const std::unordered_map<AssetHandle, Ref<Asset>>& GetLoadedAssets() { return m_LoadedAssets; }
		const std::unordered_map<AssetHandle, Ref<Asset>>& GetMemoryOnlyAssets() { return m_MemoryAssets; }

	private:
		void LoadMaterials();
		void LoadPrimitiveShapes();

    private:
        std::unordered_map<AssetHandle, Ref<Asset>> m_LoadedAssets;
		std::unordered_map<AssetHandle, Ref<Asset>> m_MemoryAssets;

        AssetRegistry m_AssetRegistry;
    };

}