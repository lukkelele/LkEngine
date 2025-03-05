#pragma once

#include "LkEngine/Core/LObject/Enum.h"
#include "LkEngine/Core/Delegate/Delegate.h"
#include "LkEngine/Renderer/UI/UICore.h"

#include "LkEngine/Editor/EditorCore.h" /* UI::Debug */

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>


namespace LkEngine::UI {

	/**
	 * EVectorSemantic
	 *
	 *  Type of annotation used on vectors.
	 */
	enum class EVectorSemantic
	{
		XYZ,
		RGB,
		XYZW,
		RGBA,
	};
	LK_ENUM_CLASS(EVectorSemantic);

	namespace Slider {
		inline static constexpr float TablePaddingX = 17.0f; /* DragFloat X-padding. */
	}

	namespace Draw 
	{
		inline static constexpr int LABEL_BUFSIZE = 72;

		FORCEINLINE bool DragFloat(const char* Label, 
								   float* Value, 
								   float ValueSpeed = 1.0f, 
								   float ValueMin = 0.0f, 
								   float ValueMax = 0.0f, 
								   const char* Format = "%.3f", 
								   ImGuiSliderFlags Flags = 0)
		{
			const int LabelSize = std::strlen(Label);

			std::array<char, LABEL_BUFSIZE> LabelBuf{};
			std::snprintf(LabelBuf.data(), LabelBuf.size(), "##%s", Label);

			if (InTable())
			{
				if ((LabelSize > 0) && (Label[0] != '#'))
				{
					ImGui::TableSetColumnIndex(0);
					UI::ShiftCursor(Slider::TablePaddingX, 7.0f);

					ImGui::Text(Label);
					UI::Draw::Underline(false, 0.0f, 2.0f);

				}

				ImGui::TableSetColumnIndex(1);
				UI::ShiftCursor(7.0f, 0.0f);
			}
			else
			{
				if ((LabelSize > 0) && (Label[0] != '#'))
				{
					ImGui::Text(Label);
					/* TODO: Do PushStyle here instead. */
					ImGui::SameLine();
				}
			}

			const bool Dragged = ImGui::DragScalar(LabelBuf.data(), ImGuiDataType_Float, Value, ValueSpeed, &ValueMin, &ValueMax, Format, Flags);

			if (InTable())
			{
				ImGui::TableNextRow();
			}

			return Dragged;
		}

		FORCEINLINE bool DragFloat3(const char* Label, 
									float Vec[3], 
									float VecSpeed = 1.0f, 
									float VecMin = 0.0f, 
									float VecMax = 0.0f, 
									const char* Format = "%.3f", 
									ImGuiSliderFlags Flags = 0)
		{
			ImGuiTable* CurrentTable = ImGui::GetCurrentTable();
			const bool IsInTable = (CurrentTable != nullptr);
			if (IsInTable)
			{
				ImGui::TableSetColumnIndex(0);
			}

			const bool Changed = ImGui::DragFloat3(Label, Vec, VecSpeed, VecMin, VecMax, Format, Flags);
			DrawItemActivityOutline();

			return Changed;
		}

		FORCEINLINE bool DragFloat4(const char* Label, 
									float Vec[4], 
									float VecSpeed = 1.0f, 
									float VecMin = 0.0f, 
									float VecMax = 0.0f, 
									const char* Format = "%.3f",
									ImGuiSliderFlags Flags = 0)
		{
			ImGuiTable* CurrentTable = ImGui::GetCurrentTable();
			const bool IsInTable = (CurrentTable != nullptr);
			if (IsInTable)
			{
				ImGui::TableSetColumnIndex(0);
			}

			const bool Changed = ImGui::DragFloat4(Label, Vec, VecSpeed, VecMin, VecMax, Format, Flags);
			DrawItemActivityOutline();

			return Changed;
		}


		FORCEINLINE bool SliderFloat3(const char* Label, 
									  float Vec[3], 
									  float VecMin, 
									  float VecMax, 
									  const char* Format = "%.3f", 
									  ImGuiSliderFlags Flags = 0) 
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
		/// TODO: Templated argument for selecting XYZ/RGB on the drawn buttons.
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

			if (InTable())
			{
				ImGui::TableSetColumnIndex(0);
				UI::ShiftCursor(Slider::TablePaddingX, 7.0f);

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

				static constexpr float FramePadding = 4.0f;
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

			if (InTable())
			{
				ImGui::TableNextRow();
			}

			return Modified || ManuallyEdited;
		}

