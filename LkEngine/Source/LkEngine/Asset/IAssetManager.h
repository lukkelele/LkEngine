#pragma once

#include <unordered_set>

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "Asset.h"
#include "AssetRegistry.h"

#include "AssimpMeshImporter.h"


namespace LkEngine {

	static FAssetMetadata NullMetadata{};

    class IAssetManager : public LObject
    {
    public:
        virtual ~IAssetManager() = default;

		virtual void Initialize() = 0;
		virtual void Destroy() = 0;

		virtual FAssetHandle ImportAsset(const std::filesystem::path& Filepath) = 0;

		virtual TObjectPtr<LAsset> GetAsset(const FAssetHandle Handle) = 0;
		virtual EAssetType GetAssetType(const FAssetHandle Handle) = 0;

		virtual bool ReloadData(const FAssetHandle Handle) = 0;
		virtual void RemoveAsset(const FAssetHandle Handle) = 0;
		virtual void AddMemoryOnlyAsset(TObjectPtr<LAsset> Asset) = 0;

		virtual bool IsAssetLoaded(const FAssetHandle Handle) const = 0;
		virtual bool IsAssetHandleValid(const FAssetHandle Handle) const = 0;
		virtual bool IsMemoryAsset(const FAssetHandle Handle) const = 0;

		virtual const std::unordered_map<FAssetHandle, TObjectPtr<LAsset>>& GetLoadedAssets() const = 0;
		virtual const std::unordered_map<FAssetHandle, TObjectPtr<LAsset>>& GetMemoryOnlyAssets() const = 0;

		virtual std::unordered_set<FAssetHandle> GetAllAssetsWithType(const EAssetType AssetType) = 0;
		virtual std::size_t GetAllAssetsWithType(const EAssetType AssetType, 
												 std::unordered_set<FAssetHandle>& AssetsOfType) = 0;

        virtual FAssetHandle GetAssetHandleFromFilePath(const std::filesystem::path& InFilePath) const = 0;
		virtual EAssetType GetAssetTypeFromExtension(const std::string& Extension) const = 0;
		virtual EAssetType GetAssetTypeFromPath(const std::filesystem::path& InFilePath) const = 0;

		virtual const FAssetMetadata& GetMetadata(const FAssetHandle Handle) const = 0;
		virtual const FAssetMetadata& GetMetadata(const TObjectPtr<LAsset>& Asset) const = 0;
        virtual const FAssetMetadata& GetMetadata(const std::filesystem::path& InFilePath) const = 0;

		virtual bool LoadAssetRegistry() = 0;
		virtual void WriteRegistryToDisk() = 0;

	protected:
        virtual FAssetMetadata& GetMetadataInternal(const FAssetHandle Handle) = 0;

    private:
        LCLASS(IAssetManager)
    };

}
