#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"
#include "LkEngine/Asset/Asset.h"

#include "LkEngine/Serialization/Serializer.h"
#include "LkEngine/Serialization/FileStream.h"


namespace LkEngine {

	struct FEditorSettings
	{
		float TranslationSnapValue = 0.10f;
		float RotationSnapValue = 5.0f;
		float ScaleSnapValue = 0.10f;

		uint16_t ContentBrowserThumbnailSize = 128;
		bool bContentBrowserShowAssetTypes = false;

		std::string Theme{};

		static FEditorSettings& Get();

		void Save();
		void Load();
	};

	class FEditorSettingsSerializer
	{
	public:
		FEditorSettingsSerializer() = default;
		~FEditorSettingsSerializer() = default;

		void Serialize(const std::filesystem::path& EditorConfig);
		void Deserialize(const std::filesystem::path& EditorConfig, FEditorSettings& Settings);

	private:
        void SerializeToYaml(YAML::Emitter& Out, const FEditorSettings& Settings);
        bool DeserializeFromYaml(const std::string& YamlString, FEditorSettings& Settings);
	};

}
