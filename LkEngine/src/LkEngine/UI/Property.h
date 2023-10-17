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
			ImGui::SliderFloat2("##scale-vec2", &transform.Scale.x, -2.0f, 2.0f, "%.2f");
			ImGui::PopItemWidth();

			ImGui::Text("Rotation");
			ImGui::SameLine();
			//ImGui::SetCursorPosX(scale_textsize.x);
			ImGui::PushItemWidth(100); // TODO
			ImGui::SliderFloat2("##rot-vec2", &transform.Rotation.x, -6.0f, 6.0f, "%.2f"); // TODO: make to rad
			ImGui::PopItemWidth();

			ImGui::PopStyleVar(1);

			ImGui::PopID();

		}

		static void Rotation3D(float rot)
		{
		}

		static void PositionXYZ(uint32_t id, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
		{
			ImGuiIO& io = ImGui::GetIO();
			auto boldFont = io.Fonts->Fonts[0];
		}

		static void RGBAColor(uint32_t id, glm::vec4& color)
		{
			//ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_RowBg;
			ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));

			float line_height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			line_height += 8.0f;
			ImVec2 button_size = { line_height + 3.0f, line_height };
			if (ImGui::BeginTable("##RGBA", 2, flags))
			{
				ImGui::PushID(id);
				//static float column_1_width = 32.0f;
				static float column_1_width = button_size.x;
				ImGui::TableSetupColumn(NULL, ImGuiTableColumnFlags_WidthFixed, column_1_width);
				ImGui::TableSetupColumn(NULL, ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoResize);

				float padding = 20.0f;
				//float slider_width = ImGui::GetContentRegionAvail().x - padding;
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 4));
				//ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				//ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(4, 10));

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(255, 0, 0, 255), 0);
				//ImGui::PushStyleColor(ImGuiCol_TableRowBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0, 0.0, 0.0, 1.0f));
				//ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (column_1_width - ImGui::CalcTextSize("R").x) * 0.50f);
				if (ImGui::Button("R", button_size))
				{
				}
				//ImGui::Text("R");
				ImGui::PopStyleColor();
				ImGui::TableSetColumnIndex(1);
				float slider_width = ImGui::GetColumnWidth();
				ImGui::SetNextItemWidth(slider_width);
				//ImGui::SliderFloat("##R", &color.r, 0.0f, 1.0f);
				ImGui::DragFloat("##R", &color.r, 0.01f, 0.0f, 1.0f, "%.3f");

				// Green
				ImGui::TableNextRow();
				//ImGui::PushStyleColor(ImGuiCol_TableRowBg, ImVec4(255.0f, 1.0f, 0.0f, 1.0f));
				//ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(0, 255, 0, 255), 0);
				ImGui::TableSetColumnIndex(0);
				//ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (column_1_width - ImGui::CalcTextSize("R").x) * 0.50f);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
				if (ImGui::Button("G", button_size))
				{
				}
				//ImGui::Text("G");
				ImGui::PopStyleColor();
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(slider_width);
				//ImGui::SliderFloat("##G", &color.g, 0.0f, 1.0f, "%.3f");
				ImGui::DragFloat("##G", &color.g, 0.01f, 0.0f, 1.0f, "%.3f");

				// Blue
				//ImGui::PushStyleColor(ImGuiCol_TableRowBg, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
				//ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(0, 0, 255, 255), 0);
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				//ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (column_1_width - ImGui::CalcTextSize("R").x) * 0.50f);
				//ImGui::Text("B");
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
				if (ImGui::Button("B", button_size))
				{
				}
				ImGui::PopStyleColor();
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(slider_width);
				//ImGui::SliderFloat("##B", &color.b, 0.0f, 1.0f);
				ImGui::DragFloat("##B", &color.b, 0.01f, 0.0f, 1.0f, "%.3f");

				// Alpha
				ImGui::TableNextRow();
				//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
				ImGui::TableSetColumnIndex(0);
				//ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(100, 100, 100, 255), 0);
				//ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (column_1_width - ImGui::CalcTextSize("R").x) * 0.50f);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.50f, 0.50f, 0.50f, 1.0f));
				if (ImGui::Button("A", button_size))
				{
				}
				//ImGui::Text("A");
				ImGui::PopStyleColor();
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(slider_width);
				//ImGui::SliderFloat("##A", &color.a, 0.0f, 1.0f);
				ImGui::DragFloat("##A", &color.a, 0.01f, 0.0f, 1.0f, "%.3f");

				ImGui::PopStyleVar(1);

				ImGui::PopID();
				ImGui::EndTable();
			}
			ImGui::PopStyleVar(2);

		}

    }

}