#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/LObjectPtr.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/ImGui/ImGuiLayer.h"

#include "LkEngine/Serialization/FileStream.h"

#include "LkEngine/Asset/RuntimeAssetManager.h"


namespace LkEngine {

	class LProject : public LObject
	{
	public:
		LProject() = default;
		LProject(std::string_view InProjectName);
		~LProject() = default;

		static TObjectPtr<LProject> Current() { return ActiveProject; }

		bool Save();
		uint64_t GetSize() const;

		std::filesystem::path GetFilepath() const { return m_Filepath; }
		const std::string& GetName() const { return m_Name; }

		FORCEINLINE TObjectPtr<LScene> GetScene() 
		{ 
			return Scene; 
		}

		static void SetActive(TObjectPtr<LProject> project);

		static TObjectPtr<LProject> CreateEmptyProject(std::string_view projectName, const bool bSetAsActive = true);
		static TObjectPtr<LProject> CreateDefaultProject(std::string_view projectName, const bool bSetAsActive = true);
		static TObjectPtr<LProject> CreateDebugProject(const bool bSetAsActive = true);

		static TObjectPtr<LRuntimeAssetManager> GetRuntimeAssetManager() 
		{ 
			return RuntimeAssetManager; 
		}

		static bool IsActiveProject(const TObjectPtr<LProject>& project) 
		{ 
			return (ActiveProject == project);
		}

		struct FProjectData
		{
		};

	public:
		FProjectData ProjectData;
		inline static std::string_view DefaultExtension = ".lkproj";
	private:
		std::string m_Name;
		std::filesystem::path m_Filepath;

		TObjectPtr<LScene> Scene{};

		inline static TObjectPtr<LProject> ActiveProject{};
		inline static TObjectPtr<LRuntimeAssetManager> RuntimeAssetManager{};

		friend class LEditorLayer;

		LCLASS(LProject)
	};

}