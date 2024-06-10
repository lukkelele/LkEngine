#include "LKpch.h"
#include "Project.h"

#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/SceneSerializer.h"

#include "LkEngine/Core/Window.h"
#include "LkEngine/Core/Event/SceneEvent.h"

#include "LkEngine/Renderer/TextureLibrary.h"

#include "LkEngine/Editor/EditorLayer.h"

#include "LkEngine/Asset/AssetManager.h"


namespace LkEngine {

	Project::Project(const std::string& name)
		: m_Name(name)
	{
		s_RuntimeAssetManager = Ref<RuntimeAssetManager>::Create();
	}

	void Project::SetActive(Ref<Project> project)
	{
		if (s_ActiveProject != project)
		{
			s_RuntimeAssetManager = nullptr;
		}

		s_ActiveProject = project;
		if (s_ActiveProject)
		{
			if (project->s_RuntimeAssetManager)
			{
				LK_CORE_INFO_TAG("Project", "RuntimeAssetManager was already initialized for project {}", project->GetName());
				s_RuntimeAssetManager = project->s_RuntimeAssetManager;
			}
			else
			{
				LK_CORE_WARN_TAG("Project", "RuntimeAssetManager was NOT initialized for project {}, initializing...", project->GetName());
				s_RuntimeAssetManager = Ref<RuntimeAssetManager>::Create();
				s_RuntimeAssetManager->Init();
			}
		}

	}

	// Save project data, aka serialize to disk
	bool Project::Save()
	{
		LK_CORE_INFO_TAG("Project", "Saving project {}", m_Name);
		//SceneSerializer sceneSerializer(Data.TargetScene);

		return true;
	}

	uint64_t Project::GetSize() const
	{
		return 0;
	}

	Ref<Project> Project::CreateEmptyProject(std::string_view projectName, bool setActive)
	{
		Ref<Project> project = Ref<Project>::Create(std::string(projectName));
		project->m_Scene = Ref<Scene>::Create("StarterScene", true, true);

		if (setActive)
		{
			//SetActive(project);
			//s_ActiveProject = project;
		}

		if (EditorLayer::Get() && EditorLayer::Get()->IsEnabled())
		{
			SceneCreatedEvent e(project->m_Scene);
			EditorLayer::Get()->RegisterEvent(e);
		}

		return project;
	}

	// TODO
	Ref<Project> Project::CreateDefaultProject(std::string_view projectName, bool setActive)
	{
		Ref<Project> project = Ref<Project>::Create("UntitledProject");

		LK_CORE_DEBUG_TAG("Project", "Creating platform entity");
		LEntity basePlatform = project->m_Scene->CreateEntity("Platform");
		SpriteComponent& sc = basePlatform.AddComponent<SpriteComponent>();
		//auto& mc = basePlatform.AddComponent<MaterialComponent>();
		CameraComponent& cc = basePlatform.AddComponent<CameraComponent>();
		LK_CORE_DEBUG_TAG("Project", "Created transform, sprite, material and camera components for platform entity");

		sc.SetSize(20, 20);
		sc.Color = Color::RGBA::Green;
		// Rotate platform

		// Center origin
		auto& tc = basePlatform.Transform();
		tc.Translation.x = sc.Size.x * 0.50f;
		tc.Translation.y = sc.Size.y * 0.50f;

		//LK_CORE_DEBUG_TAG("Project", "Setting texture2D");
		//mc.SetTexture(TextureLibrary::Get()->GetTexture2D("atte_square"));

		LK_CORE_DEBUG_TAG("Project", "Creating scene camera and setting it to perspective");
		cc.Camera = Ref<SceneCamera>::Create();
		cc.Camera->SetPerspective(60.0f, 0.10f, 5000.0f);

		if (setActive)
			s_ActiveProject = project;

		return project;
	}

	// TODO
	Ref<Project> Project::CreateDebugProject(bool setActive)
	{
		Ref<Project> project = Ref<Project>::Create("UntitledProject");

		LK_CORE_DEBUG_TAG("Project", "Creating base entity");
		LEntity atteEntity = project->m_Scene->CreateEntity("Atte");
		SpriteComponent& sc = atteEntity.AddComponent<SpriteComponent>();
		CameraComponent& cc = atteEntity.AddComponent<CameraComponent>();
		LK_CORE_DEBUG_TAG("Project", "Created transform, sprite, material and camera components for base entity");

		sc.SetSize(10, 10);
		sc.Color = Color::RGBA::Green;

		// Center origin
		auto& tc = atteEntity.Transform();
		tc.Translation.x = sc.Size.x * 0.50f;
		tc.Translation.y = sc.Size.y * 0.50f;

		LK_CORE_DEBUG_TAG("Project", "Creating scene camera and setting it to perspective");
		cc.Camera = Ref<SceneCamera>::Create();
		cc.Camera->SetPerspective(60.0f, 0.10f, 5000.0f);

		if (setActive)
			s_ActiveProject = project;

		return project;
	}


}