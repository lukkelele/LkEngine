#pragma once

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/ImGui/ImGuiLayer.h"


namespace LkEngine {

	struct ProjectSpecification
	{
		std::string Name = "LukkeleleProject1";
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

		static Ref<Project> CreateEmptyProject();
		static Ref<Project> CreateDefaultProject();
		static Ref<Project> CreateDebugProject();

	public:
		ProjectData Data;
		inline static std::string_view DefaultExtension = ".lkproj";
	private:
		std::string m_Name;
		ProjectSpecification m_Specification;

		std::filesystem::path m_Filepath;

		friend class EditorLayer;
	};

}