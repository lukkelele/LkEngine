#pragma once

#include "LkEngine/Core/Core.h"
#include "LkEngine/Core/LObject/Enum.h"
#include "LkEngine/Core/Delegate/Delegate.h"
#include "LkEngine/Renderer/UI/UICore.h"

#include "LkEngine/Editor/EditorCore.h" /// FIXME: Refactor FEditorResources out of here.
#include "LkEngine/Renderer/UI/ImGui/LImGui.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>


namespace LkEngine::UI {

	class Widget
	{
	public:
		template<uint32_t BufSize = 256, float InputBarWidth = 220.0f, typename StringType>
		static bool SearchWidget(StringType& SearchStr, 
								 const char* Hint = "Search...", 
								 bool* GrabFocus = nullptr)
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 1.0f);

			bool Modified = false;
			bool Searching = false;

			const float AreaPosX = ImGui::GetCursorPosX();
			const float FramePaddingX = ImGui::GetStyle().FramePadding.x;
			const float FramePaddingY = ImGui::GetStyle().FramePadding.y;

			/* Set frame rounding and padding styles. */
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(28.0f, FramePaddingY));

			ImGui::SameLine();
			ImGui::SetNextItemAllowOverlap();
			ImGui::SetNextItemWidth(InputBarWidth);

			if constexpr (std::is_same_v<StringType, std::string>)
			{
				char SearchBuf[BufSize + 1]{};
				strncpy(SearchBuf, SearchStr.c_str(), BufSize);

				if (ImGui::InputText("##Search", SearchBuf, BufSize))
				{
					SearchStr = SearchBuf;
					Modified = true;
				}
				else if (ImGui::IsItemDeactivatedAfterEdit())
				{
					SearchStr = SearchBuf;
					Modified = true;
				}

				Searching = (SearchBuf[0] != '\0');
			}
			else
			{
				static_assert(std::is_same_v<decltype(&SearchStr[0]), char*>, "SearchStr parameter must be std::string& or char*");
				if (ImGui::InputText("##Search", SearchStr, BufSize))
				{
					Modified = true;
				}
				else if (ImGui::IsItemDeactivatedAfterEdit())
				{
					Modified = true;
				}

				Searching = SearchStr[0] != '\0';
			}

			LK_UI_DEBUG_BOUNDING_BOX(RGBA32::NiceBlue);

			const ImVec2 InputBarPosStart = ImGui::GetItemRectMin();
			const ImVec2 InputBarPosEnd = ImGui::GetItemRectMax();

			/* Size of the search text input bar. */
			const ImVec2 InputBarSize(
				InputBarPosEnd.x - InputBarPosStart.x, 
				InputBarPosEnd.y - InputBarPosStart.y
			);

			/* Handle focus grabbing. */
			if (GrabFocus && *GrabFocus)
			{
				if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) 
					&& !ImGui::IsAnyItemActive() 
					&& !ImGui::IsMouseClicked(0))
				{
					ImGui::SetKeyboardFocusHere(-1);
				}

				if (ImGui::IsItemFocused())
				{
					*GrabFocus = false;
					LK_CORE_DEBUG_TAG("SearchWidget", "Setting GrabFocus to false");
				}
			}

			/* Draw activity outline. */
			const ImRect ItemRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
			ImGui::RenderNavCursor(ItemRect, ImGui::GetID("##Search"));

			ImGui::SameLine(AreaPosX + 5.0f);

			/***********************************
			 * Search and clear icon. 
			 ***********************************/
			ImGui::BeginGroup();
			const ImVec2 IconSize(ImGui::GetTextLineHeight(), ImGui::GetTextLineHeight());
			LK_CORE_ASSERT((IconSize.x > 0) && (IconSize.y > 0));

			/* Search Icon. */
			{
				const float IconYOffset = FramePaddingY - 3.0f;
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + IconYOffset);
				UI::Image(
					FEditorResources::SearchIcon,    /* Texture ID. */
					IconSize,                        /* Image Size. */
					ImVec2(0.0f, 0.0f),              /* UV0. */
					ImVec2(1.0f, 1.0f),              /* UV1. */
					ImVec4(1.0f, 1.0f, 1.0f, 0.20f), /* Tint Color. */
					ImVec4(0.0f, 0.0f, 0.0f, 0.0f)   /* Border Color. */
				);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - IconYOffset);

				/* Display the hint text if not searching. */
				if (!Searching)
				{
					ImGui::SameLine();

					ImGui::SetCursorPosY(ImGui::GetCursorPosY() - FramePaddingY + 2.0f);
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.50f, 0.50f, 0.50f, 1.0f));
					ImGui::TextUnformatted(Hint);
					ImGui::PopStyleColor();
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + FramePaddingY - 2.0f);
				}
			}

			/* Draw the clear button if searching. */
			if (Searching)
			{
				static constexpr float SpacingX = 4.0f;
				const float LineHeight = ImGui::GetItemRectSize().y - (FramePaddingY / 2.0f);
				ImGui::SameLine(0.0f, InputBarSize.x - (2.0f * IconSize.x) - FramePaddingX - SpacingX);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + FramePaddingY);
				if (ImGui::InvisibleButton("##Clear", ImVec2(LineHeight, LineHeight)))
				{
					if constexpr (std::is_same_v<StringType, std::string>)
					{
						SearchStr.clear();
					}
					else
					{
						std::memset(SearchStr, 0, BufSize);
					}

					Modified = true;
				}
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - FramePaddingY);

				/* Change mouse cursor to arrow if hovering over clear button. */
				if (ImGui::IsItemHovered())
				{
					ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
					UI::SetTooltip(std::format("StartPos: ({}, {})  EndPos: ({}, {})  InputBarSize: ({}, {})", 
											   InputBarPosStart.x, InputBarPosStart.y, 
											   InputBarPosEnd.x, InputBarPosEnd.y,
											   InputBarSize.x, InputBarSize.y));
				}

				/* Clear Icon. */
				UI::DrawButtonImage(
					FEditorResources::ClearIcon,
					IM_COL32(150, 150, 150, 200),
					IM_COL32(170, 170, 170, 255),
					IM_COL32(160, 160, 160, 150),
					UI::RectExpanded(UI::GetItemRect(), -1.0f, -1.0f)
				);
			}

			LK_UI_DEBUG_BOUNDING_BOX(RGBA32::BrightGreen);

			/* Set the cursor to the end of the search bar (text input). */
			ImGui::SetCursorScreenPos(ImVec2(InputBarPosEnd.x, ImGui::GetCursorScreenPos().y));

			ImGui::EndGroup();

			/* Restore cursor position and styles. */
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 1.0f);
			ImGui::PopStyleVar(2);

			return Modified;
		}

		static bool OptionsButton();

	};

}
