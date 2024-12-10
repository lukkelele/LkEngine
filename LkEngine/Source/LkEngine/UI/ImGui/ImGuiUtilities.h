#pragma once

#include <format>
#include <string>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>


#define LK_UI_ENTITY_ADD_PROPERTY(_PROPERTY) _PROPERTY

namespace LkEngine::UI {

    bool UpdateWindowManualResize(ImGuiWindow* imgui_window, ImVec2& new_size, ImVec2& new_pos);

}


#if 0

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
        return std::format_to(Context.out(), "({:2.f}, {:.2f}, {:.2f}, {:.2f})", Input.x, Input.y, Input.z, Input.w);
    }
};

#endif
