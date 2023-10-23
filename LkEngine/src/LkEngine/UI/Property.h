#pragma once
#include "LkEngine/Core/Base.h"
#include <imgui/imgui.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>
#include "LkEngine/Scene/Components.h"


namespace LkEngine::UI {

    namespace Property {

		static void Rotation2D(uint32_t id, TransformComponent& transform)
		{
			ImGui::PushID(id);
			ImGui::SliderFloat2("Rot", &transform.Rotation.x, -6.0f, 6.0f, "%.3f");
			ImGui::PopID();
		}

		static void Scale2D(uint32_t id, TransformComponent& transform)
		{
			ImGui::PushID(id);

			// Submit render
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 0));

			ImGui::Text("Scale");
			ImGui::SameLine();
			//ImGui::SetCursorPosX();
			ImGui::PushItemWidth(100); // TODO
			ImGui::SliderFloat2("##scale-vec2", &transform.Scale.x, -2.0f, 2.0f, "%.1f");
			ImGui::PopItemWidth();

			ImGui::Text("Rotation");
			ImGui::SameLine();
			//ImGui::SetCursorPosX(scale_textsize.x);
			ImGui::PushItemWidth(100); // TODO
			ImGui::SliderFloat2("##rot-vec2", &transform.Rotation.x, -6.0f, 6.0f, "%.1f"); // TODO: make to rad
			ImGui::PopItemWidth();

			ImGui::PopStyleVar(1);

