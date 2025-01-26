#pragma once

#include "LkEngine/Core/Core.h"
#include "LkEngine/Core/Math/MathLibrary.h"

#include "LkEngine/Renderer/UI/UICore.h"

#include "LkEngine/Scene/Components.h"

#include <imgui/imgui.h>
#include <imgui_internal.h>


namespace LkEngine::UI {

	/// FIXME: REFACTOR THIS
    namespace Property_OLD 
	{

		static void Rotation2D(LTransformComponent& Transform)
		{
			UI::PushID();
			{
				glm::quat Rotation = Transform.GetRotation();
				ImGui::SliderFloat2("Rot", &Rotation.x, -6.0f, 6.0f, "%.3f");
				Transform.SetRotation(Rotation);
			}
			UI::PopID();
		}

		static void Scale2D(uint32_t id, LTransformComponent& Transform)
		{
			UI::PushID();

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 0));

			ImGui::Text("Scale");
			ImGui::SameLine();
			//ImGui::SetCursorPosX();
			ImGui::PushItemWidth(100); // TODO
			ImGui::SliderFloat2("##scale-vec2", &Transform.Scale.x, -2.0f, 2.0f, "%.1f");
			ImGui::PopItemWidth();

			/* Rortation. */
			ImGui::Text("Rotation");
			ImGui::SameLine();
			//ImGui::SetCursorPosX(scale_textsize.x);
			ImGui::PushItemWidth(100); // TODO
			glm::quat Rotation = Transform.GetRotation();

			//ImGui::SliderFloat2("##Rotation-vec2", &Transform.Rotation.x, -6.0f, 6.0f, "%.1f"); // TODO: make to rad
			ImGui::SliderFloat2("Rotation", &Rotation.x, -6.0f, 6.0f, "%.3f");
			Transform.SetRotation(Rotation);
			ImGui::PopItemWidth();

			ImGui::PopStyleVar(1);

