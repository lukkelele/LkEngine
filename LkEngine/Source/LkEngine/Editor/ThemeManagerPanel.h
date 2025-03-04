#pragma once

#include "LkEngine/Renderer/UI/Panel.h"


namespace LkEngine {

	/**
	 * ETheme
	 *
	 *  Type of theme.
	 */
	enum class ETheme
	{
		Dark,
		Light,
		GruvboxMaterial,
		COUNT,
	};
	LK_ENUM_RANGE_BY_COUNT(ETheme, ETheme::COUNT);

	namespace Enum 
	{
		static const char* ToString(const ETheme Theme);
	}

	class LThemeManagerPanel : public IPanel
	{
	public:
		LThemeManagerPanel();
		~LThemeManagerPanel() = default;

		virtual void RenderUI(bool& IsOpen) override;
		virtual void Render() override {}

		static void SetTheme(const ETheme Theme);
		static std::string GetCurrentThemeName() { return CurrentThemeName; }

		static void LoadDefaultTheme();

		static void SetSelectorEnabled(const bool InEnabled);
		FORCEINLINE static bool IsSelectorEnabled() { return bSelectorEnabled; }

		static bool SaveCurrentTheme();
		static bool LoadSavedTheme(std::string_view ThemeName);

		static void OnMouseButtonPressed(const FMouseButtonData& ButtonData);

	public:
		inline static std::string_view THEME_FILE_EXTENSION = "lktheme";
	private:
		virtual void SerializeToYaml(YAML::Emitter& Out) const override;
		virtual void DeserializeFromYaml(const YAML::Node& Data) override;

		static void LoadTheme(const ETheme Theme, ImVec4* Colors);
		void LoadThemeFromYaml(const YAML::Node& Data);

		void UI_SaveThemePopup();
		void UI_SelectSavedThemePopup();

		void UI_ColorPicker();

		static bool IsDefaultTheme(std::string_view InThemeName)
		{
			for (const ETheme Theme : TEnumRange<ETheme>())
			{
				if (InThemeName == Enum::ToString(Theme))
				{
					return true;
				}
			}

			return false;
		}

	private:
		inline static std::string CurrentThemeName{};
		inline static bool bSelectorEnabled = false;
		inline static std::filesystem::path ThemesDirectory{};

		friend class LEditorLayer;

		LPANEL(LThemeManagerPanel);
	};

	namespace Enum 		
	{
		static const char* ToString(const ETheme Theme)
		{
			switch (Theme)
			{
				case ETheme::Dark:            return "Dark";
				case ETheme::Light:           return "Light";
				case ETheme::GruvboxMaterial: return "GruvboxMaterial";
				case ETheme::COUNT:           LK_CORE_VERIFY(false, "Enum::ToString(const ETheme) failed");
			}

			LK_CORE_VERIFY(false, "Enum::ToString(const ETheme) failed for: {}", static_cast<int>(Theme));
			return nullptr;
		}
	}

}
