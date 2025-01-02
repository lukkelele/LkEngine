#include "LKpch.h"
#include "EditorSettings.h"

#include "LkEngine/Editor/EditorCore.h"

#include "LkEngine/Core/Globals.h"
#include "LkEngine/Core/IO/FileSystem.h"

#define LK_ENTER_GROUP(Name) CurrentGroup = RootNode[Name];
#define LK_READ_VALUE(Name, Type, Var, DefaultValue)  Var = CurrentGroup[Name].as<Type>(DefaultValue)

#define LK_BEGIN_GROUP(Name)              Out << YAML::Key << Name << YAML::Value << YAML::BeginMap;
#define LK_END_GROUP()                    Out << YAML::EndMap;
#define LK_SERIALIZE_VALUE(Name, InValue) Out << YAML::Key << Name << YAML::Value << InValue;

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
		LK_CORE_DEBUG_TAG("EditorSettings", "Saving: {}", EditorConfig.string());

		FEditorSettingsSerializer Serializer;
		Serializer.Serialize(EditorConfig);
	}

	void FEditorSettings::Load()
	{
		const fs::path EditorConfig = LFileSystem::GetEditorConfig();
		LK_CORE_DEBUG_TAG("EditorSettings", "Loading: {}", EditorConfig.string());
		if (!LFileSystem::Exists(EditorConfig))
		{
			bNewEditorConfigFile = true;
			LK_CORE_INFO_TAG("EditorSettingsSerializer", "Creating editor settings file");
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
		LK_BEGIN_GROUP("EditorSettings");
		{
			LK_BEGIN_GROUP("Editor");
			{
				LK_SERIALIZE_VALUE("TranslationSnapValue", Settings.TranslationSnapValue);
				LK_SERIALIZE_VALUE("RotationSnapValue", Settings.RotationSnapValue);
				LK_SERIALIZE_VALUE("ScaleSnapValue", Settings.ScaleSnapValue);
			}
			LK_END_GROUP();

			LK_BEGIN_GROUP("ContentBrowser");
			{
				LK_SERIALIZE_VALUE("ThumbnailSize", Settings.ContentBrowserThumbnailSize);
			}
			LK_END_GROUP();
		}
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
		YAML::Node CurrentGroup = RootNode;
		LK_CORE_VERIFY(RootNode["Editor"] && RootNode["ContentBrowser"], "Editor settings file is missing one ore more nodes");

		LK_ENTER_GROUP("Editor");
		{
			LK_READ_VALUE("TranslationSnapValue", float, Settings.TranslationSnapValue, 0.50f);
			LK_READ_VALUE("RotationSnapValue", float, Settings.RotationSnapValue, 45.0f);
			LK_READ_VALUE("ScaleSnapValue", float, Settings.ScaleSnapValue, 0.50f);
		}

		LK_ENTER_GROUP("ContentBrowser");
		{
			LK_READ_VALUE("ThumbnailSize", uint16_t, Settings.ContentBrowserThumbnailSize, 128);
		}

		return true;
	}
}