		/**
		 * Vec3Control
		 */
		template<EVectorSemantic VecSemantic = EVectorSemantic::XYZW, typename VectorType = glm::vec3>
		FORCEINLINE bool Vec3Control(const std::string& Label, 
									 VectorType& Values, 
									 bool& ManuallyEdited, 
									 const float ResetValue = 0.0f, 
									 const float ValueSpeed = 0.10f,
									 const float ValueMin = 0.0f,
									 const float ValueMax = 0.0f,
									 const float ColumnWidth = 100.0f, 
									 uint32_t RenderMultiSelectAxes = 0,
									 const char* Format = "%.2f")
		{
			static constexpr const char* V1 = (VecSemantic == EVectorSemantic::XYZW) ? "X" : "R";
			static constexpr const char* V2 = (VecSemantic == EVectorSemantic::XYZW) ? "Y" : "G";
			static constexpr const char* V3 = (VecSemantic == EVectorSemantic::XYZW) ? "Z" : "B";

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

				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

				/* Draw: V1 (First Vector Component). */
				DrawControl(
					V1, 
					Values.x, 
					ImVec4(0.80f, 0.10f, 0.15f, 1.0f), /* Normal  */
					ImVec4(0.90f, 0.20f, 0.20f, 1.0f), /* Hover   */
					ImVec4(0.80f, 0.10f, 0.15f, 1.0f), /* Pressed */
					(RenderMultiSelectAxes & EVectorAxis::X)
				); 

				/* Draw: V2 (Second Vector Component). */
				ImGui::SameLine(0.0f, OutlineSpacing);
				DrawControl(
					V2, 
					Values.y, 
					ImVec4(0.20f, 0.70f, 0.20f, 1.0f), 
					ImVec4(0.30f, 0.80f, 0.30f, 1.0f), 
					ImVec4(0.20f, 0.70f, 0.20f, 1.0f), 
					(RenderMultiSelectAxes & EVectorAxis::Y)
				);

				/* Draw: V3 (Third Vector Component). */
				ImGui::SameLine(0.0f, OutlineSpacing);
				DrawControl(
					V3, 
					Values.z, 
					ImVec4(0.10f, 0.25f, 0.80f, 1.0f), 
					ImVec4(0.20f, 0.35f, 0.90f, 1.0f), 
					ImVec4(0.10f, 0.25f, 0.80f, 1.0f), 
					(RenderMultiSelectAxes & EVectorAxis::Z)
				);

				ImGui::PopStyleVar(1); /* FrameRounding */

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
		 * Vec4Control
		 * 
		 *  Slider for 4 different values.
		 */
		template<EVectorSemantic VecSemantic = EVectorSemantic::XYZW, typename VectorType = ImVec4>
		FORCEINLINE bool Vec4Control(const std::string& Label, 
									 VectorType& Values, 
									 const float ValueSpeed = 0.10f,
									 const float ResetValue = 0.0f, 
									 const float ValueMin = 0.0f,
									 const float ValueMax = 0.0f,
									 const float ColumnWidth = 100.0f, 
									 const char* Format = "%.2f",
									 uint32_t RenderMultiSelectAxes = 0)
		{
			static_assert((VecSemantic == EVectorSemantic::XYZW) || (VecSemantic == EVectorSemantic::RGBA),
						  "Invalid type of vector semantic, only vectors with 4 elements are allowed");
			static_assert(std::disjunction_v<
							std::is_same<VectorType, ImVec4>, 
							std::is_same<VectorType, glm::vec4>>, 
						  "Invalid vector type");

			static constexpr const char* V1 = (VecSemantic == EVectorSemantic::XYZW) ? "X" : "R";
			static constexpr const char* V2 = (VecSemantic == EVectorSemantic::XYZW) ? "Y" : "G";
			static constexpr const char* V3 = (VecSemantic == EVectorSemantic::XYZW) ? "Z" : "B";
			static constexpr const char* V4 = (VecSemantic == EVectorSemantic::XYZW) ? "W" : "A";

			bool Modified = false;
			bool ManuallyEdited = false;

			ImGuiTable* CurrentTable = ImGui::GetCurrentTable();
			const bool IsInTable = (CurrentTable != nullptr);

			if (UIContext.bInGrid)
			{
				ImGui::TableSetColumnIndex(0);
				if (!Label.empty() && Label.at(0) != '#')
				{
					ImGui::Text(Label.c_str());
					/* TODO: Do PushStyle here instead. */
				}
			}
			else if (IsInTable)
			{
				ImGui::TableSetColumnIndex(0);
				UI::ShiftCursor(Slider::TablePaddingX, 7.0f);

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

			{
				static constexpr float SpacingX = 8.0f;
				UI::FScopedStyle ItemSpacing(ImGuiStyleVar_ItemSpacing, ImVec2(SpacingX, 0.0f));
				UI::FScopedStyle WindowPadding(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 2.0f));
				{
					UI::FScopedColor BorderColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 0));
					UI::FScopedColor FrameBg(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 0));

					static constexpr float FrameHeightPadding = 4.0f;

					ImGui::BeginChild(
						ImGui::GetID((Label + "Subwindow").c_str()),
						ImVec2((ImGui::GetContentRegionAvail().x - SpacingX), ImGui::GetFrameHeightWithSpacing() + FrameHeightPadding),
						(UI::Debug::GridBorders == (int)EBorder::None) ? ImGuiChildFlags_None : ImGuiChildFlags_Border,
						ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse /* Window Flags. */
					);
				}

