#pragma once

#include "AssetManagerBase.h"

#include "LkEngine/Renderer/TextureLibrary.h"


namespace LkEngine {

    class LRuntimeAssetManager : public LAssetManagerBase
    {
    public:
		LRuntimeAssetManager();
		~LRuntimeAssetManager();

		virtual void Initialize() override;
		virtual void Destroy() override;

		template<typename AssetType>
		TObjectPtr<AssetType> ImportAsset(std::filesystem::path filepath);

        TObjectPtr<LAsset> GetAsset(FAssetHandle Asset);
		EAssetType GetAssetType(const FAssetHandle AssetHandle);

        FORCEINLINE bool IsMemoryAsset(const FAssetHandle AssetHandle) const
		{ 
			return MemoryAssets.contains(AssetHandle); 
		}

		bool ReloadData(const FAssetHandle AssetHandle);
		void AddMemoryOnlyAsset(TObjectPtr<LAsset> Asset);
		bool IsAssetHandleValid(const FAssetHandle AssetHandle) const;

		std::filesystem::path GetFileSystemPath(const FAssetMetadata& metadata);
		std::filesystem::path GetFileSystemPath(const FAssetHandle Handle);

		/**
		 * @brief 
		 * @todo: MAKE PRIVATE
		 */
		void LoadAssetRegistry();

		/**
		 * @brief 
		 * @todo: MAKE PRIVATE
		 */
		void WriteRegistryToDisk();

        const FAssetMetadata& GetMetadata(FAssetHandle AssetHandle);
        const FAssetMetadata& GetMetadata(const TObjectPtr<LAsset>& AssetRef);
        const FAssetMetadata& GetMetadata(const std::filesystem::path& InFilePath);
        FAssetMetadata& GetMetadataInternal(const FAssetHandle AssetHandle);

        FAssetHandle GetAssetHandleFromFilePath(const std::filesystem::path& InFilePath);
		EAssetType GetAssetTypeFromExtension(const std::string& Extension);
		EAssetType GetAssetTypeFromPath(const std::filesystem::path& InFilePath);

        std::filesystem::path GetRelativePath(const std::filesystem::path& InFilePath);

		template<typename T>
		TObjectPtr<T> GetAsset(const std::string& InFilePath)
		{
			/* TODO: Static validity check for retrieved Asset cast to T. */
			TObjectPtr<LAsset> Asset = GetAsset(GetAssetHandleFromFilePath(InFilePath));
			if (Asset)
			{
				return Asset->As<T>();
			}

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

			TObjectPtr<T> Asset = TObjectPtr<T>::Create(std::forward<TArgs>(Args)...);
			Asset->Handle = Metadata.Handle;
			m_LoadedAssets[Asset->Handle] = Asset;

			return Asset;
		}

		const std::unordered_map<FAssetHandle, TObjectPtr<LAsset>>& GetLoadedAssets() 
		{ 
			return m_LoadedAssets;
		}

		const std::unordered_map<FAssetHandle, TObjectPtr<LAsset>>& GetMemoryOnlyAssets() 
		{ 
			return MemoryAssets;
		}

		/**
		 * GetTextures2D
		 */
		const std::vector<TTexture2DPair>& GetTextures2D() const;
		int GetTextures2D(std::vector<TTexture2DPair>& TextureContainer);

	public:
		/**
		 * @brief Base cube.
		 */
		TObjectPtr<LMesh> DebugCube{};

	private:
		void LoadMaterials();
		void LoadPrimitiveShapes();

    private:
        std::unordered_map<FAssetHandle, TObjectPtr<LAsset>> m_LoadedAssets;
		std::unordered_map<FAssetHandle, TObjectPtr<LAsset>> MemoryAssets;

        LAssetRegistry AssetRegistry;

		LTextureLibrary& TextureLibrary;
    };

}
