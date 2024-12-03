#include "LKpch.h"
#include "NodeEditorUtils.h"

#include <imgui_internal.h>
#include <imgui-node-editor/imgui_node_editor_internal.h>


namespace LkEngine::UI::Nodes {

    void Icon(const ImVec2& size, IconType type, bool filled, const ImVec4& color/* = ImVec4(1, 1, 1, 1)*/, const ImVec4& innerColor/* = ImVec4(0, 0, 0, 0)*/)
    {
        if (ImGui::IsRectVisible(size))
        {
            auto cursorPos = ImGui::GetCursorScreenPos();
            auto drawList = ImGui::GetWindowDrawList();
            DrawIcon(drawList, cursorPos, cursorPos + size, type, filled, ImColor(color), ImColor(innerColor));
        }

        ImGui::Dummy(size);
    }

    void DrawIcon(ImDrawList* drawList, const ImVec2& a, const ImVec2& b, IconType type, bool filled, ImU32 color, ImU32 innerColor)
    {
        ImRect rect = ImRect(a, b);
        float rect_x = rect.Min.x;
        float rect_y = rect.Min.y;
        auto rect_w = rect.Max.x - rect.Min.x;
        auto rect_h = rect.Max.y - rect.Min.y;
        auto rect_center_x = (rect.Min.x + rect.Max.x) * 0.5f;
        auto rect_center_y = (rect.Min.y + rect.Max.y) * 0.5f;
        auto rect_center = ImVec2(rect_center_x, rect_center_y);
        const auto outline_scale = rect_w / 24.0f;
        const auto extra_segments = static_cast<int>(2 * outline_scale); // for full circle

        if (type == IconType::Flow)
        {
            const auto origin_scale = rect_w / 24.0f;

            const auto offset_x = 1.0f * origin_scale;
            const auto offset_y = 0.0f * origin_scale;
            const auto margin = (filled ? 2.0f : 2.0f) * origin_scale;
            const auto rounding = 0.1f * origin_scale;
            const auto tip_round = 0.7f; // percentage of triangle edge (for tip)
            //const auto edge_round = 0.7f; // percentage of triangle edge (for corner)
            const auto canvas = ImRect(
                rect.Min.x + margin + offset_x,
                rect.Min.y + margin + offset_y,
                rect.Max.x - margin + offset_x,
                rect.Max.y - margin + offset_y);
            const auto canvas_x = canvas.Min.x;
            const auto canvas_y = canvas.Min.y;
            const auto canvas_w = canvas.Max.x - canvas.Min.x;
            const auto canvas_h = canvas.Max.y - canvas.Min.y;

            const auto left = canvas_x + canvas_w * 0.5f * 0.3f;
            const auto right = canvas_x + canvas_w - canvas_w * 0.5f * 0.3f;
            const auto top = canvas_y + canvas_h * 0.5f * 0.2f;
            const auto bottom = canvas_y + canvas_h - canvas_h * 0.5f * 0.2f;
            const auto center_y = (top + bottom) * 0.5f;
            //const auto angle = AX_PI * 0.5f * 0.5f * 0.5f;

            const auto tip_top = ImVec2(canvas_x + canvas_w * 0.5f, top);
            const auto tip_right = ImVec2(right, center_y);
            const auto tip_bottom = ImVec2(canvas_x + canvas_w * 0.5f, bottom);

            drawList->PathLineTo(ImVec2(left, top) + ImVec2(0, rounding));
            drawList->PathBezierCubicCurveTo(
                ImVec2(left, top),
                ImVec2(left, top),
                ImVec2(left, top) + ImVec2(rounding, 0));
            drawList->PathLineTo(tip_top);
            drawList->PathLineTo(tip_top + (tip_right - tip_top) * tip_round);
            drawList->PathBezierCubicCurveTo(
                tip_right,
                tip_right,
                tip_bottom + (tip_right - tip_bottom) * tip_round);
            drawList->PathLineTo(tip_bottom);
            drawList->PathLineTo(ImVec2(left, bottom) + ImVec2(rounding, 0));
            drawList->PathBezierCubicCurveTo(
                ImVec2(left, bottom),
                ImVec2(left, bottom),
                ImVec2(left, bottom) - ImVec2(0, rounding));

            if (!filled)
            {
                if (innerColor & 0xFF000000)
                    drawList->AddConvexPolyFilled(drawList->_Path.Data, drawList->_Path.Size, innerColor);

                drawList->PathStroke(color, true, 2.0f * outline_scale);
            }
            else
                drawList->PathFillConvex(color);
        }
        else
        {
            auto triangleStart = rect_center_x + 0.32f * rect_w;

            auto rect_offset = -static_cast<int>(rect_w * 0.25f * 0.25f);

            rect.Min.x += rect_offset;
            rect.Max.x += rect_offset;
            rect_x += rect_offset;
            rect_center_x += rect_offset * 0.5f;
            rect_center.x += rect_offset * 0.5f;

            if (type == IconType::Circle)
            {
                const auto c = rect_center;

                if (!filled)
                {
                    const auto r = 0.5f * rect_w / 2.0f - 0.5f;

                    if (innerColor & 0xFF000000)
                        drawList->AddCircleFilled(c, r, innerColor, 12 + extra_segments);
                    drawList->AddCircle(c, r, color, 12 + extra_segments, 2.0f * outline_scale);
                }
                else
                {
                    drawList->AddCircleFilled(c, 0.5f * rect_w / 2.0f, color, 12 + extra_segments);
                }
            }

            if (type == IconType::Square)
            {
                if (filled)
                {
                    const auto r = 0.5f * rect_w / 2.0f;
                    const auto p0 = rect_center - ImVec2(r, r);
                    const auto p1 = rect_center + ImVec2(r, r);

#if IMGUI_VERSION_NUM > 18101
                    drawList->AddRectFilled(p0, p1, color, 0, ImDrawFlags_RoundCornersAll);
#else
                    drawList->AddRectFilled(p0, p1, color, 0, 15);
#endif
                }
                else
                {
                    const auto r = 0.5f * rect_w / 2.0f - 0.5f;
                    const auto p0 = rect_center - ImVec2(r, r);
                    const auto p1 = rect_center + ImVec2(r, r);

                    if (innerColor & 0xFF000000)
                    {
#if IMGUI_VERSION_NUM > 18101
                        drawList->AddRectFilled(p0, p1, innerColor, 0, ImDrawFlags_RoundCornersAll);
#else
                        drawList->AddRectFilled(p0, p1, innerColor, 0, 15);
#endif
                    }

#if IMGUI_VERSION_NUM > 18101
                    drawList->AddRect(p0, p1, color, 0, ImDrawFlags_RoundCornersAll, 2.0f * outline_scale);
#else
                    drawList->AddRect(p0, p1, color, 0, 15, 2.0f * outline_scale);
#endif
                }
            }

            if (type == IconType::Grid)
            {
                const auto r = 0.5f * rect_w / 2.0f;
                const auto w = ceilf(r / 3.0f);

                const auto baseTl = ImVec2(floorf(rect_center_x - w * 2.5f), floorf(rect_center_y - w * 2.5f));
                const auto baseBr = ImVec2(floorf(baseTl.x + w), floorf(baseTl.y + w));

                auto tl = baseTl;
                auto br = baseBr;
                for (int i = 0; i < 3; ++i)
                {
                    tl.x = baseTl.x;
                    br.x = baseBr.x;
                    drawList->AddRectFilled(tl, br, color);
                    tl.x += w * 2;
                    br.x += w * 2;
                    if (i != 1 || filled)
                        drawList->AddRectFilled(tl, br, color);
                    tl.x += w * 2;
                    br.x += w * 2;
                    drawList->AddRectFilled(tl, br, color);

                    tl.y += w * 2;
                    br.y += w * 2;
                }

                triangleStart = br.x + w + 1.0f / 24.0f * rect_w;
            }

            if (type == IconType::RoundSquare)
            {
                if (filled)
                {
                    const auto r = 0.5f * rect_w / 2.0f;
                    const auto cr = r * 0.5f;
                    const auto p0 = rect_center - ImVec2(r, r);
                    const auto p1 = rect_center + ImVec2(r, r);

#if IMGUI_VERSION_NUM > 18101
                    drawList->AddRectFilled(p0, p1, color, cr, ImDrawFlags_RoundCornersAll);
#else
                    drawList->AddRectFilled(p0, p1, color, cr, 15);
#endif
                }
                else
                {
                    const auto r = 0.5f * rect_w / 2.0f - 0.5f;
                    const auto cr = r * 0.5f;
                    const auto p0 = rect_center - ImVec2(r, r);
                    const auto p1 = rect_center + ImVec2(r, r);

                    if (innerColor & 0xFF000000)
                    {
#if IMGUI_VERSION_NUM > 18101
                        drawList->AddRectFilled(p0, p1, innerColor, cr, ImDrawFlags_RoundCornersAll);
#else
                        drawList->AddRectFilled(p0, p1, innerColor, cr, 15);
#endif
                    }

#if  IMGUI_VERSION_NUM > 18101
                    drawList->AddRect(p0, p1, color, cr, ImDrawFlags_RoundCornersAll, 2.0f * outline_scale);
#else
                    drawList->AddRect(p0, p1, color, cr, 15, 2.0f * outline_scale);
#endif
                }
            }
            else if (type == IconType::Diamond)
            {
                if (filled)
                {
                    const auto r = 0.607f * rect_w / 2.0f;
                    const auto c = rect_center;

                    drawList->PathLineTo(c + ImVec2(0, -r));
                    drawList->PathLineTo(c + ImVec2(r, 0));
                    drawList->PathLineTo(c + ImVec2(0, r));
                    drawList->PathLineTo(c + ImVec2(-r, 0));
                    drawList->PathFillConvex(color);
                }
                else
                {
                    const auto r = 0.607f * rect_w / 2.0f - 0.5f;
                    const auto c = rect_center;

                    drawList->PathLineTo(c + ImVec2(0, -r));
                    drawList->PathLineTo(c + ImVec2(r, 0));
                    drawList->PathLineTo(c + ImVec2(0, r));
                    drawList->PathLineTo(c + ImVec2(-r, 0));

                    if (innerColor & 0xFF000000)
                        drawList->AddConvexPolyFilled(drawList->_Path.Data, drawList->_Path.Size, innerColor);

                    drawList->PathStroke(color, true, 2.0f * outline_scale);
                }
            }
            else
            {
                const auto triangleTip = triangleStart + rect_w * (0.45f - 0.32f);

                drawList->AddTriangleFilled(
                    ImVec2(ceilf(triangleTip), rect_y + rect_h * 0.5f),
                    ImVec2(triangleStart, rect_center_y + 0.15f * rect_h),
                    ImVec2(triangleStart, rect_center_y - 0.15f * rect_h),
                    color);
            }
        }
    }



#if defined(LK_USE_MODDED_IMGUI)
namespace ImGui {

