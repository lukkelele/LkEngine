#pragma once

#include "LkEngine/Core/Assert.h"
#include "LkEngine/Core/Log/Log.h"
#include "LkEngine/Core/LObject/Enum.h"

#include "LkEngine/Serialization/YamlSerialization.h"

#include <imgui/imgui.h>

/**
 * Enum range: ImGuiCol
 */
LK_ENUM_RANGE_BY_COUNT(ImGuiCol_, ImGuiCol_COUNT);

namespace LkEngine::Enum
{
	static const char* ToString(const ImGuiCol_ ImGuiColor)
	{
		switch (ImGuiColor)
		{
			case ImGuiCol_Text:                      return "Text";
			case ImGuiCol_TextDisabled:              return "TextDisabled";
			case ImGuiCol_WindowBg:                  return "WindowBg";
			case ImGuiCol_ChildBg:                   return "ChildBg";
			case ImGuiCol_PopupBg:                   return "PopupBg";
			case ImGuiCol_Border:                    return "Border";
			case ImGuiCol_BorderShadow:              return "BorderShadow";
			case ImGuiCol_FrameBg:                   return "FrameBg";
			case ImGuiCol_FrameBgHovered:            return "FrameBgHovered";
			case ImGuiCol_FrameBgActive:             return "FrameBgActive";
			case ImGuiCol_TitleBg:                   return "TitleBg";
			case ImGuiCol_TitleBgActive:             return "TitleBgActive";
			case ImGuiCol_TitleBgCollapsed:          return "TitleBgCollapsed";
			case ImGuiCol_MenuBarBg:                 return "MenuBarBg";
			case ImGuiCol_ScrollbarBg:               return "ScrollbarBg";
			case ImGuiCol_ScrollbarGrab:             return "ScrollbarGrab";
			case ImGuiCol_ScrollbarGrabHovered:      return "ScrollbarGrabHovered";
			case ImGuiCol_ScrollbarGrabActive:       return "ScrollbarGrabActive";
			case ImGuiCol_CheckMark:                 return "CheckMark";
			case ImGuiCol_SliderGrab:                return "SliderGrab";
			case ImGuiCol_SliderGrabActive:          return "SliderGrabActive";
			case ImGuiCol_Button:                    return "Button";
			case ImGuiCol_ButtonHovered:             return "ButtonHovered";
			case ImGuiCol_ButtonActive:              return "ButtonActive";
			case ImGuiCol_Header:                    return "Header";
			case ImGuiCol_HeaderHovered:             return "HeaderHovered";
			case ImGuiCol_HeaderActive:              return "HeaderActive";
			case ImGuiCol_Separator:                 return "Separator";
			case ImGuiCol_SeparatorHovered:          return "SeparatorHovered";
			case ImGuiCol_SeparatorActive:           return "SeparatorActive";
			case ImGuiCol_ResizeGrip:                return "ResizeGrip";
			case ImGuiCol_ResizeGripHovered:         return "ResizeGripHovered";
			case ImGuiCol_ResizeGripActive:          return "ResizeGripActive";
			case ImGuiCol_TabHovered:                return "TabHovered";
			case ImGuiCol_Tab:                       return "Tab";
			case ImGuiCol_TabSelected:               return "TabSelected";
			case ImGuiCol_TabSelectedOverline:       return "TabSelectedOverline";
			case ImGuiCol_TabDimmed:                 return "TabDimmed";
			case ImGuiCol_TabDimmedSelected:         return "TabDimmedSelected";
			case ImGuiCol_TabDimmedSelectedOverline: return "TabDimmedSelectedOverline";
			case ImGuiCol_DockingPreview:            return "DockingPreview";
			case ImGuiCol_DockingEmptyBg:            return "DockingEmptyBg";
			case ImGuiCol_PlotLines:                 return "PlotLines";
			case ImGuiCol_PlotLinesHovered:          return "PlotLinesHovered";
			case ImGuiCol_PlotHistogram:             return "PlotHistogram";
			case ImGuiCol_PlotHistogramHovered:      return "PlotHistogramHovered";
			case ImGuiCol_TableHeaderBg:             return "TableHeaderBg";
			case ImGuiCol_TableBorderStrong:         return "TableBorderStrong";
			case ImGuiCol_TableBorderLight:          return "TableBorderLight";
			case ImGuiCol_TableRowBg:                return "TableRowBg";
			case ImGuiCol_TableRowBgAlt:             return "TableRowBgAlt";
			case ImGuiCol_TextLink:                  return "TextLink";
			case ImGuiCol_TextSelectedBg:            return "TextSelectedBg";
			case ImGuiCol_DragDropTarget:            return "DragDropTarget";
			case ImGuiCol_NavCursor:                 return "NavCursor";
			case ImGuiCol_NavWindowingHighlight:     return "NavWindowingHighlight";
			case ImGuiCol_NavWindowingDimBg:         return "NavWindowingDimBg";
			case ImGuiCol_ModalWindowDimBg:          return "ModalWindowDimBg";
			default:                                 LK_CORE_ASSERT(false, "Unsupported ImGuiCol: {}", std::underlying_type_t<ImGuiCol_>(ImGuiColor));
		}

		return "";
	}
}