				static constexpr float FramePadding = 4.0f;
				static constexpr float OutlineSpacing = 1.0f;

				const float LineHeight = GImGui->Font->FontSize + FramePadding * 2.0f;
				const ImVec2 ButtonSize = { LineHeight + 2.0f, LineHeight };
				const float InputItemWidth = (ImGui::GetContentRegionAvail().x - SpacingX) / 4.0f - ButtonSize.x;

				UI::ShiftCursor(0.0f, FramePadding);

				auto DrawControl = [&](const char* InLabel, 
									   float& InValue, 
									   const VectorType& InColorNormal,
									   const VectorType& InColorHover, 
									   const VectorType& InColorPressed, 
									   bool RenderMultiSelect)
				{
					const std::string LabelStr = std::format("##{}", InLabel);
					{
						static constexpr ImVec2 ButtonFrameSize(2.0f * FramePadding, 0.0f);
						//UI::FScopedStyle ButtonFramePadding(ImGuiStyleVar_FramePadding, ImVec2(FramePadding * 2.0f, 0.0f));
						UI::FScopedStyle ButtonFramePadding(ImGuiStyleVar_FramePadding, ButtonFrameSize);
						UI::FScopedStyle ButtonRounding(ImGuiStyleVar_FrameRounding, 1.0f);
						UI::FScopedColorStack ButtonColours(
							ImGuiCol_Button, InColorNormal, 
							ImGuiCol_ButtonHovered, InColorHover,
							ImGuiCol_ButtonActive, InColorPressed
						);

						if (ImGui::Button(InLabel, ButtonSize))
						{
							InValue = ResetValue;
							Modified = true;
							LK_CORE_DEBUG("Pressed slider button: {}", InLabel);
						}
					}

					ImGui::SameLine(0.0f, OutlineSpacing);
					ImGui::SetNextItemWidth(InputItemWidth);

					ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, RenderMultiSelect);
					const ImGuiID InputID = ImGui::GetID(LabelStr.c_str());
					const bool WasTempInputActive = ImGui::TempInputIsActive(InputID);
					Modified |= ImGui::DragFloat(LabelStr.c_str(), &InValue, ValueSpeed, ValueMin, ValueMax, Format, 0);

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

				if (UIContext.bInGrid || IsInTable)
				{
					ImGui::TableSetColumnIndex(1);
				}

				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

				/* Draw: V1 (First Vector Component). */
				DrawControl(
					V1, 
					Values.x, 
					ImVec4(0.80f, 0.10f, 0.15f, 1.0f), /* Normal  */
					ImVec4(0.90f, 0.20f, 0.20f, 1.0f), /* Hover   */
					ImVec4(0.80f, 0.10f, 0.15f, 1.0f), /* Pressed */
					(RenderMultiSelectAxes & EVectorAxis::X)
				); 

				/* Draw: V2 (Second Vector Component). */
				ImGui::SameLine(0.0f, OutlineSpacing);
				DrawControl(
					V2, 
					Values.y, 
					ImVec4(0.20f, 0.70f, 0.20f, 1.0f), 
					ImVec4(0.30f, 0.80f, 0.30f, 1.0f), 
					ImVec4(0.20f, 0.70f, 0.20f, 1.0f), 
					(RenderMultiSelectAxes & EVectorAxis::Y)
				);

				/* Draw: V3 (Third Vector Component). */
				ImGui::SameLine(0.0f, OutlineSpacing);
				DrawControl(
					V3, 
					Values.z, 
					ImVec4(0.10f, 0.25f, 0.80f, 1.0f), 
					ImVec4(0.20f, 0.35f, 0.90f, 1.0f), 
					ImVec4(0.10f, 0.25f, 0.80f, 1.0f), 
					(RenderMultiSelectAxes & EVectorAxis::Z)
				);

