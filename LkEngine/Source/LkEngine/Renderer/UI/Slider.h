#pragma once

#include "LkEngine/Core/LObject/Enum.h"
#include "LkEngine/Core/Delegate/Delegate.h"
#include "LkEngine/Renderer/UI/UICore.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>


namespace LkEngine::UI {

	namespace Draw 
	{
		FORCEINLINE bool DragFloat(const char* Label, float* Value, float ValueSpeed = 1.0f, float ValueMin = 0.0f, float ValueMax = 0.0f, const char* Format = "%.3f", ImGuiSliderFlags Flags = 0)
		{
			return ImGui::DragScalar(Label, ImGuiDataType_Float, Value, ValueSpeed, &ValueMin, &ValueMax, Format, Flags);
		}

		FORCEINLINE bool DragFloat3(const char* Label, float Vec[3], float VecSpeed = 1.0f, float VecMin = 0.0f, float VecMax = 0.0f, const char* Format = "%.3f", ImGuiSliderFlags Flags = 0)
		{
			const bool Changed = ImGui::DragFloat3(Label, Vec, VecSpeed, VecMin, VecMax, Format, Flags);
			DrawItemActivityOutline();
			return Changed;
		}

		FORCEINLINE bool SliderFloat3(const char* Label, float Vec[3], float VecMin, float VecMax, const char* Format = "%.3f", ImGuiSliderFlags Flags = 0) 
		{
			const bool Changed = ImGui::SliderFloat3(Label, Vec, VecMin, VecMax, Format, Flags);
			DrawItemActivityOutline();
			return Changed;
		}

		/**
		 * Vec3Control
		 * 
		 *  To be used in already existing tables.
		 */
		FORCEINLINE bool Vec3Control(const std::string& Label, 
									 glm::vec3& Values, 
									 const float ValueSpeed = 0.10f,
									 const float ResetValue = 0.0f, 
									 const float ValueMin = 0.0f,
									 const float ValueMax = 0.0f,
									 const float ColumnWidth = 100.0f, 
									 const char* Format = "%.2f",
									 uint32_t RenderMultiSelectAxes = 0)
		{
			bool Modified = false;
			bool ManuallyEdited = false;

		#if 0
			ImGui::TableSetColumnIndex(0);
			UI::ShiftCursor(17.0f, 7.0f);

			ImGui::Text(Label.c_str());
			UI::Draw::Underline(false, 0.0f, 2.0f);

			ImGui::TableSetColumnIndex(1);
			UI::ShiftCursor(7.0f, 0.0f);
		#else
			if (InTable())
			{
				ImGui::TableSetColumnIndex(0);
				UI::ShiftCursor(17.0f, 7.0f);

				ImGui::Text(Label.c_str());
				UI::Draw::Underline(false, 0.0f, 2.0f);

				ImGui::TableSetColumnIndex(1);
				UI::ShiftCursor(7.0f, 0.0f);
			}
			else
			{
				if (!Label.empty() && Label.at(0) != '#')
				{
					ImGui::Text(Label.c_str());
					/* TODO: Do PushStyle here instead. */
					ImGui::SameLine();
				}
			}
		#endif

			{
				static constexpr float SpacingX = 8.0f;
				UI::FScopedStyle ItemSpacing(ImGuiStyleVar_ItemSpacing, ImVec2(SpacingX, 0.0f));
				UI::FScopedStyle Padding(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 2.0f));
				{
					UI::FScopedColor Padding(ImGuiCol_Border, IM_COL32(0, 0, 0, 0));
					UI::FScopedColor Frame(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 0));

					ImGui::BeginChild(
						ImGui::GetID((Label + "Subwindow").c_str()),
						ImVec2((ImGui::GetContentRegionAvail().x - SpacingX), ImGui::GetFrameHeightWithSpacing() + 8.0f),
						ImGuiChildFlags_None,
						ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse /* Window Flags. */
					);
				}

				static constexpr float FramePadding = 3.0f;
				static constexpr float OutlineSpacing = 1.0f;
				const float LineHeight = GImGui->Font->FontSize + FramePadding * 2.0f;
				const ImVec2 ButtonSize = { LineHeight + 2.0f, LineHeight };
				const float InputItemWidth = (ImGui::GetContentRegionAvail().x - SpacingX) / 3.0f - ButtonSize.x;

				UI::ShiftCursor(0.0f, FramePadding);

