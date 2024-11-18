#include "LKpch.h"
#include "Project.h"

#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/SceneSerializer.h"

#include "LkEngine/Core/Window.h"
#include "LkEngine/Core/Event/SceneEvent.h"

#include "LkEngine/Renderer/TextureLibrary.h"
#include "LkEngine/Editor/EditorLayer.h"
#include "LkEngine/UI/UILayer.h"

#include "LkEngine/Asset/AssetManager.h"

#include "ProjectSerializer.h"


namespace LkEngine {

	namespace fs = std::filesystem;

	LProject::LProject()
	{
		/* Load default empty project. */
		LCLASS_REGISTER();
	}

	void LProject::Load(const std::string& ProjectPath)
	{
		if (ProjectPath.empty() || !fs::exists(ProjectPath))
		{
			LK_CORE_ERROR_TAG("Project", "Could not load project: '{}'", ProjectPath);
			return;
		}

		FProjectSerializer Serializer(this);

		/**
		 * Load serialized data into the project. 
		 * The data is read into FProjectConfiguration.
		 */
		LK_CORE_WARN("Deserializing project");
		if (!Serializer.Deserialize(ProjectPath))
		{
			LK_CORE_ERROR_TAG("Project", "Deserialization of project '{}' failed", ProjectPath);
		}
	}

	bool LProject::Save()
	{
		LK_CORE_INFO_TAG("Project", "Saving: \"{}\"", Configuration.Name);

		/* Serialize to disk. */
		FProjectSerializer Serializer(this);

		const fs::path ProjectPath = LK_FORMAT_STRING("Projects/{}", Configuration.Name);
		if (ProjectPath.empty())
		{
			LK_CORE_ERROR_TAG("Project", "Could not save project: '{}'", ProjectPath.string());
			return false;
		}

		/* Make sure directory exists. */
		if (ProjectPath.has_parent_path())
		{
			//const fs::path ParentPath = ProjectPath.parent_path();
		}
		else
		{
			fs::create_directories("Projects");
		}

		Serializer.Serialize(ProjectPath);

		return true;
	}


	void LProject::SetActive(TObjectPtr<LProject> InProject)
	{
		LK_CORE_VERIFY(InProject, "Project is nullptr");

		/* The currently active Project is not the same as the passed Project argument. */
		if (ActiveProject && ActiveProject != InProject)
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
				LK_CORE_INFO_TAG("Project", "AssetManager already initialized for project '{}'", ActiveProject->GetName());
				RuntimeAssetManager = ActiveProject->RuntimeAssetManager;
			}
			else
			{
				LK_CORE_INFO("Initializing the asset manager");
				RuntimeAssetManager = TObjectPtr<LRuntimeAssetManager>::Create();
				RuntimeAssetManager->Initialize();
			}
		}
	}

	uint64_t LProject::GetSize() const
	{
		return 0;
	}

}
