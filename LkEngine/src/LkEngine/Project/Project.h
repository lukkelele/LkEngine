#pragma once

#include "LkEngine/Asset/AssetManager.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/ImGui/ImGuiLayer.h"

#include "LkEngine/Serialization/FileStream.h"


namespace LkEngine {

	struct ProjectSpecification
	{
		std::string Name = "LkProject1";
		bool Blank = false;
	};

	struct ProjectData
	{
		Ref<Scene> TargetScene;
	};

	class Project : public RefCounted
	{
	public:
		Project() = default;
		Project(const ProjectSpecification& specification);
		Project(const std::string& name);
		~Project() = default;

		bool Save();
		uint64_t GetSize() const;

		std::filesystem::path GetFilepath() const { return m_Filepath; }
		const std::string& const GetName() const { return m_Name; }

		const ProjectSpecification& GetSpecification() const { return m_Specification; }

		Ref<Scene> GetTargetScene() const { return Data.TargetScene; }
		// Basic project to be used as a starter project/for when opening the engine 

		static void SetActive(Ref<Project> project);

		static Ref<Project> CreateEmptyProject(bool setActive = true);
		static Ref<Project> CreateDefaultProject(bool setActive = true);
		static Ref<Project> CreateDebugProject(bool setActive = true);

		static Ref<Project> Current() { return m_ActiveProject; }
		static Ref<AssetManager> GetAssetManager() { return m_AssetManager; }

	public:
		ProjectData Data;
		inline static std::string_view DefaultExtension = ".lkproj";
	private:
		std::string m_Name;
		ProjectSpecification m_Specification;

		std::filesystem::path m_Filepath;

		inline static Ref<Project> m_ActiveProject;
		inline static Ref<AssetManager> m_AssetManager;

		friend class EditorLayer;
	};

}