        //-----------------------------------------------------------------------------
        // [SECTION] STACK LAYOUT
        //-----------------------------------------------------------------------------
        typedef int ImGuiLayoutItemType;
    
        enum ImGuiLayoutItemType_
        {
            ImGuiLayoutItemType_Item,
            ImGuiLayoutItemType_Spring
        };
    
        struct ImGuiLayoutItem
        {
            ImGuiLayoutItemType     Type;               // Type of an item
            ImRect                  MeasuredBounds;
    
            float                   SpringWeight;       // Weight of a spring
            float                   SpringSpacing;      // Spring spacing
            float                   SpringSize;         // Calculated spring size
    
            float                   CurrentAlign;
            float                   CurrentAlignOffset;
    
            unsigned int            VertexIndexBegin;
            unsigned int            VertexIndexEnd;
    
            ImGuiLayoutItem(ImGuiLayoutItemType type)
            {
                Type = type;
                MeasuredBounds = ImRect(0, 0, 0, 0);    // FIXME: @thedmd are you sure the default ImRect value FLT_MAX,FLT_MAX,-FLT_MAX,-FLT_MAX aren't enough here?
                SpringWeight = 1.0f;
                SpringSpacing = -1.0f;
                SpringSize = 0.0f;
                CurrentAlign = 0.0f;
                CurrentAlignOffset = 0.0f;
                VertexIndexBegin = VertexIndexEnd = (ImDrawIdx)0;
            }
        };
    
