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
		LCLASS_REGISTER();

		RuntimeAssetManager = TObjectPtr<LRuntimeAssetManager>::Create();
	}

	void LProject::SetActive(TObjectPtr<LProject> InProject)
	{
		LK_CORE_VERIFY(InProject, "Project is nullptr");

		/* The currently active Project is not the same as the passed Project argument. */
		if (ActiveProject != InProject)
		{
			/* Release resources. */
			RuntimeAssetManager->BeginDestroy();

			RuntimeAssetManager.Release();
			RuntimeAssetManager = nullptr;
		}

		ActiveProject = InProject;
		if (ActiveProject)
		{
			if (InProject->RuntimeAssetManager)
			{
				LK_CORE_TRACE_TAG("Project", "RuntimeAssetManager already initialized for Project {}", 
								  ActiveProject->GetName());
				RuntimeAssetManager = ActiveProject->RuntimeAssetManager;
			}
			else
			{
				LK_CORE_WARN_TAG("Project", "RuntimeAssetManager was NOT initialized for Project {}", 
								 ActiveProject->GetName());
				RuntimeAssetManager = TObjectPtr<LRuntimeAssetManager>::Create();
				RuntimeAssetManager->Initialize(EInitFlag::NoInit);
			}
		}

	}

	bool LProject::Save()
	{
		LK_CORE_INFO_TAG("Project", "Saving Project: \"{}\"", m_Name);
		/* Serialize to disk. */
		//LSceneSerializer SceneSerializer(Data.TargetScene);

		return true;
	}

	uint64_t LProject::GetSize() const
	{
		return 0;
	}

	TObjectPtr<LProject> LProject::CreateEmptyProject(std::string_view ProjectName, bool bSetAsActive)
	{
		LK_CORE_DEBUG_TAG("Project", "Creating empty Project \"{}\", active: {}", ProjectName, bSetAsActive);
		TObjectPtr<LProject> Project = TObjectPtr<LProject>::Create(std::string(ProjectName));
		Project->Scene = TObjectPtr<LScene>::Create("StarterScene", true, true);

		if (LEditorLayer* Editor = LEditorLayer::Get(); Editor && Editor->IsEnabled())
		{
			GOnSceneSetActive.Broadcast(Project->Scene);
		}

		if (bSetAsActive)
		{
			LProject::SetActive(Project);
			ActiveProject = Project;
		}

		return Project;
	}

	// TODO
	TObjectPtr<LProject> LProject::CreateDefaultProject(std::string_view ProjectName, const bool bSetAsActive)
	{
		TObjectPtr<LProject> Project = TObjectPtr<LProject>::Create("UntitledProject");

#if true 
		LK_CORE_DEBUG_TAG("Project", "Creating platform entity");
		LEntity BasePlatform = Project->Scene->CreateEntity("Platform");
		LSpriteComponent& SpriteComponent = BasePlatform.AddComponent<LSpriteComponent>();

		LCameraComponent& CameraComponent = BasePlatform.AddComponent<LCameraComponent>();
		LK_CORE_DEBUG_TAG("Project", "Created transform, sprite, material and camera components for platform entity");

		SpriteComponent.SetSize(20, 20);
		SpriteComponent.Color = Color::RGBA::Green;

		/* Center origin. */
		LTransformComponent& TransformComponent = BasePlatform.Transform();
		TransformComponent.Translation.x = SpriteComponent.Size.x * 0.50f;
		TransformComponent.Translation.y = SpriteComponent.Size.y * 0.50f;

		CameraComponent.Camera = TObjectPtr<LSceneCamera>::Create();
		CameraComponent.Camera->SetPerspective(60.0f, 0.10f, 5000.0f);

		if (bSetAsActive)
		{
			ActiveProject = Project;
		}

		return Project;
	}

	// TODO
	TObjectPtr<LProject> LProject::CreateDebugProject(const bool bSetAsActive)
	{
		LK_MARK_FUNC_NOT_IMPLEMENTED("Not ready");
#if 0
		TObjectPtr<LProject> Project = TObjectPtr<LProject>::Create("UntitledProject");

		LK_CORE_DEBUG_TAG("Project", "Creating base entity");
		LEntity atteEntity = Project->Scene->CreateEntity("Atte");
		LSpriteComponent& SpriteComponent = atteEntity.AddComponent<LSpriteComponent>();
		LCameraComponent& CameraComponent = atteEntity.AddComponent<LCameraComponent>();
		LK_CORE_DEBUG_TAG("Project", "Created transform, sprite, material and camera components for base entity");

		SpriteComponent.SetSize(10, 10);
		SpriteComponent.Color = Color::RGBA::Green;

		/* Center origin. */
		LTransformComponent& TransformComponent = atteEntity.Transform();
		TransformComponent.Translation.x = SpriteComponent.Size.x * 0.50f;
		TransformComponent.Translation.y = SpriteComponent.Size.y * 0.50f;

		LK_CORE_DEBUG_TAG("Project", "Creating SpriteComponentene camera and setting it to perspective");
		CameraComponent.Camera = TObjectPtr<LSceneCamera>::Create();
		CameraComponent.Camera->SetPerspective(60.0f, 0.10f, 5000.0f);

		if (bSetAsActive)
		{
			ActiveProject = Project;
		}

		return Project;
#endif
		return nullptr;

#endif
	}


}