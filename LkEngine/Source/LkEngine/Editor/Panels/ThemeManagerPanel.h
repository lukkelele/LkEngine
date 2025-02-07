#pragma once

#include "LkEngine/Renderer/UI/Panel.h"
//#include "LkEngine/Core/LObject/Object.h"
//#include "LkEngine/Core/LObject/ObjectPtr.h"
//#include <imgui/imgui.h> /* REMOVE (?) */


namespace LkEngine {

	/**
	 * ETheme
	 *
	 *  Type of theme.
	 */
	enum class ETheme
	{
		Dark = 0,
		Light,
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
		
		virtual void SerializeToYaml(YAML::Emitter& Out) const override;
		virtual void DeserializeFromYaml(const YAML::Node& Data) override;

		static ETheme GetTheme() { return CurrentTheme; }

		template<ETheme Theme>
		static void SetTheme()
		{
			LK_CORE_ASSERT(false);
		}

		template<>
		static void SetTheme<ETheme::Dark>()
		{
			CurrentTheme = ETheme::Dark;

			ImGui::StyleColorsDark();
			auto& Colors = ImGui::GetStyle().Colors;

			/* Window background. */
			Colors[ImGuiCol_WindowBg]		= ImVec4(0.10f, 0.105f, 0.110f, 1.0f);

			/* Headers. */
			Colors[ImGuiCol_Header]			= ImVec4(0.20f, 0.205f,  0.210f, 1.0f);
			Colors[ImGuiCol_HeaderHovered]	= ImVec4(0.30f, 0.305f,  0.310f, 1.0f);
			Colors[ImGuiCol_HeaderActive]	= ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

			/* Buttons. */
			Colors[ImGuiCol_Button]			= ImVec4(0.20f, 0.205f, 0.210f, 1.0f);
			Colors[ImGuiCol_ButtonHovered]	= ImVec4(0.30f, 0.305f, 0.310f, 1.0f);
			Colors[ImGuiCol_ButtonActive]	= ImVec4(0.15f, 0.150f, 0.151f, 1.0f);

			/* Frame background. */
			Colors[ImGuiCol_FrameBg]		= ImVec4(0.20f, 0.2050f, 0.210f, 1.0f);
			Colors[ImGuiCol_FrameBgHovered]	= ImVec4(0.30f, 0.3050f, 0.310f, 1.0f);
			Colors[ImGuiCol_FrameBgActive]	= ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

			/* Tabs. */
			Colors[ImGuiCol_Tab]				= ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
			Colors[ImGuiCol_TabHovered]			= ImVec4(0.38f, 0.3805f, 0.381f, 1.0f);
			Colors[ImGuiCol_TabActive]			= ImVec4(0.28f, 0.2805f, 0.281f, 1.0f);
			Colors[ImGuiCol_TabUnfocused]		= ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
			Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.2050f, 0.210f, 1.0f);

			/* Title. */
			Colors[ImGuiCol_TitleBg]		  = ImVec4(0.150f, 0.1505f, 0.151f, 1.0f);
			Colors[ImGuiCol_TitleBgActive]	  = ImVec4(0.150f, 0.1505f, 0.151f, 1.0f);
			Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.150f, 0.1505f, 0.151f, 1.0f);

			/* Scrollbar. */
			Colors[ImGuiCol_ScrollbarBg]		  = ImVec4(0.020f, 0.020f, 0.020f, 0.53f);
			Colors[ImGuiCol_ScrollbarGrab]		  = ImVec4(0.310f, 0.310f, 0.310f, 1.0f);
			Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.410f, 0.410f, 0.410f, 1.0f);
			Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.510f, 0.510f, 0.510f, 1.0f);

			/* Checkmark. */
			Colors[ImGuiCol_CheckMark] = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);

			/* Slider. */
			Colors[ImGuiCol_SliderGrab]		  = ImVec4(0.51f, 0.51f, 0.51f, 0.70f);
			Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.66f, 0.66f, 0.66f, 1.0f);
		}

		template<>
		static void SetTheme<ETheme::Light>()
		{
			CurrentTheme = ETheme::Light;

			ImGui::StyleColorsLight();
			auto& Colors = ImGui::GetStyle().Colors;

		#if 0
			/* Window background. */
			Colors[ImGuiCol_WindowBg]		= ImVec4(0.10f, 0.105f, 0.110f, 1.0f);

			/* Headers. */
			Colors[ImGuiCol_Header]			= ImVec4(0.20f, 0.205f,  0.210f, 1.0f);
			Colors[ImGuiCol_HeaderHovered]	= ImVec4(0.30f, 0.305f,  0.310f, 1.0f);
			Colors[ImGuiCol_HeaderActive]	= ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

			/* Buttons. */
			Colors[ImGuiCol_Button]			= ImVec4(0.20f, 0.205f, 0.210f, 1.0f);
			Colors[ImGuiCol_ButtonHovered]	= ImVec4(0.30f, 0.305f, 0.310f, 1.0f);
			Colors[ImGuiCol_ButtonActive]	= ImVec4(0.15f, 0.150f, 0.151f, 1.0f);

			/* Frame background. */
			Colors[ImGuiCol_FrameBg]		= ImVec4(0.20f, 0.2050f, 0.210f, 1.0f);
			Colors[ImGuiCol_FrameBgHovered]	= ImVec4(0.30f, 0.3050f, 0.310f, 1.0f);
			Colors[ImGuiCol_FrameBgActive]	= ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

			/* Tabs. */
			Colors[ImGuiCol_Tab]				= ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
			Colors[ImGuiCol_TabHovered]			= ImVec4(0.38f, 0.3805f, 0.381f, 1.0f);
			Colors[ImGuiCol_TabActive]			= ImVec4(0.28f, 0.2805f, 0.281f, 1.0f);
			Colors[ImGuiCol_TabUnfocused]		= ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
			Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.2050f, 0.210f, 1.0f);

			/* Title. */
			Colors[ImGuiCol_TitleBg]		  = ImVec4(0.150f, 0.1505f, 0.151f, 1.0f);
			Colors[ImGuiCol_TitleBgActive]	  = ImVec4(0.150f, 0.1505f, 0.151f, 1.0f);
			Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.150f, 0.1505f, 0.151f, 1.0f);

			/* Scrollbar. */
			Colors[ImGuiCol_ScrollbarBg]		  = ImVec4(0.020f, 0.020f, 0.020f, 0.53f);
			Colors[ImGuiCol_ScrollbarGrab]		  = ImVec4(0.310f, 0.310f, 0.310f, 1.0f);
			Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.410f, 0.410f, 0.410f, 1.0f);
			Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.510f, 0.510f, 0.510f, 1.0f);

			/* Checkmark. */
			Colors[ImGuiCol_CheckMark] = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);

			/* Slider. */
			Colors[ImGuiCol_SliderGrab]		  = ImVec4(0.51f, 0.51f, 0.51f, 0.70f);
			Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.66f, 0.66f, 0.66f, 1.0f);
		#endif
		}

	private:
		inline static ETheme CurrentTheme = ETheme::Dark;

		LPANEL(LThemeManagerPanel);
	};


	namespace Enum 		
	{
		static constexpr const char* ToString(const ETheme Theme)
		{
			switch (Theme)
			{
				case ETheme::Dark:  return "Dark";
				case ETheme::Light: return "Light";
				case ETheme::COUNT: return "COUNT";
			}

			LK_CORE_VERIFY(false, "Enum::ToString(const ETheme) failed for: {}", static_cast<int>(Theme));
			return nullptr;
		}
	}

}