        struct ImGuiLayout
        {
            ImGuiID                     Id;
            ImGuiLayoutType             Type;
            bool                        Live;
            ImVec2                      Size;               // Size passed to BeginLayout
            ImVec2                      CurrentSize;        // Bounds of layout known at the beginning the frame.
            ImVec2                      MinimumSize;        // Minimum possible size when springs are collapsed.
            ImVec2                      MeasuredSize;       // Measured size with springs expanded.
    
            ImVector<ImGuiLayoutItem>   Items;
            int                         CurrentItemIndex;
            int                         ParentItemIndex;
            ImGuiLayout* Parent;
            ImGuiLayout* FirstChild;
            ImGuiLayout* NextSibling;
            float                       Align;              // Current item alignment.
            float                       Indent;             // Indent used to align items in vertical layout.
            ImVec2                      StartPos;           // Initial cursor position when BeginLayout is called.
            ImVec2                      StartCursorMaxPos;  // Maximum cursor position when BeginLayout is called.
    
            ImGuiLayout(ImGuiID id, ImGuiLayoutType type)
            {
                Id = id;
                Type = type;
                Live = false;
                Size = CurrentSize = MinimumSize = MeasuredSize = ImVec2(0, 0);
                CurrentItemIndex = 0;
                ParentItemIndex = 0;
                Parent = FirstChild = NextSibling = NULL;
                Align = -1.0f;
                Indent = 0.0f;
                StartPos = ImVec2(0, 0);
                StartCursorMaxPos = ImVec2(0, 0);
            }
        };
    
    
        static ImGuiLayout* ImGui::FindLayout(ImGuiID id, ImGuiLayoutType type)
        {
            IM_ASSERT(type == ImGuiLayoutType_Horizontal || type == ImGuiLayoutType_Vertical);
    
            ImGuiWindow* window = GetCurrentWindow();
            ImGuiLayout* layout = (ImGuiLayout*)window->DC.Layouts.GetVoidPtr(id);
            if (!layout)
                return NULL;
    
            if (layout->Type != type)
            {
                layout->Type = type;
                layout->MinimumSize = ImVec2(0.0f, 0.0f);
                layout->Items.clear();
            }
    
            return layout;
        }
    
        static ImGuiLayout* ImGui::CreateNewLayout(ImGuiID id, ImGuiLayoutType type, ImVec2 size)
        {
            IM_ASSERT(type == ImGuiLayoutType_Horizontal || type == ImGuiLayoutType_Vertical);
    
            ImGuiWindow* window = GetCurrentWindow();
    
            ImGuiLayout* layout = IM_NEW(ImGuiLayout)(id, type);
            layout->Size = size;
    
            window->DC.Layouts.SetVoidPtr(id, layout);
    
            return layout;
        }
    
