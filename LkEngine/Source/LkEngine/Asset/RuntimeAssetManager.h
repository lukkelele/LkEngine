#pragma once

#include "LkEngine/Asset/IAssetManager.h"
#include "LkEngine/Asset/AssetImporter.h"

namespace LkEngine {

    class LRuntimeAssetManager : public IAssetManager
    {
    public:
		LRuntimeAssetManager();
		~LRuntimeAssetManager();

		virtual void Initialize() override;
		virtual void Destroy() override;

		template<typename AssetType>
		TObjectPtr<AssetType> ImportAsset(const std::filesystem::path& Filepath);

		virtual FAssetHandle ImportAsset(const std::filesystem::path& Filepath) override;
		virtual void RemoveAsset(const FAssetHandle Handle) override;

        virtual TObjectPtr<LAsset> GetAsset(const FAssetHandle Handle) override;
		virtual EAssetType GetAssetType(const FAssetHandle Handle) override;

		virtual bool ReloadData(const FAssetHandle Handle) override;
		virtual void AddMemoryOnlyAsset(TObjectPtr<LAsset> Asset) override;
		virtual bool IsAssetHandleValid(const FAssetHandle Handle) const override;
		virtual bool IsAssetLoaded(const FAssetHandle Handle) const override;

        virtual bool IsMemoryAsset(const FAssetHandle Handle) const override
		{ 
			return MemoryAssets.contains(Handle); 
		}

		virtual bool LoadAssetRegistry() override;
		virtual void WriteRegistryToDisk() override;

        virtual const FAssetMetadata& GetMetadata(const FAssetHandle Handle) const override;
		virtual const FAssetMetadata& GetMetadata(const TObjectPtr<LAsset>& Asset) const override;
        virtual const FAssetMetadata& GetMetadata(const std::filesystem::path& InFilePath) const override;

        virtual FAssetHandle GetAssetHandleFromFilePath(const std::filesystem::path& InFilePath) const override;
		virtual EAssetType GetAssetTypeFromExtension(const std::string& Extension) const override;
		virtual EAssetType GetAssetTypeFromPath(const std::filesystem::path& InFilePath) const override;

		virtual std::unordered_set<FAssetHandle> GetAllAssetsWithType(const EAssetType AssetType) override;
		virtual std::size_t GetAllAssetsWithType(const EAssetType AssetType, std::unordered_set<FAssetHandle>& AssetsOfType) override;

		std::filesystem::path GetFileSystemPath(const FAssetMetadata& Metadata) const;
		std::filesystem::path GetFileSystemPath(const FAssetHandle Handle) const;
        std::filesystem::path GetRelativePath(const std::filesystem::path& InFilePath) const;

		template<typename T>
		TObjectPtr<T> GetAsset(const std::string& InFilePath)
		{
			TObjectPtr<LAsset> Asset = GetAsset(GetAssetHandleFromFilePath(InFilePath));
			if (Asset)
			{
				return Asset->As<T>();
			}

			LK_CORE_ERROR_TAG("RuntimeAssetManager", "Failed to find asset with path: {}", InFilePath);
			return nullptr;
		}

		template<typename T, typename... TArgs>
		TObjectPtr<T> CreateNewAsset(const std::string& FileName, const std::string& DirectoryPath, TArgs&&... Args)
		{
			static_assert(std::is_base_of_v<LAsset, T>, "CreateNewAsset only works for assets derived from LAsset");
			FAssetMetadata Metadata{};
			Metadata.Handle = FAssetHandle();

			if (DirectoryPath.empty() || DirectoryPath == ".")
			{
				Metadata.FilePath = FileName;
			}
			else
			{
				Metadata.FilePath = GetRelativePath(DirectoryPath + "/" + FileName);
			}

			Metadata.bIsDataLoaded = true;
			Metadata.Type = T::GetStaticType();

			AssetRegistry[Metadata.Handle] = Metadata;
			WriteRegistryToDisk();

			TObjectPtr<T> Asset = TObjectPtr<T>::Create(std::forward<TArgs>(Args)...);
			Asset->Handle = Metadata.Handle;
			LoadedAssets[Asset->Handle] = Asset;
			LAssetImporter::Serialize(Metadata, Asset);

			return Asset;
		}

		virtual const std::unordered_map<FAssetHandle, TObjectPtr<LAsset>>& GetLoadedAssets() const override
		{ 
			return LoadedAssets;
		}

		virtual const std::unordered_map<FAssetHandle, TObjectPtr<LAsset>>& GetMemoryOnlyAssets() const override
		{ 
			return MemoryAssets;
		}

		int GetTextures2D(std::vector<TObjectPtr<LTexture2D>>& TextureContainer);
		int GetTextures2D(std::vector<std::pair<std::string, TObjectPtr<LTexture2D>>>& TextureContainer);
		const std::vector<std::pair<std::string, TObjectPtr<LTexture2D>>>& GetTextures2D() const;

		FORCEINLINE const LAssetRegistry& GetAssetRegistry() const { return AssetRegistry; }

	private:
        virtual FAssetMetadata& GetMetadataInternal(const FAssetHandle Handle) override;

		void LoadBaseMaterials();
		void LoadPrimitiveShapes();

    private:
        LAssetRegistry AssetRegistry;
		bool bAssetRegistryValid = false;

		std::unordered_map<FAssetHandle, TObjectPtr<LAsset>> LoadedAssets{};
		std::unordered_map<FAssetHandle, TObjectPtr<LAsset>> MemoryAssets{};

		std::unordered_map<std::string, TObjectPtr<LTexture2D>> Texture2DMap{};
    };

}
