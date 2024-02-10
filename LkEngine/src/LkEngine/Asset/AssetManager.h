#pragma once

#include "RuntimeAssetManager.h"

#include "LkEngine/Project/Project.h"


namespace LkEngine {

	class AssetManager : public RefCounted
	{
	public:
		static bool IsAssetHandleValid(AssetHandle assetHandle) { return Project::GetRuntimeAssetManager()->IsAssetHandleValid(assetHandle); }
		static AssetType GetAssetType(AssetHandle assetHandle) { return Project::GetRuntimeAssetManager()->GetAssetType(assetHandle); }
		static bool ReloadData(AssetHandle assetHandle) { return Project::GetRuntimeAssetManager()->ReloadData(assetHandle); }

		template<typename T>
		static Ref<T> GetAsset(AssetHandle assetHandle)
		{
			Ref<Asset> asset = Project::GetRuntimeAssetManager()->GetAsset(assetHandle);
			return asset.As<T>();
		}

		template<typename T>
		static std::unordered_set<AssetHandle> GetAllAssetsWithType()
		{
			return Project::GetAssetManager()->GetAllAssetsWithType(T::GetStaticType());
		}

		static const std::unordered_map<AssetHandle, Ref<Asset>>& GetLoadedAssets() 
		{ 
			return Project::GetRuntimeAssetManager()->GetLoadedAssets(); 
		}

		static const std::unordered_map<AssetHandle, Ref<Asset>>& GetMemoryOnlyAssets() 
		{ 
			return Project::GetRuntimeAssetManager()->GetMemoryOnlyAssets(); 
		}

		static AssetHandle GetAssetHandleFromFilePath(const std::filesystem::path& filepath)
		{
			return Project::GetRuntimeAssetManager()->GetAssetHandleFromFilePath(filepath);
		}

		template<typename TAsset, typename... TArgs>
		static AssetHandle CreateMemoryOnlyAsset(TArgs&&... args)
		{
			static_assert(std::is_base_of<Asset, TAsset>::value, "CreateMemoryOnlyAsset only works for types derived from Asset");

			Ref<TAsset> asset = Ref<TAsset>::Create(std::forward<TArgs>(args)...);
			asset->Handle = AssetHandle(); 

			Project::GetRuntimeAssetManager()->AddMemoryOnlyAsset(asset);
			return asset->Handle;
		}

		template<typename TAsset, typename... TArgs>
		static AssetHandle CreateMemoryOnlyRendererAsset(TArgs&&... args)
		{
			static_assert(std::is_base_of<Asset, TAsset>::value, "CreateMemoryOnlyAsset only works for types derived from Asset");

			Ref<TAsset> asset = TAsset::Create(std::forward<TArgs>(args)...);
			asset->Handle = AssetHandle();

			Project::GetAssetManager()->AddMemoryOnlyAsset(asset);
			return asset->Handle;
		}

		template<typename TAsset, typename... TArgs>
		static AssetHandle CreateMemoryOnlyAssetWithHandle(AssetHandle handle, TArgs&&... args)
		{
			static_assert(std::is_base_of<Asset, TAsset>::value, "CreateMemoryOnlyAsset only works for types derived from Asset");

			Ref<TAsset> asset = Ref<TAsset>::Create(std::forward<TArgs>(args)...);
			asset->Handle = handle;

			Project::GetAssetManager()->AddMemoryOnlyAsset(asset);
			return asset->Handle;
		}

		template<typename TAsset>
		static AssetHandle AddMemoryOnlyAsset(Ref<TAsset> asset)
		{
			static_assert(std::is_base_of<Asset, TAsset>::value, "AddMemoryOnlyAsset only works for types derived from Asset");
			asset->Handle = AssetHandle(); // NOTE(Yan): should handle generation happen here?

			Project::GetAssetManager()->AddMemoryOnlyAsset(asset);
			return asset->Handle;
		}

		static bool IsMemoryAsset(AssetHandle handle) { return Project::GetRuntimeAssetManager()->IsMemoryAsset(handle); }

		static void RemoveAsset(AssetHandle handle)
		{
			//Project::GetRuntimeAssetManager()->RemoveAsset(handle);
		}

	};

}