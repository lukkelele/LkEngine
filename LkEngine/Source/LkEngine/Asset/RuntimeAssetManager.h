#pragma once

#include "AssetManagerBase.h"

#include "LkEngine/Renderer/TextureLibrary.h"


namespace LkEngine {

    class LRuntimeAssetManager : public LAssetManagerBase
    {
    public:
		LRuntimeAssetManager();
		~LRuntimeAssetManager() = default;

		void Initialize(const EInitFlag AssetInitialization = EInitFlag::True);

		template<typename T>
		TObjectPtr<T> ImportAsset(std::filesystem::path filepath);

        TObjectPtr<LAsset> GetAsset(FAssetHandle asset);

		EAssetType GetAssetType(FAssetHandle assetHandle);
        bool IsMemoryAsset(FAssetHandle handle) { return m_MemoryAssets.contains(handle); }

		bool ReloadData(FAssetHandle assetHandle);
		void AddMemoryOnlyAsset(TObjectPtr<LAsset> asset);
		bool IsAssetHandleValid(FAssetHandle assetHandle);

        const FAssetMetadata& GetMetadata(FAssetHandle handle);
        const FAssetMetadata& GetMetadata(const TObjectPtr<LAsset>& asset);
        const FAssetMetadata& GetMetadata(const std::filesystem::path& filepath);
        FAssetMetadata& GetMetadataInternal(FAssetHandle handle);

        FAssetHandle GetAssetHandleFromFilePath(const std::filesystem::path& filepath);
		EAssetType GetAssetTypeFromExtension(const std::string& extension);
		EAssetType GetAssetTypeFromPath(const std::filesystem::path& path);

        std::filesystem::path GetRelativePath(const std::filesystem::path& filepath);
		void WriteRegistryToFile();

		template<typename T>
		TObjectPtr<T> GetAsset(const std::string& filepath)
		{
			/* TODO: Static validity check for retrieved asset cast to T. */
			TObjectPtr<LAsset> Asset = GetAsset(GetAssetHandleFromFilePath(filepath));
			return Asset.As<T>();
		}

		template<typename T, typename... TArgs>
		TObjectPtr<T> CreateNewAsset(const std::string& filename, 
									 const std::string& directoryPath, 
									 TArgs&&... args)
		{
			static_assert(std::is_base_of_v<LAsset, T>, "CreateNewAsset only works for assets derived from LAsset");

			FAssetMetadata metadata;
			metadata.Handle = FAssetHandle();

			if (directoryPath.empty() || directoryPath == ".")
			{
				metadata.FilePath = filename;
			}
			else
			{
				metadata.FilePath = GetRelativePath(directoryPath + "/" + filename);
			}

			metadata.IsDataLoaded = true;
			metadata.Type = T::GetStaticType();

			AssetRegistry[metadata.Handle] = metadata;

			TObjectPtr<T> asset = TObjectPtr<T>::Create(std::forward<TArgs>(args)...);
			asset->Handle = metadata.Handle;
			m_LoadedAssets[asset->Handle] = asset;

			return asset;
		}

		const std::unordered_map<FAssetHandle, TObjectPtr<LAsset>>& GetLoadedAssets() 
		{ 
			return m_LoadedAssets;
		}

		const std::unordered_map<FAssetHandle, TObjectPtr<LAsset>>& GetMemoryOnlyAssets() 
		{ 
			return m_MemoryAssets;
		}

		/**
		 * GetTextures2D
		 */
		const std::vector<TTexture2DPair>& GetTextures2D() const;
		int GetTextures2D(std::vector<TTexture2DPair>& TextureContainer);

	private:
		void LoadMaterials();
		void LoadPrimitiveShapes();

    private:
        std::unordered_map<FAssetHandle, TObjectPtr<LAsset>> m_LoadedAssets;
		std::unordered_map<FAssetHandle, TObjectPtr<LAsset>> m_MemoryAssets;

        LAssetRegistry AssetRegistry;

		LTextureLibrary& TextureLibrary;
    };

}