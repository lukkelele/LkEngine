#pragma once

#include "AssetManagerBase.h"
#include "AssetImporter.h"

#include "LkEngine/Renderer/TextureLibrary.h"


namespace LkEngine {

    class LRuntimeAssetManager : public LAssetManagerBase
    {
    public:
		LRuntimeAssetManager();
		~LRuntimeAssetManager();

		virtual void Initialize() override;
		virtual void Destroy() override;

		LUUID ImportAsset(const std::filesystem::path& Filepath);

		template<typename AssetType>
		TObjectPtr<AssetType> ImportAsset(const std::filesystem::path& Filepath);

        TObjectPtr<LAsset> GetAsset(const FAssetHandle Handle);
		EAssetType GetAssetType(const FAssetHandle Handle);

        FORCEINLINE bool IsMemoryAsset(const FAssetHandle Handle) const
		{ 
			return MemoryAssets.contains(Handle); 
		}

		bool ReloadData(const FAssetHandle Handle);
		void AddMemoryOnlyAsset(TObjectPtr<LAsset> Asset);
		bool IsAssetHandleValid(const FAssetHandle Handle) const;
		bool IsAssetLoaded(const FAssetHandle Handle) const;

		std::filesystem::path GetFileSystemPath(const FAssetMetadata& metadata);
		std::filesystem::path GetFileSystemPath(const FAssetHandle Handle);

		/**
		 * @brief 
		 * @todo: MAKE PRIVATE
		 */
		bool LoadAssetRegistry();

		/**
		 * @brief 
		 * @todo: MAKE PRIVATE
		 */
		void WriteRegistryToDisk();

        const FAssetMetadata& GetMetadata(const FAssetHandle Handle) const;
        const FAssetMetadata& GetMetadata(const TObjectPtr<LAsset>& AssetRef);
        const FAssetMetadata& GetMetadata(const std::filesystem::path& InFilePath);
        FAssetMetadata& GetMetadataInternal(const FAssetHandle Handle);

        LUUID GetAssetHandleFromFilePath(const std::filesystem::path& InFilePath);
		EAssetType GetAssetTypeFromExtension(const std::string& Extension);
		EAssetType GetAssetTypeFromPath(const std::filesystem::path& InFilePath);

        std::filesystem::path GetRelativePath(const std::filesystem::path& InFilePath);

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
			static_assert(std::is_base_of_v<LAsset, T>, "CreateNewAsset only works for Assets derived from LAsset");

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

		const std::unordered_map<LUUID, TObjectPtr<LAsset>>& GetLoadedAssets() 
		{ 
			return LoadedAssets;
		}

		const std::unordered_map<LUUID, TObjectPtr<LAsset>>& GetMemoryOnlyAssets() 
		{ 
			return MemoryAssets;
		}

		/**
		 * GetTextures2D
		 */
		const std::vector<TTexture2DPair>& GetTextures2D() const;
		int GetTextures2D(std::vector<TTexture2DPair>& TextureContainer);

		FORCEINLINE const LAssetRegistry& GetAssetRegistry() const { return AssetRegistry; }

	private:
		void LoadBaseMaterial();
		void LoadPrimitiveShapes();

    private:
		std::unordered_map<LUUID, TObjectPtr<LAsset>> LoadedAssets{};
		std::unordered_map<LUUID, TObjectPtr<LAsset>> MemoryAssets{};

        LAssetRegistry AssetRegistry;
		bool bAssetRegistryValid = false;

		LTextureLibrary& TextureLibrary;
    };

}