        static void ImGui::BeginLayout(ImGuiID id, ImGuiLayoutType type, ImVec2 size, float align)
        {
            ImGuiWindow* window = GetCurrentWindow();
    
            PushID(id);
    
            // Find or create
            ImGuiLayout* layout = FindLayout(id, type);
            if (!layout)
                layout = CreateNewLayout(id, type, size);
    
            layout->Live = true;
    
            PushLayout(layout);
    
            if (layout->Size.x != size.x || layout->Size.y != size.y)
                layout->Size = size;
    
            if (align < 0.0f)
                layout->Align = -1.0f;
            else
                layout->Align = ImClamp(align, 0.0f, 1.0f);
    
            // Start capture
            layout->CurrentItemIndex = 0;
    
            layout->CurrentSize.x = layout->Size.x > 0.0f ? layout->Size.x : layout->MinimumSize.x;
            layout->CurrentSize.y = layout->Size.y > 0.0f ? layout->Size.y : layout->MinimumSize.y;
    
            layout->StartPos = window->DC.CursorPos;
            layout->StartCursorMaxPos = window->DC.CursorMaxPos;
    
            if (type == ImGuiLayoutType_Vertical)
            {
                // Push empty item to recalculate cursor position.
                PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
                Dummy(ImVec2(0.0f, 0.0f));
                PopStyleVar();
    
                // Indent horizontal position to match edge of the layout.
                layout->Indent = layout->StartPos.x - window->DC.CursorPos.x;
                SignedIndent(layout->Indent);
            }
    
            BeginLayoutItem(*layout);
        }
    
        static void ImGui::EndLayout(ImGuiLayoutType type)
        {
            ImGuiWindow* window = GetCurrentWindow();
            IM_ASSERT(window->DC.CurrentLayout);
            IM_ASSERT(window->DC.CurrentLayout->Type == type);
            IM_UNUSED(type);
    
            ImGuiLayout* layout = window->DC.CurrentLayout;
    
            EndLayoutItem(*layout);
    
            if (layout->CurrentItemIndex < layout->Items.Size)
                layout->Items.resize(layout->CurrentItemIndex);
    
            if (layout->Type == ImGuiLayoutType_Vertical)
                SignedIndent(-layout->Indent);
    
            PopLayout(layout);
    
            const bool auto_width = layout->Size.x <= 0.0f;
            const bool auto_height = layout->Size.y <= 0.0f;
    
            ImVec2 new_size = layout->Size;
            if (auto_width)
                new_size.x = layout->CurrentSize.x;
            if (auto_height)
                new_size.y = layout->CurrentSize.y;
    
            ImVec2 new_minimum_size = CalculateLayoutSize(*layout, true);
    
            if (new_minimum_size.x != layout->MinimumSize.x || new_minimum_size.y != layout->MinimumSize.y)
            {
                layout->MinimumSize = new_minimum_size;
    
                // Shrink
                if (auto_width)
                    new_size.x = new_minimum_size.x;
                if (auto_height)
                    new_size.y = new_minimum_size.y;
            }
    
            if (!auto_width)
                new_size.x = layout->Size.x;
            if (!auto_height)
                new_size.y = layout->Size.y;
    
            layout->CurrentSize = new_size;
    
            ImVec2 measured_size = new_size;
            if ((auto_width || auto_height) && layout->Parent)
            {
                if (layout->Type == ImGuiLayoutType_Horizontal && auto_width && layout->Parent->CurrentSize.x > 0)
                    layout->CurrentSize.x = layout->Parent->CurrentSize.x;
                else if (layout->Type == ImGuiLayoutType_Vertical && auto_height && layout->Parent->CurrentSize.y > 0)
                    layout->CurrentSize.y = layout->Parent->CurrentSize.y;
    
                BalanceLayoutSprings(*layout);
    
                measured_size = layout->CurrentSize;
            }
    
            layout->CurrentSize = new_size;
    
            PopID();
    
            ImVec2 current_layout_item_max = ImVec2(0.0f, 0.0f);
            if (window->DC.CurrentLayoutItem)
                current_layout_item_max = ImMax(window->DC.CurrentLayoutItem->MeasuredBounds.Max, layout->StartPos + new_size);
    
            window->DC.CursorPos = layout->StartPos;
            window->DC.CursorMaxPos = layout->StartCursorMaxPos;
            ItemSize(new_size);
            ItemAdd(ImRect(layout->StartPos, layout->StartPos + measured_size), 0);
    
            if (window->DC.CurrentLayoutItem)
                window->DC.CurrentLayoutItem->MeasuredBounds.Max = current_layout_item_max;
    
            if (layout->Parent == NULL)
                BalanceChildLayouts(*layout);
    
            //window->DrawList->AddRect(layout->StartPos, layout->StartPos + measured_size, IM_COL32(0,255,0,255));           // [DEBUG]
            //window->DrawList->AddRect(window->DC.LastItemRect.Min, window->DC.LastItemRect.Max, IM_COL32(255,255,0,255));   // [DEBUG]
        }
    
