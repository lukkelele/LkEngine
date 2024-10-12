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

	LProject::LProject(std::string_view InProjectName)
		: m_Name(InProjectName)
	{
		//RuntimeAssetManager = TObjectPtr<LRuntimeAssetManager>::Create();
		RuntimeAssetManager = TObjectPtr<LRuntimeAssetManager>::Create();
	}

	void LProject::SetActive(TObjectPtr<LProject> project)
	{
		LK_CORE_VERIFY(project, "Project is nullptr");

		/* The currently active project is not the same as the passed project argument. */
		if (ActiveProject != project)
		{
			RuntimeAssetManager = nullptr;
		}

		ActiveProject = project;
		if (ActiveProject)
		{
			if (project->RuntimeAssetManager)
			{
				LK_CORE_TRACE_TAG("Project", "RuntimeAssetManager was already initialized for project {}", project->GetName());
				RuntimeAssetManager = project->RuntimeAssetManager;
			}
			else
			{
				LK_CORE_WARN_TAG("Project", "RuntimeAssetManager was NOT initialized for project {}, initializing...", project->GetName());
				RuntimeAssetManager = TObjectPtr<LRuntimeAssetManager>::Create();
				RuntimeAssetManager->Initialize(EInitFlag::NoInit);
			}
		}

	}

	// Save project data, aka serialize to disk
	bool LProject::Save()
	{
		LK_CORE_INFO_TAG("Project", "Saving project {}", m_Name);
		//SceneSerializer sceneSerializer(Data.TargetScene);

		return true;
	}

	uint64_t LProject::GetSize() const
	{
		return 0;
	}

	TObjectPtr<LProject> LProject::CreateEmptyProject(std::string_view projectName, bool bSetAsActive)
	{
		LK_CORE_DEBUG_TAG("Project", "Creating empty project \"{}\", setting as active: {}", 
						  projectName, bSetAsActive);
		TObjectPtr<LProject> project = TObjectPtr<LProject>::Create(std::string(projectName));
		project->Scene = TObjectPtr<LScene>::Create("StarterScene", true, true);

		if (LEditorLayer* Editor = LEditorLayer::Get(); Editor && Editor->IsEnabled())
		{
			// REMOVE
			SceneCreatedEvent Event(project->Scene);
			Editor->RegisterEvent(Event);
		}

		if (bSetAsActive)
		{
			LProject::SetActive(project);
			ActiveProject = project;
		}

		return project;
	}

	// TODO
	TObjectPtr<LProject> LProject::CreateDefaultProject(std::string_view projectName, const bool bSetAsActive)
	{
		TObjectPtr<LProject> project = TObjectPtr<LProject>::Create("UntitledProject");

#if true 
		LK_CORE_DEBUG_TAG("Project", "Creating platform entity");
		LEntity BasePlatform = project->Scene->CreateEntity("Platform");
		LSpriteComponent& sc = BasePlatform.AddComponent<LSpriteComponent>();

		//auto& mc = basePlatform.AddComponent<MaterialComponent>();
		LCameraComponent& CameraComponent = BasePlatform.AddComponent<LCameraComponent>();
		LK_CORE_DEBUG_TAG("Project", "Created transform, sprite, material and camera components for platform entity");

		sc.SetSize(20, 20);
		sc.Color = Color::RGBA::Green;
		// Rotate platform

		// Center origin
		auto& tc = BasePlatform.Transform();
		tc.Translation.x = sc.Size.x * 0.50f;
		tc.Translation.y = sc.Size.y * 0.50f;

		//LK_CORE_DEBUG_TAG("Project", "Setting texture2D");
		//mc.SetTexture(TextureLibrary::Get()->GetTexture2D("atte_square"));

		LK_CORE_DEBUG_TAG("Project", "Creating scene camera and setting it to perspective");
		CameraComponent.Camera = TObjectPtr<LSceneCamera>::Create();
		CameraComponent.Camera->SetPerspective(60.0f, 0.10f, 5000.0f);

		if (bSetAsActive)
		{
			ActiveProject = project;
		}

		return project;
	}

	// TODO
	TObjectPtr<LProject> LProject::CreateDebugProject(const bool bSetAsActive)
	{
#if 0
		TObjectPtr<LProject> project = TObjectPtr<LProject>::Create("UntitledProject");

		LK_CORE_DEBUG_TAG("Project", "Creating base entity");
		LEntity atteEntity = project->Scene->CreateEntity("Atte");
		LSpriteComponent& sc = atteEntity.AddComponent<LSpriteComponent>();
		LCameraComponent& CameraComponent = atteEntity.AddComponent<LCameraComponent>();
		LK_CORE_DEBUG_TAG("Project", "Created transform, sprite, material and camera components for base entity");

		sc.SetSize(10, 10);
		sc.Color = Color::RGBA::Green;

		// Center origin
		auto& tc = atteEntity.Transform();
		tc.Translation.x = sc.Size.x * 0.50f;
		tc.Translation.y = sc.Size.y * 0.50f;

		LK_CORE_DEBUG_TAG("Project", "Creating scene camera and setting it to perspective");
		CameraComponent.Camera = TObjectPtr<LSceneCamera>::Create();
		CameraComponent.Camera->SetPerspective(60.0f, 0.10f, 5000.0f);

		if (bSetAsActive)
		{
			ActiveProject = project;
		}

		return project;
#endif
		return nullptr;

#endif
	}


}