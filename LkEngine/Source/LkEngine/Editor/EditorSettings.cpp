#include "LKpch.h"
#include "EditorSettings.h"

#include "LkEngine/Editor/EditorCore.h"

#include "LkEngine/Core/Globals.h"
#include "LkEngine/Core/IO/FileSystem.h"


namespace LkEngine {

	namespace fs = std::filesystem;

	namespace 
	{
		/**
		 * Flag for handling loading and saving of editor settings when 
		 * no file existed before.
		 */
		bool bNewEditorConfigFile = false;
	}

	FEditorSettings& FEditorSettings::Get()
	{
		static FEditorSettings EditorSettings;
		return EditorSettings;
	}

	void FEditorSettings::Save()
	{
		const fs::path EditorConfig = LFileSystem::GetEditorConfig();
		LK_CORE_DEBUG_TAG("EditorSettings", "Saving: {}", 
						  LFileSystem::ConvertToUnixPath(fs::relative(EditorConfig.string(), LFileSystem::GetEngineDir())));

		FEditorSettingsSerializer Serializer;
		Serializer.Serialize(EditorConfig);
	}

	void FEditorSettings::Load()
	{
		const fs::path EditorConfig = LFileSystem::GetEditorConfig();
		LK_CORE_DEBUG_TAG("EditorSettings", "Loading: {}", 
						  LFileSystem::ConvertToUnixPath(fs::relative(EditorConfig.string(), LFileSystem::GetEngineDir())));
		if (!LFileSystem::Exists(EditorConfig))
		{
			bNewEditorConfigFile = true;
			LK_CORE_INFO_TAG("EditorSettingsSerializer", "No editor settings config exists, creating one");
			std::ofstream File(EditorConfig);
			if (!File.is_open())
			{
				LK_CORE_ERROR_TAG("EditorSettingsSerializer", "Failed to create editor config file");
				return;
			}

			File.close();
		}

		FEditorSettings& EditorSettings = FEditorSettings::Get();
		FEditorSettingsSerializer Serializer;
		Serializer.Deserialize(EditorConfig, EditorSettings);
	}

	void FEditorSettingsSerializer::Serialize(const std::filesystem::path& EditorConfig)
	{
		const FEditorSettings& Settings = FEditorSettings::Get();

		YAML::Emitter Out;
		SerializeToYaml(Out, Settings);

		std::ofstream FileOut(EditorConfig);
		FileOut << Out.c_str();
		FileOut.close();
	}

	void FEditorSettingsSerializer::Deserialize(const std::filesystem::path& EditorConfig, FEditorSettings& Settings)
	{
		std::ifstream InputStream(EditorConfig);
		LK_CORE_VERIFY(InputStream, "Inputstream failed for: {}", EditorConfig.string());
		std::stringstream StringStream;
		StringStream << InputStream.rdbuf();

		DeserializeFromYaml(StringStream.str(), Settings);
		InputStream.close();
	}

	void FEditorSettingsSerializer::SerializeToYaml(YAML::Emitter& Out, const FEditorSettings& Settings)
	{
		Out << YAML::BeginMap;
		LK_SERIALIZE_BEGIN_GROUP(EditorSettings);
		{
			LK_SERIALIZE_BEGIN_GROUP(Editor);
			{
				LK_SERIALIZE_VALUE(TranslationSnapValue, Settings.TranslationSnapValue);
				LK_SERIALIZE_VALUE(RotationSnapValue, Settings.RotationSnapValue);
				LK_SERIALIZE_VALUE(ScaleSnapValue, Settings.ScaleSnapValue);
				LK_SERIALIZE_VALUE(Theme, Settings.Theme);
			}
			LK_SERIALIZE_END_GROUP();

			LK_SERIALIZE_BEGIN_GROUP(ContentBrowser);
			{
				LK_SERIALIZE_VALUE(ThumbnailSize, Settings.ContentBrowserThumbnailSize);

				LK_SERIALIZE_BEGIN_GROUP(Debug);
				{
					LK_SERIALIZE_VALUE(DrawOutlinerBorders, UI::Debug::ContentBrowser::bDrawOutlinerBorders);
				}
				LK_SERIALIZE_END_GROUP();
			}
			LK_SERIALIZE_END_GROUP();
		}
		LK_SERIALIZE_END_GROUP();
		Out << YAML::EndMap;
	}

	bool FEditorSettingsSerializer::DeserializeFromYaml(const std::string& YamlString, FEditorSettings& Settings)
	{
		const YAML::Node Data = YAML::Load(YamlString);
		if (!Data["EditorSettings"])
		{
			if (bNewEditorConfigFile)
			{
				return true;
			}

			LK_CORE_ERROR_TAG("EditorSettingsSerializer", "Missing 'EditorSettings' node in editor settings file");
			return false;
		}

		YAML::Node RootNode = Data["EditorSettings"];

		/* Node: Editor */
		if (YAML::Node EditorNode = RootNode["Editor"])
		{
			LK_DESERIALIZE_PROPERTY(TranslationSnapValue, Settings.TranslationSnapValue, EditorNode, 0.50f);
			LK_DESERIALIZE_PROPERTY(RotationSnapValue, Settings.RotationSnapValue, EditorNode, 45.0f);
			LK_DESERIALIZE_PROPERTY(ScaleSnapValue, Settings.ScaleSnapValue, EditorNode, 0.50f);
			LK_DESERIALIZE_PROPERTY(Theme, Settings.Theme, EditorNode, "");
		}
		else
		{
			LK_CORE_ERROR_TAG("EditorSettings", "Missing 'Editor' node");
		}

		/* Node: ContentBrowser */
		if (YAML::Node ContentBrowserNode = RootNode["ContentBrowser"])
		{
			LK_DESERIALIZE_PROPERTY(ThumbnailSize, Settings.ContentBrowserThumbnailSize, ContentBrowserNode, 128);

			if (YAML::Node DebugNode = ContentBrowserNode["Debug"])
			{
				LK_DESERIALIZE_PROPERTY(DrawOutlinerBorders, UI::Debug::ContentBrowser::bDrawOutlinerBorders, DebugNode, false);
			}
		}
		else
		{
			LK_CORE_ERROR_TAG("EditorSettings", "Missing 'ContentBrowser' node");
		}

		return true;
	}

}