				auto DrawControl = [&](const std::string& InLabel, 
									   float& InValue, 
									   const ImVec4& InColorNormal,
									   const ImVec4& InColorHover, 
									   const ImVec4& InColorPressed, 
									   bool RenderMultiSelect)
				{
					{
						UI::FScopedStyle ButtonFrame(ImGuiStyleVar_FramePadding, ImVec2(FramePadding, 0.0f));
						UI::FScopedStyle ButtonRounding(ImGuiStyleVar_FrameRounding, 1.0f);
						UI::FScopedColorStack ButtonColours(
							ImGuiCol_Button, InColorNormal, 
							ImGuiCol_ButtonHovered, InColorHover,
							ImGuiCol_ButtonActive, InColorPressed
						);

						if (ImGui::Button(InLabel.c_str(), ButtonSize))
						{
							InValue = ResetValue;
							Modified = true;
							LK_CORE_DEBUG("Pressed Button: {}", InLabel.c_str());
						}
					}

					ImGui::SameLine(0.0f, OutlineSpacing);
					ImGui::SetNextItemWidth(InputItemWidth);

					ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, RenderMultiSelect);
					const ImGuiID InputID = ImGui::GetID(("##" + InLabel).c_str());
					const bool WasTempInputActive = ImGui::TempInputIsActive(InputID);
					Modified |= ImGui::DragFloat(("##" + InLabel).c_str(), &InValue, ValueSpeed, ValueMin, ValueMax, Format, 0);

					if (ImGui::TempInputIsActive(InputID))
					{
						Modified = false;
					}

					ImGui::PopItemFlag();

					if (WasTempInputActive)
					{
						ManuallyEdited |= ImGui::IsItemDeactivatedAfterEdit();
					}
				};

				/* Draw X. */
				DrawControl(
					"X", 
					Values.x, 
					ImVec4(0.80f, 0.10f, 0.15f, 1.0f), /* Normal  */
					ImVec4(0.90f, 0.20f, 0.20f, 1.0f), /* Hover   */
					ImVec4(0.80f, 0.10f, 0.15f, 1.0f), /* Pressed */
					(RenderMultiSelectAxes & EVectorAxis::X)
				); 

				/* Draw Y. */
				ImGui::SameLine(0.0f, OutlineSpacing);
				DrawControl(
					"Y", 
					Values.y, 
					ImVec4(0.20f, 0.70f, 0.20f, 1.0f), 
					ImVec4(0.30f, 0.80f, 0.30f, 1.0f), 
					ImVec4(0.20f, 0.70f, 0.20f, 1.0f), 
					(RenderMultiSelectAxes & EVectorAxis::Y)
				);

				/* Draw Z. */
				ImGui::SameLine(0.0f, OutlineSpacing);
				DrawControl(
					"Z", 
					Values.z, 
					ImVec4(0.10f, 0.25f, 0.80f, 1.0f), 
					ImVec4(0.20f, 0.35f, 0.90f, 1.0f), 
					ImVec4(0.10f, 0.25f, 0.80f, 1.0f), 
					(RenderMultiSelectAxes & EVectorAxis::Z)
				);

