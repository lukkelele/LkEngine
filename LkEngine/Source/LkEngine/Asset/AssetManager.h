#pragma once

#include "RuntimeAssetManager.h"

#include "LkEngine/Project/Project.h"


namespace LkEngine {

	class LAssetManager
	{
	public:
		FORCEINLINE static bool IsAssetHandleValid(FAssetHandle AssetHandle) 
		{ 
			return LProject::GetRuntimeAssetManager()->IsAssetHandleValid(AssetHandle); 
		}

		FORCEINLINE static EAssetType GetAssetType(FAssetHandle AssetHandle) 
		{ 
			return LProject::GetRuntimeAssetManager()->GetAssetType(AssetHandle); 
		}

		FORCEINLINE static bool ReloadData(FAssetHandle AssetHandle) 
		{ 
			return LProject::GetRuntimeAssetManager()->ReloadData(AssetHandle); 
		}

		FORCEINLINE static int GetTextures2D(std::vector<TTexture2DPair>& TextureContainer)
		{ 
			return LProject::GetRuntimeAssetManager()->GetTextures2D(TextureContainer);
		}

		FORCEINLINE static const std::vector<TTexture2DPair>& GetTextures2D()
		{ 
			return LProject::GetRuntimeAssetManager()->GetTextures2D();
		}

		FORCEINLINE static TObjectPtr<LMesh> GetDebugCube()
		{
			return LProject::GetRuntimeAssetManager()->DebugCube;
		}

		template<typename T>
		static TObjectPtr<T> GetAsset(FAssetHandle AssetHandle)
		{
			TObjectPtr<LAsset> asset = LProject::GetRuntimeAssetManager()->GetAsset(AssetHandle);
			return asset.As<T>();
		}

		template<typename T>
		static std::unordered_set<FAssetHandle> GetAllAssetsWithType()
		{
			/// TODO: Static checks on T
			return LProject::GetAssetManager()->GetAllAssetsWithType(T::GetStaticType());
		}

		static const std::unordered_map<FAssetHandle, TObjectPtr<LAsset>>& GetLoadedAssets() 
		{ 
			return LProject::GetRuntimeAssetManager()->GetLoadedAssets(); 
		}

		static const std::unordered_map<FAssetHandle, TObjectPtr<LAsset>>& GetMemoryOnlyAssets() 
		{ 
			return LProject::GetRuntimeAssetManager()->GetMemoryOnlyAssets(); 
		}

		static FAssetHandle GetAssetHandleFromFilePath(const std::filesystem::path& filepath)
		{
			return LProject::GetRuntimeAssetManager()->GetAssetHandleFromFilePath(filepath);
		}

		template<typename TAsset, typename... TArgs>
		static FAssetHandle CreateMemoryOnlyAsset(TArgs&&... args)
		{
			static_assert(std::is_base_of<LAsset, TAsset>::value, "CreateMemoryOnlyAsset only works for types derived from Asset");

			TObjectPtr<TAsset> asset = TObjectPtr<TAsset>::Create(std::forward<TArgs>(args)...);
			asset->Handle = FAssetHandle(); 

			LProject::GetRuntimeAssetManager()->AddMemoryOnlyAsset(asset);
			return asset->Handle;
		}

		template<typename TAsset, typename... TArgs>
		static FAssetHandle CreateMemoryOnlyRendererAsset(TArgs&&... args)
		{
			static_assert(std::is_base_of<LAsset, TAsset>::value, "CreateMemoryOnlyAsset only works for types derived from Asset");

			TObjectPtr<TAsset> asset = TAsset::Create(std::forward<TArgs>(args)...);
			asset->Handle = FAssetHandle();

			LProject::GetAssetManager()->AddMemoryOnlyAsset(asset);

			return asset->Handle;
		}

		template<typename TAsset, typename... TArgs>
		static FAssetHandle CreateMemoryOnlyAssetWithHandle(FAssetHandle handle, TArgs&&... args)
		{
			static_assert(std::is_base_of_v<LAsset, TAsset>, "CreateMemoryOnlyAsset only works for types derived from Asset");

			TObjectPtr<TAsset> asset = TObjectPtr<TAsset>::Create(std::forward<TArgs>(args)...);
			asset->Handle = handle;

			LProject::GetAssetManager()->AddMemoryOnlyAsset(asset);
			return asset->Handle;
		}

		template<typename TAsset>
		static FAssetHandle AddMemoryOnlyAsset(TObjectPtr<TAsset> asset)
		{
			static_assert(std::is_base_of_v<LAsset, TAsset>, "AddMemoryOnlyAsset only works for types derived from Asset");
			asset->Handle = FAssetHandle(); 

			LProject::GetAssetManager()->AddMemoryOnlyAsset(asset);
			return asset->Handle;
		}

		static bool IsMemoryAsset(FAssetHandle handle) 
		{ 
			return LProject::GetRuntimeAssetManager()->IsMemoryAsset(handle); 
		}

		static void RemoveAsset(FAssetHandle InHandle)
		{
			LK_UNUSED(InHandle);
		}

	};

}