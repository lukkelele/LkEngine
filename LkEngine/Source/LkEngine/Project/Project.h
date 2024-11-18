/******************************************************************
 * LProject
 * 
 *******************************************************************/
#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Serialization/FileStream.h"

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
		LProject();
		~LProject() = default;

		/**
		 * @brief Load a project.
		 */
		void Load(const std::string& ProjectPath);

		/**
		 * @brief Get currently active project.
		 */
		FORCEINLINE static TObjectPtr<LProject> Current() { return ActiveProject; }

		bool Save();
		uint64_t GetSize() const;

		FORCEINLINE const std::string& GetName() const { return Configuration.Name; }

		FORCEINLINE void SetName(const std::string& InName)
		{ 
			Configuration.Name = InName;
		}

		FORCEINLINE FProjectConfiguration& GetConfiguration() { return Configuration; }
		FORCEINLINE const FProjectConfiguration& GetConfiguration() const { return Configuration; }

		FORCEINLINE static TObjectPtr<LRuntimeAssetManager> GetRuntimeAssetManager() 
		{ 
			LK_CORE_VERIFY(RuntimeAssetManager, "Invalid RuntimeAssetManager reference");
			return RuntimeAssetManager; 
		}

		static void SetActive(TObjectPtr<LProject> Project);

		FORCEINLINE static bool IsActiveProject(const TObjectPtr<LProject>& Project) 
		{ 
			return (ActiveProject == Project);
		}

	public:
		inline static constexpr const char* FILE_EXTENSION = "lkproject";
	private:
		FProjectConfiguration Configuration{};

		inline static TObjectPtr<LProject> ActiveProject{};
		inline static TObjectPtr<LRuntimeAssetManager> RuntimeAssetManager{};

		friend class LEditorLayer;

		LCLASS(LProject)
	};

}