        static ImVec2 ImGui::CalculateLayoutSize(ImGuiLayout& layout, bool collapse_springs)
        {
            ImVec2 bounds = ImVec2(0.0f, 0.0f);
    
            if (layout.Type == ImGuiLayoutType_Vertical)
            {
                for (int i = 0; i < layout.Items.Size; i++)
                {
                    ImGuiLayoutItem& item = layout.Items[i];
                    ImVec2 item_size = item.MeasuredBounds.GetSize();
    
                    if (item.Type == ImGuiLayoutItemType_Item)
                    {
                        bounds.x = ImMax(bounds.x, item_size.x);
                        bounds.y += item_size.y;
                    }
                    else
                    {
                        bounds.y += ImFloor(item.SpringSpacing);
    
                        if (!collapse_springs)
                            bounds.y += item.SpringSize;
                    }
                }
            }
            else
            {
                for (int i = 0; i < layout.Items.Size; i++)
                {
                    ImGuiLayoutItem& item = layout.Items[i];
                    ImVec2 item_size = item.MeasuredBounds.GetSize();
    
                    if (item.Type == ImGuiLayoutItemType_Item)
                    {
                        bounds.x += item_size.x;
                        bounds.y = ImMax(bounds.y, item_size.y);
                    }
                    else
                    {
                        bounds.x += ImFloor(item.SpringSpacing);
    
                        if (!collapse_springs)
                            bounds.x += item.SpringSize;
                    }
                }
            }
    
            return bounds;
        }
    
        static void ImGui::PushLayout(ImGuiLayout* layout)
        {
            ImGuiWindow* window = GetCurrentWindow();
    
            if (layout)
            {
                layout->Parent = window->DC.CurrentLayout;
                if (layout->Parent != NULL)
                    layout->ParentItemIndex = layout->Parent->CurrentItemIndex;
                if (window->DC.CurrentLayout)
                {
                    layout->NextSibling = window->DC.CurrentLayout->FirstChild;
                    layout->FirstChild = NULL;
                    window->DC.CurrentLayout->FirstChild = layout;
                }
                else
                {
                    layout->NextSibling = NULL;
                    layout->FirstChild = NULL;
                }
            }
    
            window->DC.LayoutStack.push_back(layout);
            window->DC.CurrentLayout = layout;
            window->DC.CurrentLayoutItem = NULL;
        }
    
        static void ImGui::PopLayout(ImGuiLayout* layout)
        {
            ImGuiWindow* window = GetCurrentWindow();
    
            IM_ASSERT(!window->DC.LayoutStack.empty());
            IM_ASSERT(window->DC.LayoutStack.back() == layout);
            IM_UNUSED(layout);
    
            window->DC.LayoutStack.pop_back();
    
            if (!window->DC.LayoutStack.empty())
            {
                window->DC.CurrentLayout = window->DC.LayoutStack.back();
                window->DC.CurrentLayoutItem = &window->DC.CurrentLayout->Items[window->DC.CurrentLayout->CurrentItemIndex];
            }
            else
            {
                window->DC.CurrentLayout = NULL;
                window->DC.CurrentLayoutItem = NULL;
            }
        }
    
        static void ImGui::BalanceLayoutSprings(ImGuiLayout& layout)
        {
            // Accumulate amount of occupied space and springs weights
            float total_spring_weight = 0.0f;
    
            int last_spring_item_index = -1;
            for (int i = 0; i < layout.Items.Size; i++)
            {
                ImGuiLayoutItem& item = layout.Items[i];
                if (item.Type == ImGuiLayoutItemType_Spring)
                {
                    total_spring_weight += item.SpringWeight;
                    last_spring_item_index = i;
                }
            }
    
            // Determine occupied space and available space depending on layout type
            const bool  is_horizontal = (layout.Type == ImGuiLayoutType_Horizontal);
            const bool  is_auto_sized = ((is_horizontal ? layout.Size.x : layout.Size.y) <= 0.0f) && (layout.Parent == NULL);
            const float occupied_space = is_horizontal ? layout.MinimumSize.x : layout.MinimumSize.y;
            const float available_space = is_auto_sized ? occupied_space : (is_horizontal ? layout.CurrentSize.x : layout.CurrentSize.y);
            const float free_space = ImMax(available_space - occupied_space, 0.0f);
    
            float span_start = 0.0f;
            float current_weight = 0.0f;
            for (int i = 0; i < layout.Items.Size; i++)
            {
                ImGuiLayoutItem& item = layout.Items[i];
                if (item.Type != ImGuiLayoutItemType_Spring)
                    continue;
    
                float last_spring_size = item.SpringSize;
    
                if (free_space > 0.0f && total_spring_weight > 0.0f)
                {
                    float next_weight = current_weight + item.SpringWeight;
                    float span_end = ImFloor((i == last_spring_item_index) ? free_space : (free_space * next_weight / total_spring_weight));
                    float spring_size = span_end - span_start;
                    item.SpringSize = spring_size;
                    span_start = span_end;
                    current_weight = next_weight;
                }
                else
                {
                    item.SpringSize = 0.0f;
                }
    
                // If spring changed its size, fix positioning of following items to avoid one frame visual bugs.
                if (last_spring_size != item.SpringSize)
                {
                    float difference = item.SpringSize - last_spring_size;
    
                    ImVec2 offset = is_horizontal ? ImVec2(difference, 0.0f) : ImVec2(0.0f, difference);
    
                    item.MeasuredBounds.Max += offset;
    
                    for (int j = i + 1; j < layout.Items.Size; j++)
                    {
                        ImGuiLayoutItem& translated_item = layout.Items[j];
    
                        TranslateLayoutItem(translated_item, offset);
    
                        translated_item.MeasuredBounds.Min += offset;
                        translated_item.MeasuredBounds.Max += offset;
                    }
                }
            }
        }
    