			UI::PopID();
		}

		static void Rotation3D(float Rotation)
		{
		}

		static void PositionXY(glm::vec3& Position, 
							   float step = 1.0f, 
							   float Min = -2500.0f, 
							   float Max = 2500.0f, 
							   float ResetValue = 0.0f, 
							   float ColumnWidth = 100.0f)
		{
			UI::PushID();

			ImGuiIO& io = ImGui::GetIO();
			auto BoldFont = io.Fonts->Fonts[0];

			ImGuiTableFlags Flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));

			ImGuiSliderFlags SliderFlags = ImGuiSliderFlags_None;

			float LineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			LineHeight += 8.0f;
			ImVec2 ButtonSize = { LineHeight + 3.0f, LineHeight };

			if (ImGui::BeginTable("##POS_XYZ", 2, Flags, ImVec2(ColumnWidth, 0)))
			{
				static float ColumnWidth_1 = ButtonSize.x;
				ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, ColumnWidth_1);
				ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoResize);

				float Padding = 20.0f;
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 4));

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(255, 0, 0, 255), 0);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0, 0.0, 0.50, 1.0f));
				if (ImGui::Button("X", ButtonSize))
				{
				}
				ImGui::PopStyleColor();
				ImGui::TableSetColumnIndex(1);
				float SliderWidth = ImGui::GetColumnWidth();
				ImGui::SetNextItemWidth(SliderWidth);
				ImGui::DragFloat("##Position-x", &Position.x, step, Min, Max, "%1.f");

				// Y
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.10f, 0.90f, 0.26f, 1.0f));
				if (ImGui::Button("Y", ButtonSize))
				{
				}
				ImGui::PopStyleColor();
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(SliderWidth);
				ImGui::DragFloat("##Position-y", &Position.y, step, Min, Max, "%1.f");

				ImGui::PopStyleVar(1);

				ImGui::EndTable();
			}
			ImGui::PopStyleVar(2);

			UI::PopID();
		}

		static void PositionXYZ(glm::vec3& Position, 
								float step = 1.0f, 
								float Min = -2500.0f, 
								float Max = 2500.0f, 
								float ResetValue = 0.0f, 
								float ColumnWidth = 100.0f)
		{
			ImGuiIO& io = ImGui::GetIO();
			ImFont* BoldFont = io.Fonts->Fonts[0];

			ImGuiTableFlags Flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));

			float LineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			LineHeight += 8.0f;
			ImVec2 ButtonSize = { LineHeight + 3.0f, LineHeight };
			if (ImGui::BeginTable("##POS_XYZ", 2, Flags, ImVec2(ColumnWidth, 0)))
			{
				static float ColumnWidth_1 = ButtonSize.x;
				ImGui::TableSetupColumn(NULL, ImGuiTableColumnFlags_WidthFixed, ColumnWidth_1);
				ImGui::TableSetupColumn(NULL, ImGuiTableColumnFlags_NoResize);

				float Padding = 20.0f;
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 4));

				/*------------------------------
					X
				 ------------------------------*/
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(255, 0, 0, 255), 0);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0, 0.0, 0.50, 1.0f));
				if (ImGui::Button("X", ButtonSize))
				{
				}
				ImGui::PopStyleColor();
				ImGui::TableSetColumnIndex(1);
				float SliderWidth = ImGui::GetColumnWidth();
				if (SliderWidth > ColumnWidth)
					SliderWidth = ColumnWidth;
				ImGui::SetNextItemWidth(SliderWidth);
				ImGui::DragFloat("##Position-x", &Position.x, step, Min, Max, "%1.f");

				/*------------------------------
					Y
				 ------------------------------*/
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.10f, 0.90f, 0.26f, 1.0f));
				if (ImGui::Button("Y", ButtonSize))
				{
				}
				ImGui::PopStyleColor();
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(SliderWidth);
				ImGui::DragFloat("##Position-y", &Position.y, step, Min, Max, "%1.f");

				/*------------------------------
					Z
				 ------------------------------*/
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.90f, 0.55f, 0.0f, 0.9f));
				if (ImGui::Button("Z", ButtonSize))
				{
				}
				ImGui::PopStyleColor();
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(SliderWidth);
				ImGui::DragFloat("##Z", &Position.z, step, Min, Max, "%1.f");

				ImGui::PopStyleVar(1);

				ImGui::EndTable();
			}
			ImGui::PopStyleVar(2);
		}

		static void RGBAColor(glm::vec4& Color, float ColumnWidth = 100.0f)
		{
			ImGui::PushID("##UI_Property-RGBA_Color");
			ImGuiTableFlags Flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));

			float LineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			LineHeight += 8.0f;
			ImVec2 ButtonSize = { LineHeight + 3.0f, LineHeight };
			if (ImGui::BeginTable("##RGBA", 2, Flags, ImVec2(ColumnWidth, 0)))
			{
				static float ColumnWidth_1 = ButtonSize.x;
				ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, ColumnWidth_1);
				ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoResize);

				float Padding = 20.0f;
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 4));

				/* Red. */
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(255, 0, 0, 255), 0);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.830f, 0.0f, 0.0f, 1.0f));
				if (ImGui::Button("R", ButtonSize))
				{
				}
				ImGui::PopStyleColor();
				ImGui::TableSetColumnIndex(1);
				float SliderWidth = ImGui::GetColumnWidth();
				ImGui::SetNextItemWidth(SliderWidth);
				ImGui::DragFloat("##R", &Color.r, 0.01f, 0.0f, 1.0f, "%.3f");

				/* Green. */
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.83f, 0.0f, 1.0f));
				if (ImGui::Button("G", ButtonSize))
				{
				}
				ImGui::PopStyleColor();
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(SliderWidth);
				ImGui::DragFloat("##G", &Color.g, 0.01f, 0.0f, 1.0f, "%.3f");

				/* Blue. */
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
				if (ImGui::Button("B", ButtonSize))
				{
				}
				ImGui::PopStyleColor();
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(SliderWidth);
				ImGui::DragFloat("##B", &Color.b, 0.01f, 0.0f, 1.0f, "%.3f");

				/* Alpha. */
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.50f, 0.50f, 0.50f, 1.0f));
				if (ImGui::Button("A", ButtonSize))
				{
				}
				ImGui::PopStyleColor();
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(SliderWidth);
				ImGui::DragFloat("##A", &Color.a, 0.01f, 0.0f, 1.0f, "%.3f");

				ImGui::PopStyleVar(1);

				ImGui::EndTable();
			}
			ImGui::PopStyleVar(2);

			ImGui::PopID();
		}

    }

}
