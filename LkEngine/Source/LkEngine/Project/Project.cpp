#include "LKpch.h"
#include "Project.h"

#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/SceneSerializer.h"

#include "LkEngine/Core/Window.h"
#include "LkEngine/Core/IO/FileSystem.h"

#include "LkEngine/Renderer/TextureLibrary.h"
#include "LkEngine/Editor/EditorLayer.h"
#include "LkEngine/UI/UILayer.h"

#include "LkEngine/Asset/AssetManager.h"

#include "ProjectSerializer.h"


namespace LkEngine {

	namespace fs = std::filesystem;

	LProject::FOnProjectChanged LProject::OnProjectChanged{};

	LProject::LProject()
	{
		LOBJECT_REGISTER();
	}

	void LProject::Load(const std::string& ProjectPath)
	{
		if (ProjectPath.empty() || !LFileSystem::Exists(ProjectPath))
		{
			LK_CORE_ERROR_TAG("Project", "Could not load project: '{}'", ProjectPath);
			return;
		}

		std::filesystem::path Filepath = ProjectPath;

		/**
		 * Add the name of the project directory with the project suffix to get
		 * the project name. 
		 */
		if (LFileSystem::IsDirectory(Filepath))
		{
			Filepath = (Filepath / fs::path(Filepath.filename().string() + "." + LProject::FILE_EXTENSION));
		}

		/** Load serialized data into the project instance. */
		FProjectSerializer ProjectSerializer(this);
		if (!ProjectSerializer.Deserialize(Filepath))
		{
			LK_CORE_ERROR_TAG("Project", "Deserialization of project '{}' failed", Filepath.string());
		}

		/* TODO: Load the project configuration from the project serializer. */
		Configuration.AssetDirectory = LFileSystem::GetAssetsDir().string();
	}

	std::filesystem::path LProject::GetAssetDirectory()
	{
		return LFileSystem::GetAssetsDir();
	}

	std::filesystem::path LProject::GetAssetRegistryPath()
	{
		/* TODO: This should be unique for every project and not some global path hardcoded like now. */
		//return LFileSystem::GetRuntimeDir() / "AssetRegistry.lkreg";
		return LFileSystem::GetAssetsDir() / "AssetRegistry.lkr";
	}

	bool LProject::Save()
	{
		LK_CORE_INFO_TAG("Project", "Saving: \"{}\"", Configuration.Name);

		/* Serialize to disk. */
		FProjectSerializer ProjectSerializer(this);

		const fs::path ProjectPath = LK_FORMAT_STRING("Projects/{}/{}", Configuration.Name, Configuration.Name);
		if (ProjectPath.empty())
		{
			LK_CORE_ERROR_TAG("Project", "Could not save project: '{}'", ProjectPath.string());
			return false;
		}

		ProjectSerializer.Serialize(ProjectPath);

		/* Serialize the active scene. */
		if (!LFileSystem::Exists("Scenes"))
		{
			LK_INFO("Creating 'Scenes' directory");
			LFileSystem::CreateDirectory("Scenes");
		}

		/* Save the scene. */
		if (TObjectPtr<LScene> Scene = LScene::GetActiveScene())
		{
			LSceneSerializer SceneSerializer(Scene);
			const std::string SceneFilepath = LK_FORMAT_STRING("Scenes/{}.{}", Scene->GetName(), LScene::FILE_EXTENSION);
			SceneSerializer.Serialize(SceneFilepath);
		}

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
				/* TODO: Destroy and re-initialize the asset manager here. */
				LK_CORE_INFO_TAG("Project", "AssetManager already initialized for project '{}'", ActiveProject->GetName());
				RuntimeAssetManager = ActiveProject->RuntimeAssetManager;
			}
			else
			{
				LK_CORE_INFO("Initializing the asset manager");
				RuntimeAssetManager = TObjectPtr<LRuntimeAssetManager>::Create();
				RuntimeAssetManager->Initialize();
			}

			OnProjectChanged.Broadcast(ActiveProject);
		}
	}

	uint64_t LProject::GetSize() const
	{
		return 0;
	}

}