        static ImVec2 ImGui::BalanceLayoutItemAlignment(ImGuiLayout& layout, ImGuiLayoutItem& item)
        {
            // Fixup item alignment if necessary.
            ImVec2 position_correction = ImVec2(0.0f, 0.0f);
            if (item.CurrentAlign > 0.0f)
            {
                float item_align_offset = CalculateLayoutItemAlignmentOffset(layout, item);
                if (item.CurrentAlignOffset != item_align_offset)
                {
                    float offset = item_align_offset - item.CurrentAlignOffset;
    
                    if (layout.Type == ImGuiLayoutType_Horizontal)
                        position_correction.y = offset;
                    else
                        position_correction.x = offset;
    
                    TranslateLayoutItem(item, position_correction);
    
                    item.CurrentAlignOffset = item_align_offset;
                }
            }
    
            return position_correction;
        }
    
        static void ImGui::BalanceLayoutItemsAlignment(ImGuiLayout& layout)
        {
            for (int i = 0; i < layout.Items.Size; ++i)
            {
                ImGuiLayoutItem& item = layout.Items[i];
                BalanceLayoutItemAlignment(layout, item);
            }
        }
    
        static bool HasAnyNonZeroSpring(ImGuiLayout& layout)
        {
            for (int i = 0; i < layout.Items.Size; ++i)
            {
                ImGuiLayoutItem& item = layout.Items[i];
                if (item.Type != ImGuiLayoutItemType_Spring)
                    continue;
                if (item.SpringWeight > 0)
                    return true;
            }
            return false;
        }
    
        static void ImGui::BalanceChildLayouts(ImGuiLayout& layout)
        {
            for (ImGuiLayout* child = layout.FirstChild; child != NULL; child = child->NextSibling)
            {
                //ImVec2 child_layout_size = child->CurrentSize;
    
                // Propagate layout size down to child layouts.
                //
                // TODO: Distribution assume inner layout is only
                //       element inside parent item and assigns
                //       all available space to it.
                //
                //       Investigate how to split space between
                //       adjacent layouts.
                //
                //       Investigate how to measure non-layout items
                //       to treat them as fixed size blocks.
                //
                if (child->Type == ImGuiLayoutType_Horizontal && child->Size.x <= 0.0f)
                    child->CurrentSize.x = layout.CurrentSize.x;
                else if (child->Type == ImGuiLayoutType_Vertical && child->Size.y <= 0.0f)
                    child->CurrentSize.y = layout.CurrentSize.y;
    
                BalanceChildLayouts(*child);
    
                //child->CurrentSize = child_layout_size;
    
                if (HasAnyNonZeroSpring(*child))
                {
                    // Expand item measured bounds to make alignment correct.
                    ImGuiLayoutItem& item = layout.Items[child->ParentItemIndex];
    
                    if (child->Type == ImGuiLayoutType_Horizontal && child->Size.x <= 0.0f)
                        item.MeasuredBounds.Max.x = ImMax(item.MeasuredBounds.Max.x, item.MeasuredBounds.Min.x + layout.CurrentSize.x);
                    else if (child->Type == ImGuiLayoutType_Vertical && child->Size.y <= 0.0f)
                        item.MeasuredBounds.Max.y = ImMax(item.MeasuredBounds.Max.y, item.MeasuredBounds.Min.y + layout.CurrentSize.y);
                }
            }
    
            BalanceLayoutSprings(layout);
            BalanceLayoutItemsAlignment(layout);
        }
    
        static ImGuiLayoutItem* ImGui::GenerateLayoutItem(ImGuiLayout& layout, ImGuiLayoutItemType type)
        {
            ImGuiContext& g = *GImGui;
            IM_ASSERT(layout.CurrentItemIndex <= layout.Items.Size);
    
            if (layout.CurrentItemIndex < layout.Items.Size)
            {
                ImGuiLayoutItem& item = layout.Items[layout.CurrentItemIndex];
                if (item.Type != type)
                    item = ImGuiLayoutItem(type);
            }
            else
            {
                layout.Items.push_back(ImGuiLayoutItem(type));
            }
    
            g.CurrentWindow->DC.CurrentLayoutItem = &layout.Items[layout.CurrentItemIndex];
    
            return &layout.Items[layout.CurrentItemIndex];
        }
    