/**
 * Log Formatter: ImVec2
 */
template<>
struct LK_FMT_LIB::formatter<ImVec2>
{
	template<typename ParseContext>
    constexpr auto parse(ParseContext& Context)
    {
        return Context.begin();
    }
	
	template<typename FormatContext>
    auto format(const ImVec2& Input, FormatContext& Context) const
    {
		return LK_FMT_LIB::format_to(Context.out(), "({:.2f}, {:.2f})", Input.x, Input.y);
    }
};

/**
 * Log Formatter: ImVec4
 */
template<>
struct LK_FMT_LIB::formatter<ImVec4>
{
	template<typename ParseContext>
    constexpr auto parse(ParseContext& Context)
    {
        return Context.begin();
    }
	
	template<typename FormatContext>
    auto format(const ImVec4& Input, FormatContext& Context) const
    {
        return LK_FMT_LIB::format_to(Context.out(), "({:.2f}, {:.2f}, {:.2f}, {:.2f})", Input.x, Input.y, Input.z, Input.z);
    }
};


namespace YAML
{
	/* Yaml Formatter: ImVec2 */
	template<>
	struct convert<ImVec2>
	{
		static Node encode(const ImVec2& Rhs)
		{
			Node YamlNode;
			YamlNode.push_back(Rhs.x);
			YamlNode.push_back(Rhs.y);
			return YamlNode;
		}

		static bool decode(const Node& YamlNode, ImVec2& Rhs)
		{
			if (!YamlNode.IsSequence() || (YamlNode.size() != 2))
			{
				return false;
			}

			Rhs.x = YamlNode[0].as<float>();
			Rhs.y = YamlNode[1].as<float>();
			return true;
		}
	};

	/* Yaml Formatter: ImVec4 */
	template<>
	struct convert<ImVec4>
	{
		static Node encode(const ImVec4& Rhs)
		{
			Node YamlNode;
			YamlNode.push_back(Rhs.x);
			YamlNode.push_back(Rhs.y);
			YamlNode.push_back(Rhs.z);
			YamlNode.push_back(Rhs.w);
			return YamlNode;
		}

		static bool decode(const Node& YamlNode, ImVec4& Rhs)
		{
			if (!YamlNode.IsSequence() || (YamlNode.size() != 4))
			{
				return false;
			}

			Rhs.x = YamlNode[0].as<float>();
			Rhs.y = YamlNode[1].as<float>();
			Rhs.z = YamlNode[2].as<float>();
			Rhs.w = YamlNode[3].as<float>();
			return true;
		}
	};

}

namespace LkEngine 
{
	/* Yaml bitwise operator: ImVec2 */
	inline YAML::Emitter& operator<<(YAML::Emitter& Out, const ImVec2& InVec2)
	{
		Out << YAML::Flow;
		Out << YAML::BeginSeq << InVec2.x << InVec2.y << YAML::EndSeq;
		return Out;
	}

	/* Yaml bitwise operator: ImVec4 */
	inline YAML::Emitter& operator<<(YAML::Emitter& Out, const ImVec4& InVec4)
	{
		Out << YAML::Flow;
		Out << YAML::BeginSeq << InVec4.x << InVec4.y << InVec4.z << InVec4.w << YAML::EndSeq;
		return Out;
	}
}
