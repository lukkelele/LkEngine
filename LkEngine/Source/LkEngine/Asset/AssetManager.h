#pragma once

#include "RuntimeAssetManager.h"

#include "LkEngine/Project/Project.h"


namespace LkEngine {

	class LAssetManager
	{
	public:
		static void Initialize();
		static void Destroy();

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
			LK_CORE_ASSERT(LProject::GetRuntimeAssetManager()->DebugCube);
			return LProject::GetRuntimeAssetManager()->DebugCube;
		}

		FORCEINLINE static FAssetHandle GetDebugCubeHandle()
		{
			LK_CORE_ASSERT(LProject::GetRuntimeAssetManager()->DebugCube);
			return LProject::GetRuntimeAssetManager()->DebugCube->Handle;
		}

		template<typename T>
		static TObjectPtr<T> GetAsset(FAssetHandle AssetHandle)
		{
			TObjectPtr<LAsset> Asset = LProject::GetRuntimeAssetManager()->GetAsset(AssetHandle);
			return Asset.As<T>();
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
		static FAssetHandle CreateMemoryOnlyAsset(TArgs&&... Args)
		{
			static_assert(std::is_base_of_v<LAsset, TAsset>, "CreateMemoryOnlyAsset only works for types derived from Asset");

			TObjectPtr<TAsset> Asset = TObjectPtr<TAsset>::Create(std::forward<TArgs>(Args)...);
			Asset->Handle = FAssetHandle(); 

			LProject::GetRuntimeAssetManager()->AddMemoryOnlyAsset(Asset);
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
		static FAssetHandle CreateMemoryOnlyAssetWithHandle(FAssetHandle handle, TArgs&&... Args)
		{
			static_assert(std::is_base_of_v<LAsset, TAsset>, "CreateMemoryOnlyAsset only works for types derived from Asset");

			TObjectPtr<TAsset> Asset = TObjectPtr<TAsset>::Create(std::forward<TArgs>(Args)...);
			Asset->Handle = handle;

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