        // Calculate how many pixels from top/left layout edge item need to be moved to match
        // layout alignment.
        static float ImGui::CalculateLayoutItemAlignmentOffset(ImGuiLayout& layout, ImGuiLayoutItem& item)
        {
            if (item.CurrentAlign <= 0.0f)
                return 0.0f;
    
            ImVec2 item_size = item.MeasuredBounds.GetSize();
    
            float layout_extent = (layout.Type == ImGuiLayoutType_Horizontal) ? layout.CurrentSize.y : layout.CurrentSize.x;
            float item_extent = (layout.Type == ImGuiLayoutType_Horizontal) ? item_size.y : item_size.x;
    
            if (item_extent <= 0/* || layout_extent <= item_extent*/)
                return 0.0f;
    
            float align_offset = ImFloor(item.CurrentAlign * (layout_extent - item_extent));
    
            return align_offset;
        }
    
        static void ImGui::TranslateLayoutItem(ImGuiLayoutItem& item, const ImVec2& offset)
        {
            if ((offset.x == 0.0f && offset.y == 0.0f) || (item.VertexIndexBegin == item.VertexIndexEnd))
                return;
    
            //IMGUI_DEBUG_LOG("TranslateLayoutItem by %f,%f\n", offset.x, offset.y);
            ImDrawList* draw_list = GetWindowDrawList();
    
            ImDrawVert* begin = draw_list->VtxBuffer.Data + item.VertexIndexBegin;
            ImDrawVert* end = draw_list->VtxBuffer.Data + item.VertexIndexEnd;
    
            for (ImDrawVert* vtx = begin; vtx < end; ++vtx)
            {
                vtx->pos.x += offset.x;
                vtx->pos.y += offset.y;
            }
        }
    
        static void ImGui::SignedIndent(float indent)
        {
            if (indent > 0.0f)
                Indent(indent);
            else if (indent < 0.0f)
                Unindent(-indent);
        }
    
        static void ImGui::BeginLayoutItem(ImGuiLayout& layout)
        {
            ImGuiContext& g = *GImGui;
            ImGuiWindow* window = g.CurrentWindow;
            ImGuiLayoutItem& item = *GenerateLayoutItem(layout, ImGuiLayoutItemType_Item);
    
            item.CurrentAlign = layout.Align;
            if (item.CurrentAlign < 0.0f)
                item.CurrentAlign = ImClamp(g.Style.LayoutAlign, 0.0f, 1.0f);
    
            // Align item according to data from previous frame.
            // If layout changes in current frame alignment will
            // be corrected in EndLayout() to it visualy coherent.
            item.CurrentAlignOffset = CalculateLayoutItemAlignmentOffset(layout, item);
            if (item.CurrentAlign > 0.0f)
            {
                if (layout.Type == ImGuiLayoutType_Horizontal)
                {
                    window->DC.CursorPos.y += item.CurrentAlignOffset;
                }
                else
                {
                    float new_position = window->DC.CursorPos.x + item.CurrentAlignOffset;
    
                    // Make placement behave like in horizontal case when next
                    // widget is placed at very same Y position. This indent
                    // make sure for vertical layout placed widgets has same X position.
                    SignedIndent(item.CurrentAlignOffset);
    
                    window->DC.CursorPos.x = new_position;
                }
            }
    
            item.MeasuredBounds.Min = item.MeasuredBounds.Max = window->DC.CursorPos;
            item.VertexIndexBegin = item.VertexIndexEnd = window->DrawList->_VtxCurrentIdx;
        }
    
        static void ImGui::EndLayoutItem(ImGuiLayout& layout)
        {
            ImGuiContext& g = *GImGui;
            ImGuiWindow* window = g.CurrentWindow;
            IM_ASSERT(layout.CurrentItemIndex < layout.Items.Size);
    
            ImGuiLayoutItem& item = layout.Items[layout.CurrentItemIndex];
    
            ImDrawList* draw_list = window->DrawList;
            item.VertexIndexEnd = draw_list->_VtxCurrentIdx;
    
            if (item.CurrentAlign > 0.0f && layout.Type == ImGuiLayoutType_Vertical)
                SignedIndent(-item.CurrentAlignOffset);
    
            // Fixup item alignment in case item size changed in current frame.
            ImVec2 position_correction = BalanceLayoutItemAlignment(layout, item);
    
            item.MeasuredBounds.Min += position_correction;
            item.MeasuredBounds.Max += position_correction;
    
            if (layout.Type == ImGuiLayoutType_Horizontal)
                window->DC.CursorPos.y = layout.StartPos.y;
            else
                window->DC.CursorPos.x = layout.StartPos.x;
    
            layout.CurrentItemIndex++;
        }
    