				/* Draw: V4 (Fourth Vector Component). */
				ImGui::SameLine(0.0f, OutlineSpacing);
				DrawControl(
					V4, 
					Values.w, 
					VectorType(0.50f, 0.40f, 0.70f, 1.0f),
					VectorType(0.55f, 0.40f, 0.60f, 1.0f),
					VectorType(0.50f, 0.40f, 0.70f, 1.0f),
					(RenderMultiSelectAxes & EVectorAxis::Z)
				);

				ImGui::PopStyleVar(1); /* FrameRounding */
				ImGui::EndChild();
			}

			if (UIContext.bInGrid)
			{
				ImGui::TableNextRow();
			}
			else if (IsInTable)
			{
				ImGui::TableNextRow();
			}

			return (Modified || ManuallyEdited);
		}

	#if 0
		/**
		 * Vec4Control
		 * 
		 *  To be used in already existing tables.
		 */
		FORCEINLINE bool Vec4Control(const std::string& Label, 
									 ImVec4& Values, 
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

			ImGuiTable* CurrentTable = ImGui::GetCurrentTable();
			const bool IsInTable = (CurrentTable != nullptr);

			if (UIContext.bInGrid)
			{
				ImGui::TableSetColumnIndex(0);
				if (!Label.empty() && Label.at(0) != '#')
				{
					ImGui::Text(Label.c_str());
					/* TODO: Do PushStyle here instead. */
				}
			}
			else if (IsInTable)
			{
				ImGui::TableSetColumnIndex(0);
				UI::ShiftCursor(Slider::TablePaddingX, 7.0f);

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

				static constexpr float FramePadding = 4.0f;
				static constexpr float OutlineSpacing = 1.0f;
				const float LineHeight = GImGui->Font->FontSize + FramePadding * 2.0f;
				const ImVec2 ButtonSize = { LineHeight + 2.0f, LineHeight };
				const float InputItemWidth = (ImGui::GetContentRegionAvail().x - SpacingX) / 4.0f - ButtonSize.x;

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

				if (IsInTable)
				{
					ImGui::TableSetColumnIndex(1);
					//UI::ShiftCursor(Slider::TablePaddingX, 7.0f);
				}

				/* Draw R. */
				DrawControl(
					"R", 
					Values.x, 
					ImVec4(0.80f, 0.10f, 0.15f, 1.0f), /* Normal  */
					ImVec4(0.90f, 0.20f, 0.20f, 1.0f), /* Hover   */
					ImVec4(0.80f, 0.10f, 0.15f, 1.0f), /* Pressed */
					(RenderMultiSelectAxes & EVectorAxis::X)
				); 

				/* Draw G. */
				ImGui::SameLine(0.0f, OutlineSpacing);
				DrawControl(
					"G", 
					Values.y, 
					ImVec4(0.20f, 0.70f, 0.20f, 1.0f), 
					ImVec4(0.30f, 0.80f, 0.30f, 1.0f), 
					ImVec4(0.20f, 0.70f, 0.20f, 1.0f), 
					(RenderMultiSelectAxes & EVectorAxis::Y)
				);

				/* Draw B. */
				ImGui::SameLine(0.0f, OutlineSpacing);
				DrawControl(
					"B", 
					Values.z, 
					ImVec4(0.10f, 0.25f, 0.80f, 1.0f), 
					ImVec4(0.20f, 0.35f, 0.90f, 1.0f), 
					ImVec4(0.10f, 0.25f, 0.80f, 1.0f), 
					(RenderMultiSelectAxes & EVectorAxis::Z)
				);

				/* Draw A. */
				ImGui::SameLine(0.0f, OutlineSpacing);
				DrawControl(
					"A", 
					Values.w, 
					ImVec4(0.50f, 0.40f, 0.70f, 1.0f),
					ImVec4(0.55f, 0.40f, 0.60f, 1.0f),
					ImVec4(0.50f, 0.40f, 0.70f, 1.0f),
					(RenderMultiSelectAxes & EVectorAxis::W)
				);

				ImGui::EndChild();
			}

			if (UIContext.bInGrid)
			{
				ImGui::TableNextRow();
			}
			else if (IsInTable)
			{
				ImGui::TableNextRow();
			}

			return (Modified || ManuallyEdited);
		}
	#endif
	}

}