			ImGui::PopID();

		}

		static void Rotation3D(float rot)
		{
		}

		// 2D
		static void PositionXY(uint32_t id, glm::vec3& pos, float step = 1.0f, float min = -2500.0f, float max = 2500.0f, float resetValue = 0.0f, float column_width = 100.0f)
		{
			ImGui::PushID(id);
			ImGuiIO& io = ImGui::GetIO();
			auto boldFont = io.Fonts->Fonts[0];

			ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));

			ImGuiSliderFlags slider_flags = ImGuiSliderFlags_None;
			float line_height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			line_height += 8.0f;
			ImVec2 button_size = { line_height + 3.0f, line_height };
			if (ImGui::BeginTable("##POS_XYZ", 2, flags, ImVec2(column_width, 0)))
			{
				static float column_1_width = button_size.x;
				ImGui::TableSetupColumn(NULL, ImGuiTableColumnFlags_WidthFixed, column_1_width);
				ImGui::TableSetupColumn(NULL, ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoResize);
				//ImGui::TableSetupColumn(NULL, ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoResize);

				float padding = 20.0f;
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 4));

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(255, 0, 0, 255), 0);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0, 0.0, 0.50, 1.0f));
				if (ImGui::Button("X", button_size))
				{
				}
				ImGui::PopStyleColor();
				ImGui::TableSetColumnIndex(1);
				float slider_width = ImGui::GetColumnWidth();
				ImGui::SetNextItemWidth(slider_width);
				ImGui::DragFloat("##pos-x", &pos.x, step, min, max, "%1.f");

				// Y
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.10f, 0.90f, 0.26f, 1.0f));
				if (ImGui::Button("Y", button_size))
				{
				}
				ImGui::PopStyleColor();
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(slider_width);
				ImGui::DragFloat("##pos-y", &pos.y, step, min, max, "%1.f");

				ImGui::PopStyleVar(1);

				ImGui::EndTable();
			}
			ImGui::PopStyleVar(2);
			ImGui::PopID();
		}

		static void PositionXYZ(uint32_t id, glm::vec3& pos, float step = 1.0f, 
			float min = -2500.0f, float max = 2500.0f, float resetValue = 0.0f, float column_width = 100.0f)
		{
			ImGui::PushID(id);
			ImGuiIO& io = ImGui::GetIO();
			auto boldFont = io.Fonts->Fonts[0];

			ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));

			float line_height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			line_height += 8.0f;
			ImVec2 button_size = { line_height + 3.0f, line_height };
			if (ImGui::BeginTable("##POS_XYZ", 2, flags, ImVec2(column_width, 0)))
			{
				//static float column_1_width = 32.0f;
				static float column_1_width = button_size.x;
				ImGui::TableSetupColumn(NULL, ImGuiTableColumnFlags_WidthFixed, column_1_width);
				//ImGui::TableSetupColumn(NULL, ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoResize);
				ImGui::TableSetupColumn(NULL, ImGuiTableColumnFlags_NoResize);

				float padding = 20.0f;
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 4));

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(255, 0, 0, 255), 0);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0, 0.0, 0.50, 1.0f));
				if (ImGui::Button("X", button_size))
				{
				}
				ImGui::PopStyleColor();
				ImGui::TableSetColumnIndex(1);
				float slider_width = ImGui::GetColumnWidth();
				if (slider_width > column_width)
					slider_width = column_width;
				ImGui::SetNextItemWidth(slider_width);
				ImGui::DragFloat("##pos-x", &pos.x, step, min, max, "%1.f");

				// Y
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.10f, 0.90f, 0.26f, 1.0f));
				if (ImGui::Button("Y", button_size))
				{
				}
				ImGui::PopStyleColor();
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(slider_width);
				ImGui::DragFloat("##pos-y", &pos.y, step, min, max, "%1.f");

				// Z
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.90f, 0.55f, 0.0f, 0.9f));
				if (ImGui::Button("Z", button_size))
				{
				}
				ImGui::PopStyleColor();
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(slider_width);
				ImGui::DragFloat("##Z", &pos.z, step, min, max, "%1.f");

				ImGui::PopStyleVar(1);

				ImGui::EndTable();
			}
			ImGui::PopStyleVar(2);
			ImGui::PopID();
		}

		static void RGBAColor(uint32_t id, glm::vec4& color, float column_width = 100.0f)
		{
			//ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_RowBg;
			ImGui::PushID(id);
			ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));

			float line_height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			line_height += 8.0f;
			ImVec2 button_size = { line_height + 3.0f, line_height };
			if (ImGui::BeginTable("##RGBA", 2, flags, ImVec2(column_width, 0)))
			{
				//static float column_1_width = 32.0f;
				static float column_1_width = button_size.x;
				ImGui::TableSetupColumn(NULL, ImGuiTableColumnFlags_WidthFixed, column_1_width);
				ImGui::TableSetupColumn(NULL, ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoResize);

				float padding = 20.0f;
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 4));

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(255, 0, 0, 255), 0);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.830f, 0.0f, 0.0f, 1.0f));
				if (ImGui::Button("R", button_size))
				{
				}
				ImGui::PopStyleColor();
				ImGui::TableSetColumnIndex(1);
				float slider_width = ImGui::GetColumnWidth();
				ImGui::SetNextItemWidth(slider_width);
				ImGui::DragFloat("##R", &color.r, 0.01f, 0.0f, 1.0f, "%.3f");

				// Green
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.83f, 0.0f, 1.0f));
				if (ImGui::Button("G", button_size))
				{
				}
				ImGui::PopStyleColor();
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(slider_width);
				ImGui::DragFloat("##G", &color.g, 0.01f, 0.0f, 1.0f, "%.3f");

				// Blue
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
				if (ImGui::Button("B", button_size))
				{
				}
				ImGui::PopStyleColor();
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(slider_width);
				ImGui::DragFloat("##B", &color.b, 0.01f, 0.0f, 1.0f, "%.3f");

				// Alpha
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.50f, 0.50f, 0.50f, 1.0f));
				if (ImGui::Button("A", button_size))
				{
				}
				ImGui::PopStyleColor();
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(slider_width);
				ImGui::DragFloat("##A", &color.a, 0.01f, 0.0f, 1.0f, "%.3f");

				ImGui::PopStyleVar(1);

				ImGui::EndTable();
			}
			ImGui::PopStyleVar(2);

			ImGui::PopID();
		}

    }

}