        static void ImGui::AddLayoutSpring(ImGuiLayout& layout, float weight, float spacing)
        {
            ImGuiContext& g = *GImGui;
            ImGuiWindow* window = g.CurrentWindow;
            ImGuiLayoutItem* previous_item = &layout.Items[layout.CurrentItemIndex];
    
            // Undo item padding, spring should consume all space between items.
            if (layout.Type == ImGuiLayoutType_Horizontal)
                window->DC.CursorPos.x = previous_item->MeasuredBounds.Max.x;
            else
                window->DC.CursorPos.y = previous_item->MeasuredBounds.Max.y;
    
            previous_item = NULL; // may be invalid after call to GenerateLayoutItem()
    
            EndLayoutItem(layout);
    
            ImGuiLayoutItem* spring_item = GenerateLayoutItem(layout, ImGuiLayoutItemType_Spring);
    
            spring_item->MeasuredBounds.Min = spring_item->MeasuredBounds.Max = window->DC.CursorPos;
    
            if (weight < 0.0f)
                weight = 0.0f;
    
            if (spring_item->SpringWeight != weight)
                spring_item->SpringWeight = weight;
    
            if (spacing < 0.0f)
            {
                ImVec2 style_spacing = g.Style.ItemSpacing;
                if (layout.Type == ImGuiLayoutType_Horizontal)
                    spacing = style_spacing.x;
                else
                    spacing = style_spacing.y;
            }
    
            if (spring_item->SpringSpacing != spacing)
                spring_item->SpringSpacing = spacing;
    
            if (spring_item->SpringSize > 0.0f || spacing > 0.0f)
            {
                ImVec2 spring_size, spring_spacing;
                if (layout.Type == ImGuiLayoutType_Horizontal)
                {
                    spring_spacing = ImVec2(0.0f, g.Style.ItemSpacing.y);
                    spring_size = ImVec2(spacing + spring_item->SpringSize, layout.CurrentSize.y);
                }
                else
                {
                    spring_spacing = ImVec2(g.Style.ItemSpacing.x, 0.0f);
                    spring_size = ImVec2(layout.CurrentSize.x, spacing + spring_item->SpringSize);
                }
    
                PushStyleVar(ImGuiStyleVar_ItemSpacing, ImFloor(spring_spacing));
                Dummy(ImFloor(spring_size));
                PopStyleVar();
            }
    
            layout.CurrentItemIndex++;
    
            BeginLayoutItem(layout);
        }
    
        void ImGui::BeginHorizontal(const char* str_id, const ImVec2& size/* = ImVec2(0, 0)*/, float align/* = -1*/)
        {
            ImGuiWindow* window = GetCurrentWindow();
            BeginLayout(window->GetID(str_id), ImGuiLayoutType_Horizontal, size, align);
        }
    
        void ImGui::BeginHorizontal(const void* ptr_id, const ImVec2& size/* = ImVec2(0, 0)*/, float align/* = -1*/)
        {
            ImGuiWindow* window = GetCurrentWindow();
            BeginLayout(window->GetID(ptr_id), ImGuiLayoutType_Horizontal, size, align);
        }
    
        void ImGui::BeginHorizontal(int id, const ImVec2& size/* = ImVec2(0, 0)*/, float align/* = -1*/)
        {
            ImGuiWindow* window = GetCurrentWindow();
            BeginLayout(window->GetID((void*)(intptr_t)id), ImGuiLayoutType_Horizontal, size, align);
        }
    
        void ImGui::EndHorizontal()
        {
            EndLayout(ImGuiLayoutType_Horizontal);
        }
    
        void ImGui::BeginVertical(const char* str_id, const ImVec2& size/* = ImVec2(0, 0)*/, float align/* = -1*/)
        {
            ImGuiWindow* window = GetCurrentWindow();
            BeginLayout(window->GetID(str_id), ImGuiLayoutType_Vertical, size, align);
        }
    
        void ImGui::BeginVertical(const void* ptr_id, const ImVec2& size/* = ImVec2(0, 0)*/, float align/* = -1*/)
        {
            ImGuiWindow* window = GetCurrentWindow();
            BeginLayout(window->GetID(ptr_id), ImGuiLayoutType_Vertical, size, align);
        }
    
        void ImGui::BeginVertical(int id, const ImVec2& size/* = ImVec2(0, 0)*/, float align/* = -1*/)
        {
            ImGuiWindow* window = GetCurrentWindow();
            BeginLayout(window->GetID((void*)(intptr_t)id), ImGuiLayoutType_Vertical, size, align);
        }
    
        void ImGui::EndVertical()
        {
            EndLayout(ImGuiLayoutType_Vertical);
        }
    
        // Inserts spring separator in layout
        //      weight <= 0     : spring will always have zero size
        //      weight > 0      : power of current spring
        //      spacing < 0     : use default spacing if pos_x == 0, no spacing if pos_x != 0
        //      spacing >= 0    : enforce spacing amount
        void ImGui::Spring(float weight/* = 1.0f*/, float spacing/* = -1.0f*/)
        {
            ImGuiWindow* window = GetCurrentWindow();
            IM_ASSERT(window->DC.CurrentLayout);
    
            AddLayoutSpring(*window->DC.CurrentLayout, weight, spacing);
        }
    
        void ImGui::SuspendLayout()
        {
            PushLayout(NULL);
        }
    
        void ImGui::ResumeLayout()
        {
            ImGuiWindow* window = GetCurrentWindow();
            IM_ASSERT(!window->DC.CurrentLayout);
            IM_ASSERT(!window->DC.LayoutStack.empty());
            IM_UNUSED(window);
            PopLayout(NULL);
        }

    }
    #endif // LK_USE_MODDED_IMGUI

}
