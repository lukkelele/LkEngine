#include "LKpch.h"
#include "Project.h"

#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/SceneSerializer.h"

#include "LkEngine/Core/Window.h"

#include "LkEngine/Renderer/TextureLibrary.h"


namespace LkEngine {

	Project::Project(const ProjectSpecification& specification)
		: m_Specification(specification)
		, m_Name(specification.Name)
	{
		m_AssetManager = Ref<AssetManager>::Create();
	}

	Project::Project(const std::string& name)
		: m_Name(name)
	{
	}

	void Project::SetActive(Ref<Project> project)
	{
		if (m_ActiveProject)
		{
			m_AssetManager = nullptr;
		}

		m_ActiveProject = project;
		if (m_ActiveProject)
		{
			m_AssetManager = Ref<AssetManager>::Create();
		}

	}

	// Save project data, aka serialize to disk
	bool Project::Save()
	{
		LK_CORE_INFO_TAG("Project", "Saving project {}", m_Name);
		if (Data.TargetScene != nullptr)
		{
			SceneSerializer sceneSerializer(Data.TargetScene);
		}

		return true;
	}

	uint64_t Project::GetSize() const
	{
		return 0;
	}

	Ref<Project> Project::CreateEmptyProject(bool setActive)
	{
		Ref<Project> project = Ref<Project>::Create("UntitledProject");
		project->Data.TargetScene = Ref<Scene>::Create("StarterScene", true, true);

		if (setActive)
		{
			m_ActiveProject = project;
		}

		return project;
	}

	// TODO
	Ref<Project> Project::CreateDefaultProject(bool setActive)
	{
		Ref<Project> project = Ref<Project>::Create("UntitledProject");
		project->Data.TargetScene = Ref<Scene>::Create("StarterScene", true, true);

		LK_CORE_DEBUG_TAG("Project", "Creating platform entity");
		Entity basePlatform = project->Data.TargetScene->CreateEntity("Platform");
		auto& sc = basePlatform.AddComponent<SpriteComponent>();
		auto& mc = basePlatform.AddComponent<MaterialComponent>();
		auto& cc = basePlatform.AddComponent<CameraComponent>();
		LK_CORE_DEBUG_TAG("Project", "Created transform, sprite, material and camera components for platform entity");

		sc.SetSize(20, 20);
		sc.Color = Color::RGBA::Green;
		// Rotate platform

		// Center origin
		auto& tc = basePlatform.Transform();
		tc.Translation.x = sc.Size.x * 0.50f;
		tc.Translation.y = sc.Size.y * 0.50f;

		LK_CORE_DEBUG_TAG("Project", "Setting texture2D");
		mc.SetTexture(TextureLibrary::Get()->GetTexture2D("atte_square"));

		LK_CORE_DEBUG_TAG("Project", "Creating scene camera and setting it to perspective");
		cc.Camera = Ref<SceneCamera>::Create();
		cc.Camera->SetPerspective(60.0f, 0.10f, 5000.0f);

		if (setActive)
			m_ActiveProject = project;

		return project;
	}

	// TODO
	Ref<Project> Project::CreateDebugProject(bool setActive)
	{
		Ref<Project> project = Ref<Project>::Create("UntitledProject");
		project->Data.TargetScene = Ref<Scene>::Create("StarterScene", true, true);

		LK_CORE_DEBUG_TAG("Project", "Creating base entity");
		Entity atteEntity = project->Data.TargetScene->CreateEntity("Atte");
		auto& sc = atteEntity.AddComponent<SpriteComponent>();
		auto& mc = atteEntity.AddComponent<MaterialComponent>();
		auto& cc = atteEntity.AddComponent<CameraComponent>();
		LK_CORE_DEBUG_TAG("Project", "Created transform, sprite, material and camera components for base entity");

		sc.SetSize(10, 10);
		sc.Color = Color::RGBA::Green;

		// Center origin
		auto& tc = atteEntity.Transform();
		tc.Translation.x = sc.Size.x * 0.50f;
		tc.Translation.y = sc.Size.y * 0.50f;

		LK_CORE_DEBUG_TAG("Project", "Setting texture2D");
		mc.SetTexture(TextureLibrary::Get()->GetTexture2D("atte_square"));

		LK_CORE_DEBUG_TAG("Project", "Creating scene camera and setting it to perspective");
		cc.Camera = Ref<SceneCamera>::Create();
		cc.Camera->SetPerspective(60.0f, 0.10f, 5000.0f);

		if (setActive)
			m_ActiveProject = project;

		return project;
	}


}