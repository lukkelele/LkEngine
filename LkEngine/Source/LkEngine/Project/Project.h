/******************************************************************
 * LProject
 * 
 *******************************************************************/
#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Serialization/FileStream.h"

#include "LkEngine/Asset/EditorAssetManager.h"
#include "LkEngine/Asset/RuntimeAssetManager.h"


namespace LkEngine {

	struct FProjectConfiguration
	{
		std::string Name{};

		std::string AssetDirectory{};
		std::string AssetRegistryPath{};
		std::string MeshPath{};

		std::string StartScene{};

		bool bAutoSave = true;
		std::chrono::seconds AutoSaveInterval = 150s;

		/* Not serialized. */
		std::string ProjectFileName{};
		std::string ProjectDirectory{};
	};

	class LProject : public LObject
	{
	public:
		LK_DECLARE_MULTICAST_DELEGATE(FOnProjectChanged, TObjectPtr<LProject>);
	public:
		LProject();
		~LProject() = default;

		void Load(const std::string& ProjectPath);

		/** Get currently active project. */
		FORCEINLINE static TObjectPtr<LProject> Current() { return ActiveProject; }

		static std::filesystem::path GetAssetDirectory();
		static std::filesystem::path GetAssetRegistryPath();

		bool Save();
		uint64_t GetSize() const;

		FProjectConfiguration& GetConfiguration() { return Configuration; }
		const FProjectConfiguration& GetConfiguration() const { return Configuration; }

		FORCEINLINE static TObjectPtr<IAssetManager> GetAssetManager()
		{
			LK_CORE_VERIFY(AssetManager);
			return AssetManager;
		}

		FORCEINLINE static TObjectPtr<LEditorAssetManager> GetEditorAssetManager() 
		{ 
			LK_CORE_VERIFY(AssetManager);
			return AssetManager.As<LEditorAssetManager>(); 
		}

		FORCEINLINE static TObjectPtr<LRuntimeAssetManager> GetRuntimeAssetManager() 
		{ 
			LK_CORE_VERIFY(AssetManager);
			return AssetManager.As<LRuntimeAssetManager>(); 
		}

		static void SetActive(TObjectPtr<LProject> Project);

		FORCEINLINE static bool IsActiveProject(const TObjectPtr<LProject>& Project) 
		{ 
			return (ActiveProject == Project);
		}

		void SetName(const std::string& InName) { Configuration.Name = InName; }
		const std::string& GetName() const { return Configuration.Name; }

	public:
		inline static constexpr const char* FILE_EXTENSION = ".lkproject";
		static FOnProjectChanged OnProjectChanged;
	private:
		FProjectConfiguration Configuration{};

		inline static TObjectPtr<LProject> ActiveProject{};
		inline static TObjectPtr<IAssetManager> AssetManager{};

		friend class LEditorLayer;

		LCLASS(LProject)
	};

}
