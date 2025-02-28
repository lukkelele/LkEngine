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
		Custom,
		COUNT,
	};
	LK_ENUM_RANGE_BY_COUNT(ETheme, ETheme::COUNT);


	class LThemeManagerPanel : public IPanel
	{
	public:
		LThemeManagerPanel();
		~LThemeManagerPanel() = default;

		virtual void RenderUI(bool& IsOpen) override;
		virtual void Render() override {}

		static void SetTheme(const ETheme Theme);
		static ETheme GetTheme() { return CurrentTheme; }

		static void SetSelectorEnabled(const bool InEnabled);
		FORCEINLINE static bool IsSelectorEnabled() { return bSelectorEnabled; }

		static bool SaveCurrentTheme();

		static void OnMouseButtonPressed(const FMouseButtonData& ButtonData);

	private:
		virtual void SerializeToYaml(YAML::Emitter& Out) const override;
		virtual void DeserializeFromYaml(const YAML::Node& Data) override;

	private:
		inline static std::array<char, 140> ThemeName;
		inline static ETheme CurrentTheme = ETheme::Dark;
		inline static bool bSelectorEnabled = false;

		friend class LEditorLayer;

		LPANEL(LThemeManagerPanel);
	};


	namespace Enum 		
	{
		static constexpr const char* ToString(const ETheme Theme)
		{
			switch (Theme)
			{
				case ETheme::Dark:            return "Dark";
				case ETheme::Light:           return "Light";
				case ETheme::GruvboxMaterial: return "GruvboxMaterial";
				case ETheme::Custom:          return "Custom";
				case ETheme::COUNT:           return "COUNT";
			}

			LK_CORE_VERIFY(false, "Enum::ToString(const ETheme) failed for: {}", static_cast<int>(Theme));
			return nullptr;
		}
	}

}
