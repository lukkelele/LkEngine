#include "LKpch.h"
#include "Table.h"


namespace LkEngine::UI {

	bool TableRowClickable(const char* RowID, const float RowHeight)
	{
		ImGuiWindow* Window = ImGui::GetCurrentWindow();
		Window->DC.CurrLineSize.y = RowHeight;

		ImGui::TableNextRow(0, RowHeight);
		ImGui::TableNextColumn();

		//Window->DC.CurrLineTextBaseOffset = 3.0f;
		Window->DC.CurrLineTextBaseOffset = 1.0f;
		const ImVec2 RowAreaMin = ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 0).Min;
		const ImVec2 RowAreaMax = ImVec2(
			ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), ImGui::TableGetColumnCount() - 1).Max.x, 
			RowAreaMin.y + RowHeight 
		);

		ImGui::PushClipRect(RowAreaMin, RowAreaMax, false);

		ImGui::SetNextItemAllowOverlap();
		bool IsRowHovered, IsHeld;
		const bool IsRowClicked = ImGui::ButtonBehavior(
			ImRect(RowAreaMin, RowAreaMax), 
			ImGui::GetID(RowID), 
			&IsRowHovered, /* Hovered */
			&IsHeld,       /* Held */
			ImGuiButtonFlags_AllowOverlap
		);

		ImGui::PopClipRect();

	#if 0
		LK_CORE_INFO("Row '{}' Clicked={} Held={} Hovered={}  AreaMin=({},{}) AreaMax=({},{})",
					 ImGui::GetID(RowID), IsRowClicked, IsHeld, IsRowHovered,
					 RowAreaMin.x, RowAreaMin.y, RowAreaMax.x, RowAreaMax.y);
	#endif

		return IsRowClicked;
	}


}