				ImGui::EndChild();
			}

			/* TODO: Check if we are in a table before calling this. */
			if (InTable())
			{
				ImGui::TableNextRow();
			}

			return Modified || ManuallyEdited;
		}

		/**
		 * Vec3Control
		 * 
		 *  To be used in already existing tables.
		 */
		FORCEINLINE bool Vec3Control(const std::string& Label, 
									 glm::vec3& Values, 
									 bool& ManuallyEdited, 
									 const float ResetValue = 0.0f, 
									 const float ValueSpeed = 0.10f,
									 const float ValueMin = 0.0f,
									 const float ValueMax = 0.0f,
									 const float ColumnWidth = 100.0f, 
									 uint32_t RenderMultiSelectAxes = 0,
									 const char* Format = "%.2f")
		{
			bool Modified = false;

			ImGui::TableSetColumnIndex(0);
			UI::ShiftCursor(17.0f, 7.0f);

			ImGui::Text(Label.c_str());
			UI::Draw::Underline(false, 0.0f, 2.0f);

			ImGui::TableSetColumnIndex(1);
			UI::ShiftCursor(7.0f, 0.0f);
			{
				static constexpr float SpacingX = 8.0f;
				UI::FScopedStyle ItemSpacing(ImGuiStyleVar_ItemSpacing, ImVec2(SpacingX, 0.0f));
				UI::FScopedStyle Padding(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 2.0f));
				{
					UI::FScopedColor Padding(ImGuiCol_Border, IM_COL32(0, 0, 0, 0));
					UI::FScopedColor Frame(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 0));

					ImGui::BeginChild(
						ImGui::GetID((Label + "Subwindow").c_str()),
						ImVec2((ImGui::GetContentRegionAvail().x - SpacingX), ImGui::GetFrameHeightWithSpacing() + 8.0f),
						ImGuiChildFlags_None,
						ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse /* Window Flags. */
					);
				}

				//static constexpr float FramePadding = 2.0f;
				static constexpr float FramePadding = 3.0f;
				static constexpr float OutlineSpacing = 1.0f;
				const float LineHeight = GImGui->Font->FontSize + FramePadding * 2.0f;
				const ImVec2 ButtonSize = { LineHeight + 2.0f, LineHeight };
				const float InputItemWidth = (ImGui::GetContentRegionAvail().x - SpacingX) / 3.0f - ButtonSize.x;

				UI::ShiftCursor(0.0f, FramePadding);

				auto DrawControl = [&](const std::string& InLabel, 
									   float& InValue, 
									   const ImVec4& InColorNormal,
									   const ImVec4& InColorHover, 
									   const ImVec4& InColorPressed, 
									   bool RenderMultiSelect)
				{
					{
						UI::FScopedStyle ButtonFrame(ImGuiStyleVar_FramePadding, ImVec2(FramePadding, 0.0f));
						UI::FScopedStyle ButtonRounding(ImGuiStyleVar_FrameRounding, 1.0f);
						UI::FScopedColorStack ButtonColours(
							ImGuiCol_Button, InColorNormal, 
							ImGuiCol_ButtonHovered, InColorHover,
							ImGuiCol_ButtonActive, InColorPressed
						);

						//UI::ShiftCursorY(FramePadding);
						if (ImGui::Button(InLabel.c_str(), ButtonSize))
						{
							InValue = ResetValue;
							Modified = true;
							LK_CORE_DEBUG("Pressed Button: {}", InLabel.c_str());
						}
					}

					ImGui::SameLine(0.0f, OutlineSpacing);
					ImGui::SetNextItemWidth(InputItemWidth);
					//UI::ShiftCursorY(-FramePadding);

					ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, RenderMultiSelect);
					const ImGuiID InputID = ImGui::GetID(("##" + InLabel).c_str());
					const bool WasTempInputActive = ImGui::TempInputIsActive(InputID);
					Modified |= ImGui::DragFloat(("##" + InLabel).c_str(), &InValue, ValueSpeed, ValueMin, ValueMax, Format, 0);

					if (ImGui::TempInputIsActive(InputID))
					{
						Modified = false;
					}

					ImGui::PopItemFlag();

					if (WasTempInputActive)
					{
						ManuallyEdited |= ImGui::IsItemDeactivatedAfterEdit();
					}
				};

				/* Draw X. */
				DrawControl(
					"X", 
					Values.x, 
					ImVec4(0.80f, 0.10f, 0.15f, 1.0f), /* Normal  */
					ImVec4(0.90f, 0.20f, 0.20f, 1.0f), /* Hover   */
					ImVec4(0.80f, 0.10f, 0.15f, 1.0f), /* Pressed */
					(RenderMultiSelectAxes & EVectorAxis::X)
				); 

				/* Draw Y. */
				ImGui::SameLine(0.0f, OutlineSpacing);
				DrawControl(
					"Y", 
					Values.y, 
					ImVec4(0.20f, 0.70f, 0.20f, 1.0f), 
					ImVec4(0.30f, 0.80f, 0.30f, 1.0f), 
					ImVec4(0.20f, 0.70f, 0.20f, 1.0f), 
					(RenderMultiSelectAxes & EVectorAxis::Y)
				);

				/* Draw Z. */
				ImGui::SameLine(0.0f, OutlineSpacing);
				DrawControl(
					"Z", 
					Values.z, 
					ImVec4(0.10f, 0.25f, 0.80f, 1.0f), 
					ImVec4(0.20f, 0.35f, 0.90f, 1.0f), 
					ImVec4(0.10f, 0.25f, 0.80f, 1.0f), 
					(RenderMultiSelectAxes & EVectorAxis::Z)
				);

				ImGui::EndChild();
			}

			/* TODO: Check if we are in a table before calling this. */
			if (InTable())
			{
				ImGui::TableNextRow();
			}

			return Modified || ManuallyEdited;
		}
	}

}
