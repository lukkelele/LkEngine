#pragma once

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/ImGui/ImGuiLayer.h"

#include "LkEngine/Serialization/FileStream.h"

#include "LkEngine/Asset/RuntimeAssetManager.h"


namespace LkEngine {

	class Project : public RefCounted
	{
	public:
		Project() = default;
		Project(const std::string& name);
		~Project() = default;

		static Ref<Project> Current() { return s_ActiveProject; }

		bool Save();
		uint64_t GetSize() const;

		std::filesystem::path GetFilepath() const { return m_Filepath; }
		const std::string& const GetName() const { return m_Name; }
		Ref<Scene> GetScene() { return m_Scene; }

		static void SetActive(Ref<Project> project);

		static Ref<Project> CreateEmptyProject(std::string_view projectName, bool setActive = true);
		static Ref<Project> CreateDefaultProject(std::string_view projectName, bool setActive = true);
		static Ref<Project> CreateDebugProject(bool setActive = true);

		static Ref<RuntimeAssetManager> GetRuntimeAssetManager() { return s_RuntimeAssetManager; }
		static bool IsActiveProject(const Ref<Project>& project) { return s_ActiveProject == project; }

		struct ProjectData
		{

		};

	public:
		ProjectData Data;
		inline static std::string_view DefaultExtension = ".lkproj";
	private:
		std::string m_Name;
		std::filesystem::path m_Filepath;

		Ref<Scene> m_Scene;

		inline static Ref<Project> s_ActiveProject;
		inline static Ref<RuntimeAssetManager> s_RuntimeAssetManager;

		friend class EditorLayer;
	};

}