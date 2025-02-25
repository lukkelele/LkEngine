#pragma once

#include "LkEngine/Core/Assert.h"
#include "LkEngine/Core/Log/Log.h"
#include "LkEngine/Core/LObject/Enum.h"

#include <imgui/imgui.h>


/**
 * Enum range: ImGuiCol
 */
LK_ENUM_RANGE_BY_COUNT(ImGuiCol_, ImGuiCol_COUNT);

namespace LkEngine::Enum
{
	static constexpr const char* ToString(const ImGuiCol_ ImGuiColor)
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
			default:                                 LK_CORE_ASSERT(false, "Unsupported ImGuiCol: {}", ImGuiColor);
		}

		return "";
	}
}

/**
 * Formatter: ImVec2
 */
template<>
struct std::formatter<ImVec2>
{
	template<typename ParseContext>
    constexpr auto parse(ParseContext& Context)
    {
        return Context.begin();
    }
	
	template<typename FormatContext>
    auto format(const ImVec2& Input, FormatContext& Context) const
    {
		return std::format_to(Context.out(), "({:.2f}, {:.2f})", Input.x, Input.y);
    }
};

/**
 * Formatter: ImVec4
 */
template<>
struct std::formatter<ImVec4>
{
	template<typename ParseContext>
    constexpr auto parse(ParseContext& Context)
    {
        return Context.begin();
    }
	
	template<typename FormatContext>
    auto format(const ImVec4& Input, FormatContext& Context) const
    {
        return std::format_to(Context.out(), "({:.2f}, {:.2f}, {:.2f}, {:.2f})", Input.x, Input.y, Input.z, Input.z);
    }
};

/**
 * Formatter: ImGuiCol_
 */
template<>
struct std::formatter<ImGuiCol_>
{
	template<typename ParseContext>
    constexpr auto parse(ParseContext& Context)
    {
        return Context.begin();
    }
	
	template<typename FormatContext>
    auto format(const ImGuiCol_& Input, FormatContext& Context) const
    {
		return std::format_to(Context.out(), "{}", static_cast<std::underlying_type_t<ImGuiCol_>>(Input));
    }
};

