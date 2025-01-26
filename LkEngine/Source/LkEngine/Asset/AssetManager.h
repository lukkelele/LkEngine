#pragma once

#define LK_DEBUG_ASSET_LOG 0

#include "EditorAssetManager.h"
#include "RuntimeAssetManager.h"

#include "LkEngine/Project/Project.h"


namespace LkEngine {

	class LAssetManager
	{
	public:
		static void Initialize();
		static void Destroy();

		FORCEINLINE static bool IsAssetHandleValid(const FAssetHandle Handle) 
		{ 
			return LProject::GetAssetManager()->IsAssetHandleValid(Handle); 
		}

		FORCEINLINE static EAssetType GetAssetType(const FAssetHandle Handle) 
		{ 
			return LProject::GetAssetManager()->GetAssetType(Handle); 
		}

		FORCEINLINE static bool ReloadData(const FAssetHandle Handle) 
		{ 
			return LProject::GetAssetManager()->ReloadData(Handle); 
		}

		template<typename T>
		FORCEINLINE static TObjectPtr<T> GetAsset(const FAssetHandle Handle)
		{
			TObjectPtr<LAsset> Asset = LProject::GetAssetManager()->GetAsset(Handle);
			return Asset.As<T>();
		}

		template<typename T>
		static std::unordered_set<FAssetHandle> GetAllAssetsWithType()
		{
			return LProject::GetAssetManager()->GetAllAssetsWithType(T::GetStaticType());
		}

		FORCEINLINE static const std::unordered_map<FAssetHandle, TObjectPtr<LAsset>>& GetLoadedAssets() 
		{ 
			return LProject::GetAssetManager()->GetLoadedAssets(); 
		}

		FORCEINLINE static const std::unordered_map<FAssetHandle , TObjectPtr<LAsset>>& GetMemoryOnlyAssets() 
		{ 
			return LProject::GetAssetManager()->GetMemoryOnlyAssets(); 
		}

		FORCEINLINE static FAssetHandle GetAssetHandleFromFilePath(const std::filesystem::path& InFilepath)
		{
			return LProject::GetAssetManager()->GetAssetHandleFromFilePath(InFilepath);
		}

		template<typename TAsset, typename... TArgs>
		static FAssetHandle CreateMemoryOnlyAsset(TArgs&&... Args)
		{
			static_assert(std::is_base_of_v<LAsset, TAsset>, "CreateMemoryOnlyAsset only works for types derived from Asset");
			TObjectPtr<TAsset> Asset = TObjectPtr<TAsset>::Create(std::forward<TArgs>(Args)...);
			Asset->Handle = FAssetHandle(); 
			LProject::GetAssetManager()->AddMemoryOnlyAsset(Asset);

			return Asset->Handle;
		}

		template<typename TAsset, typename... TArgs>
		static FAssetHandle CreateMemoryOnlyRendererAsset(TArgs&&... Args)
		{
			static_assert(std::is_base_of_v<LAsset, TAsset>, "CreateMemoryOnlyAsset only works for types derived from Asset");
			TObjectPtr<TAsset> Asset = TAsset::Create(std::forward<TArgs>(Args)...);
			Asset->Handle = FAssetHandle();
			LProject::GetAssetManager()->AddMemoryOnlyAsset(Asset);

			return Asset->Handle;
		}

		template<typename TAsset, typename... TArgs>
		static FAssetHandle CreateMemoryOnlyAssetWithHandle(FAssetHandle Handle, TArgs&&... Args)
		{
			static_assert(std::is_base_of_v<LAsset, TAsset>, "CreateMemoryOnlyAsset only works for types derived from Asset");
			TObjectPtr<TAsset> Asset = TObjectPtr<TAsset>::Create(std::forward<TArgs>(Args)...);
			Asset->Handle = Handle;
			LProject::GetAssetManager()->AddMemoryOnlyAsset(Asset);

			return Asset->Handle;
		}

		template<typename TAsset>
		static FAssetHandle AddMemoryOnlyAsset(TObjectPtr<TAsset> Asset)
		{
			static_assert(std::is_base_of_v<LAsset, TAsset>, "AddMemoryOnlyAsset only works for types derived from Asset");
			Asset->Handle = FAssetHandle(); 
			LProject::GetAssetManager()->AddMemoryOnlyAsset(Asset);

			return Asset->Handle;
		}

		FORCEINLINE static bool IsMemoryAsset(const FAssetHandle Handle) 
		{ 
			return LProject::GetAssetManager()->IsMemoryAsset(Handle); 
		}

	};

}
