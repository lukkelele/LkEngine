#include "LKpch.h"
#include "AssetEditorManager.h"

#include "AssetEditors/MaterialEditor.h"

#include "LkEngine/Project/Project.h"
#include "LkEngine/Scene/Scene.h"


namespace LkEngine {

	std::unordered_map<EAssetType, std::unique_ptr<LAssetEditor>> LAssetEditorManager::AssetEditorMap{};
	TObjectPtr<LScene> LAssetEditorManager::SceneContext{};

	LAssetEditorManager::LAssetEditorManager()
	{
	}

	void LAssetEditorManager::Tick(const float DeltaTime)
	{
		for (auto& [AssetType, Editor] : AssetEditorMap)
		{
			Editor->Tick(DeltaTime);
		}
	}

	void LAssetEditorManager::RenderUI()
	{
		for (auto& [AssetType, Editor] : AssetEditorMap)
		{
			Editor->RenderUI();
		}
	}

	void LAssetEditorManager::OpenEditor(const TObjectPtr<LAsset>& Asset)
	{
		LK_CORE_VERIFY(Asset && Asset->IsValid(), "LAssetEditorManager::OpenEditor failed, invalid asset");

		if (!AssetEditorMap.contains(Asset->GetAssetType()))
		{
			LK_CORE_WARN("No asset editor exists for: {}", Enum::ToString(Asset->GetAssetType()));
			return;
		}

		std::unique_ptr<LAssetEditor>& Editor = AssetEditorMap.at(Asset->GetAssetType());
		const FAssetMetadata& Metadata = LProject::GetEditorAssetManager()->GetMetadata(Asset->Handle);
		Editor->SetTitle(Metadata.FilePath.filename().string());
		Editor->SetAsset(Asset);
		Editor->SetOpen(true);
	}

	void LAssetEditorManager::SetSceneContext(const TObjectPtr<LScene>& InScene)
	{
		LK_CORE_INFO_TAG("AssetEditorManager", "Changing scene context");
		SceneContext = InScene;
	}

	void LAssetEditorManager::SetEditorOpen(const EAssetType AssetType, const bool Open)
	{
		if (!AssetEditorMap.contains(AssetType))
		{
			LK_CORE_ERROR_TAG("AssetEditorManager", "No editor exists for: {}", Enum::ToString(AssetType));
			return;
		}

		LK_CORE_DEBUG_TAG("AssetEditorManager", "{} {} editor", (Open ? "Opening" : "Closing"), Enum::ToString(AssetType));
		AssetEditorMap.at(AssetType)->SetOpen(Open);
	}

	void LAssetEditorManager::RegisterEditors()
	{
		LK_CORE_INFO_TAG("AssetEditorManager", "Registering editors");
		RegisterEditor<LMaterialEditor>(EAssetType::Material);

		Deserialize();
	}

	void LAssetEditorManager::UnregisterEditors()
	{
		Serialize();
	}

	void LAssetEditorManager::Serialize() 
	{
		YAML::Emitter Out;
		Out << YAML::BeginMap;

		Out << YAML::Key << "AssetEditorManager" << YAML::Value << YAML::BeginSeq;
		{
			for (const auto& [AssetType, Editor] : AssetEditorMap)
			{
				LK_CORE_TRACE_TAG("AssetEditorManager", "Serializing: {}", Editor->GetTitle());
				Out << YAML::BeginMap;
				Out << YAML::Key << "Title"  << YAML::Value << Editor->GetTitle();
				Out << YAML::Key << "ID"     << YAML::Value << Editor->GetID();
				Out << YAML::Key << "Type"   << YAML::Value << Enum::ToString(AssetType);
				Out << YAML::Key << "IsOpen" << YAML::Value << Editor->IsOpen();
				Editor->SerializeToYaml(Out);
				Out << YAML::EndMap;
			}
		}
		Out << YAML::EndSeq;
		Out << YAML::EndMap;

		std::ofstream FileOut(LFileSystem::GetConfigDir() / "AssetEditor.yaml");
		FileOut << Out.c_str();
		FileOut.close();
	}

	void LAssetEditorManager::Deserialize()
	{
		LK_CORE_DEBUG_TAG("AssetEditorManager", "Deserializing");
		const std::filesystem::path ConfigFilePath = LFileSystem::GetConfigDir() / "AssetEditor.yaml";

		YAML::Node Data;
		try
		{
			Data = YAML::LoadFile(ConfigFilePath.string());
		}
		catch (YAML::BadFile& Exception)
		{
			LK_CORE_VERIFY(false, "Failed to load file: {}\nError: {}", ConfigFilePath.string(), Exception.what());
		}
		catch (YAML::EmitterException& Exception)
		{
			LK_CORE_VERIFY(false, "Failed to load file: {}\nError: {}", ConfigFilePath.string(), Exception.what());
		}

		for (auto EditorNode : Data["AssetEditorManager"])
		{
			if (!EditorNode["Type"])
			{
				LK_CORE_ERROR_TAG("AssetEditorManager", "Missing node 'Type'");
				continue;
			}

			const std::string Type = EditorNode["Type"].as<std::string>();
			LK_CORE_TRACE_TAG("AssetEditorManager", "Type: {}", Type);

			/**
			 * Editor: Material Editor
			 */
			if (Type == Enum::ToString(EAssetType::Material))
			{
				auto& Editor = AssetEditorMap.at(EAssetType::Material);

				if (EditorNode["Title"])
				{
					Editor->SetTitle(EditorNode["Title"].as<std::string>());
				}
				if (EditorNode["IsOpen"])
				{
					Editor->SetOpen(EditorNode["IsOpen"].as<bool>());
				}

				Editor->DeserializeFromYaml(EditorNode);
			}
		}
	}

}
