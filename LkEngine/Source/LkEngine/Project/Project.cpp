#include "LKpch.h"
#include "Project.h"
#include "ProjectSerializer.h"

#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/SceneSerializer.h"

#include "LkEngine/Core/Window.h"
#include "LkEngine/Core/IO/FileSystem.h"

#include "LkEngine/Renderer/UI/UILayer.h"

#include "LkEngine/Asset/AssetManager.h"


namespace LkEngine {

	namespace fs = std::filesystem;

	LProject::FOnProjectChanged LProject::OnProjectChanged{};

	LProject::LProject()
	{
		LOBJECT_REGISTER();
	}

	void LProject::Load(const std::string& ProjectPath)
	{
		LK_CORE_DEBUG_TAG("Project", "Loading: {}", ProjectPath);
		if (ProjectPath.empty())
		{
			LK_CORE_ERROR_TAG("Project", "Could not load project, the path is empty");
			return;
		}
		else if (!LFileSystem::Exists(ProjectPath))
		{
			LK_CORE_ERROR_TAG("Project", "Could not load project, the path does not exist: '{}'", ProjectPath);
			return;
		}

		std::filesystem::path Filepath = ProjectPath;

		/* Add the name of the project directory with the project suffix to get the project name. */
		if (LFileSystem::IsDirectory(Filepath))
		{
			Filepath = (Filepath / fs::path(Filepath.filename().string() + "." + LProject::FILE_EXTENSION));
		}

		/* Load serialized data into the project instance. */
		LK_CORE_DEBUG("Deserializing project: {}", ProjectPath);
		FProjectSerializer ProjectSerializer(this);
		if (!ProjectSerializer.Deserialize(Filepath))
		{
			LK_CORE_ERROR_TAG("Project", "Deserialization of project '{}' failed", Filepath.string());
		}

		/* TODO: Load the project configuration from the project serializer. */
		Configuration.AssetDirectory = LFileSystem::GetAssetsDir().string();
	}

	bool LProject::Save()
	{
		LK_CORE_CONSOLE_INFO("Saving project: {}", Configuration.Name);
		if (AssetManager)
		{
			AssetManager->WriteRegistryToDisk();
		}
		else
		{
			LK_CORE_ERROR_TAG("Project", "Failed to write asset registry to disk, no asset manager instance exists");
		}

		/* Serialize to disk. */
		FProjectSerializer ProjectSerializer(this);

		const std::filesystem::path ProjectPath = Configuration.ProjectDirectory;
		LK_CORE_TRACE("Project path: {}", ProjectPath.string());
		if (ProjectPath.empty())
		{
			LK_CORE_ERROR_TAG("Project", "Could not save project: '{}'", ProjectPath.string());
			return false;
		}

		ProjectSerializer.Serialize(ProjectPath);

		/* Save the scene. */
		if (TObjectPtr<LScene> Scene = LScene::GetActiveScene(); Scene != nullptr)
		{
			LK_CORE_INFO_TAG("Project", "Saving project scene: {}", Scene->GetName());
			LSceneSerializer SceneSerializer(Scene);
			const std::string SceneFilepath = std::format("{}/Scenes/{}.{}", Configuration.ProjectDirectory, Scene->GetName(), LScene::FILE_EXTENSION);
			LK_CORE_DEBUG_TAG("Project", "Scene filepath: {}", SceneFilepath);
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
			AssetManager = nullptr;
		}

		ActiveProject = InProject;
		if (ActiveProject)
		{
			if (InProject->AssetManager)
			{
				/* TODO: Destroy and re-initialize the asset manager here. */
				LK_CORE_INFO_TAG("Project", "AssetManager already initialized for project '{}'", ActiveProject->GetName());
				AssetManager = TObjectPtr<LEditorAssetManager>::Create();
			}
			else
			{
				LK_CORE_INFO_TAG("Project", "Creating the asset manager");
				AssetManager = TObjectPtr<LEditorAssetManager>::Create();
				AssetManager->Initialize();
			}

			OnProjectChanged.Broadcast(ActiveProject);
		}
	}

	std::filesystem::path LProject::GetAssetDirectory()
	{
		LK_CORE_VERIFY(ActiveProject);
		return ActiveProject->Configuration.AssetDirectory;
	}

	std::filesystem::path LProject::GetAssetRegistryPath()
	{
		LK_CORE_VERIFY(ActiveProject);
		return std::filesystem::path(ActiveProject->Configuration.AssetRegistryPath);
	}

	std::filesystem::path LProject::GetMeshDirectory()
	{
		LK_CORE_VERIFY(ActiveProject);
		return std::filesystem::path(ActiveProject->Configuration.MeshDirectory);
	}

}
