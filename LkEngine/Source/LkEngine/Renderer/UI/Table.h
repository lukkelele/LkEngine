#pragma once

#include "UICore.h"

#include "Property.h"

#include "LkEngine/Editor/EditorGlobals.h" /* For UI::Debug namespace, should be changed in the future. */


namespace LkEngine::UI {

	FORCEINLINE void BeginPropertyGrid(const char* Label = nullptr,
									   const ImVec2& Size = ImVec2(0.0f, 0.0f), 
									   ImGuiTableFlags Flags = ImGuiTableFlags_SizingStretchProp,
									   const bool UseHeaderLabels = false)
	{
		UI::PushID();

		UIContext.bInGrid = true;

		if (UI::Debug::GridBorders & EBorder::Horizontal) Flags |= ImGuiTableFlags_BordersH;
		if (UI::Debug::GridBorders & EBorder::Vertical)   Flags |= ImGuiTableFlags_BordersV;
		if (UI::Debug::GridBorders & EBorder::All)        Flags |= ImGuiTableFlags_Borders;

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));
		if (!ImGui::BeginTable(Label ? Label : GenerateID(), 2, Flags, Size))
		{
			return;
		}

		static constexpr ImGuiTableColumnFlags ColumnFlags = ImGuiTableColumnFlags_None;
		for (uint32_t ColumnIdx = 0; ColumnIdx < 2; ColumnIdx++)
		{
			if (UseHeaderLabels)
			{
				ImGui::TableSetupColumn(std::format("Column-{}", ColumnIdx).c_str(), ColumnFlags);
			}
			else
			{
				ImGui::TableSetupColumn(std::format("##Column-{}", ColumnIdx).c_str(), ColumnFlags);
			}
		}

		if (UseHeaderLabels)
		{
			ImGui::TableHeadersRow();
		}
		ImGui::TableNextRow();
	}

	FORCEINLINE void EndPropertyGrid()
	{
		ImGui::EndTable();
		ImGui::PopStyleVar(2); /* ItemSpacing, FramePadding */

		UIContext.bInGrid = false;

		UI::PopID();
	}

	FORCEINLINE bool PropertyGridHeader(const std::string& Name, bool OpenByDefault = true)
	{
		ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Framed
			| ImGuiTreeNodeFlags_SpanAvailWidth
			| ImGuiTreeNodeFlags_AllowItemOverlap
			| ImGuiTreeNodeFlags_FramePadding;

		if (OpenByDefault)
		{
			TreeNodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
		}

		static constexpr float FramePaddingX = 6.0f;
		static constexpr float FramePaddingY = 6.0f; // affects height of the header

		UI::FScopedStyle HeaderRounding(ImGuiStyleVar_FrameRounding, 0.0f);
		UI::FScopedStyle HeaderPaddingAndHeight(ImGuiStyleVar_FramePadding, ImVec2(FramePaddingX, FramePaddingY));

		ImGui::PushID(Name.c_str());
		UI::Font::Push("Bold");
		const bool IsOpen = ImGui::TreeNodeEx("##DummyID", TreeNodeFlags, Name.c_str());
		UI::Font::Pop();
		ImGui::PopID();

		return IsOpen;
	}

	FORCEINLINE void EndPropertyGridHeader()
	{
		ImGui::TreePop();
	}

	template<typename T>
	static void Table(const char* TableName, 
					  const char** Columns, 
					  const uint32_t ColumnCount, 
					  const ImVec2& Size, 
					  T RenderFunc)
	{
		if ((Size.x <= 0.0f) || (Size.y <= 0.0f))
		{
			return;
		}

		static constexpr float EdgeOffset = 4.0f;

		FScopedStyle CellPadding(ImGuiStyleVar_CellPadding, ImVec2(4.0f, 0.0f));
		const ImColor BgColor = ImColor(RGBA32::Background);
		const ImColor ColRowAlt = ColorWithMultipliedValue(BgColor, 1.29f);

		FScopedColor RowColor(ImGuiCol_TableRowBg, BgColor);
		FScopedColor RowAltColor(ImGuiCol_TableRowBgAlt, ColRowAlt);
		FScopedColor TableColor(ImGuiCol_ChildBg, BgColor);

		static constexpr ImGuiTableFlags TableFlags = ImGuiTableFlags_NoPadInnerX
			| ImGuiTableFlags_Resizable
			| ImGuiTableFlags_Reorderable
			| ImGuiTableFlags_ScrollY
			| ImGuiTableFlags_RowBg;

		if (!ImGui::BeginTable(TableName, ColumnCount, TableFlags, Size))
		{
			return;
		}

		/* Cache the cursor position on the X-axis. */
		const float CursorX = ImGui::GetCursorScreenPos().x;

		for (uint32_t Idx = 0; Idx < ColumnCount; Idx++)
		{
			ImGui::TableSetupColumn(Columns[Idx]);
		}

		/* Setup table headers. */
		{
			const ImColor ActiveColor = ColorWithMultipliedValue(BgColor, 1.35f);
			FScopedColorStack HeaderCol(ImGuiCol_HeaderHovered, ActiveColor, 
										ImGuiCol_HeaderActive, ActiveColor);

			ImGui::TableSetupScrollFreeze(ImGui::TableGetColumnCount(), 1);
			ImGui::TableNextRow(ImGuiTableRowFlags_Headers, 24.0f);

			for (uint32_t Idx = 0; Idx < ColumnCount; Idx++)
			{
				ImGui::TableSetColumnIndex(Idx);
				const char* ColumnName = ImGui::TableGetColumnName(Idx);
				ImGui::PushID(ColumnName);

				/**
				 * Not sure about the edge offset on the Y-axis.
				 * A factor of 1.0f or 2.0f works.
				 */
				//UI::ShiftCursor(EdgeOffset * 3.0f, EdgeOffset * 2.0f);
				UI::ShiftCursor(EdgeOffset * 3.0f, EdgeOffset * 1.0f);
				ImGui::TableHeader(ColumnName);
				UI::ShiftCursor(-EdgeOffset * 3.0f, -EdgeOffset * 1.0f);

				ImGui::PopID();
			}

			ImGui::SetCursorScreenPos(ImVec2(CursorX, ImGui::GetCursorScreenPos().y));
		}

		RenderFunc();

		ImGui::EndTable();
	}

	template<typename T, std::size_t N>
	static void Table(const char* TableName, 
					  const std::array<const char*, N>& Columns, 
					  const ImVec2& Size, 
					  T RenderFunc)
	{
		if ((Size.x <= 0.0f) || (Size.y <= 0.0f))
		{
			return;
		}

		static constexpr float EdgeOffset = 4.0f;

		FScopedStyle CellPadding(ImGuiStyleVar_CellPadding, ImVec2(4.0f, 0.0f));
		const ImColor BgColor = ImColor(RGBA32::Background);
		const ImColor ColRowAlt = ColorWithMultipliedValue(BgColor, 1.29f);

		FScopedColor TableColor(ImGuiCol_ChildBg, BgColor);
		FScopedColor RowColor(ImGuiCol_TableRowBg, BgColor);
		FScopedColor RowAltColor(ImGuiCol_TableRowBgAlt, ColRowAlt);

		static constexpr ImGuiTableFlags TableFlags = ImGuiTableFlags_NoPadInnerX
			| ImGuiTableFlags_Resizable
			| ImGuiTableFlags_Reorderable
			| ImGuiTableFlags_ScrollY
			| ImGuiTableFlags_RowBg;

		if (!ImGui::BeginTable(TableName, N, TableFlags, Size))
		{
			return;
		}

		/* Cache the cursor position on the X-axis. */
		const float CursorX = ImGui::GetCursorScreenPos().x;

		for (uint32_t Idx = 0; Idx < N; Idx++)
		{
			ImGui::TableSetupColumn(Columns[Idx]);
		}

		/* Setup table headers. */
		{
			const ImColor ActiveColor = ColorWithMultipliedValue(BgColor, 1.35f);
			FScopedColorStack HeaderCol(
				ImGuiCol_HeaderHovered, ActiveColor, 
				ImGuiCol_HeaderActive, ActiveColor
			);

			ImGui::TableSetupScrollFreeze(ImGui::TableGetColumnCount(), 1);
			ImGui::TableNextRow(ImGuiTableRowFlags_Headers, 24.0f);

			for (uint32_t Idx = 0; Idx < N; Idx++)
			{
				ImGui::TableSetColumnIndex(Idx);
				const char* ColumnName = ImGui::TableGetColumnName(Idx);
				ImGui::PushID(ColumnName);

				/**
				 * Not sure about the edge offset on the Y-axis.
				 * A factor of 1.0f or 2.0f works.
				 */
				//UI::ShiftCursor(EdgeOffset * 3.0f, EdgeOffset * 2.0f);
				UI::ShiftCursor(EdgeOffset * 3.0f, EdgeOffset * 1.0f);
				ImGui::TableHeader(ColumnName);
				UI::ShiftCursor(-EdgeOffset * 3.0f, -EdgeOffset * 1.0f);

				ImGui::PopID();
			}

			ImGui::SetCursorScreenPos(ImVec2(CursorX, ImGui::GetCursorScreenPos().y));
		}

		RenderFunc();

		ImGui::EndTable();
	}


	bool TableRowClickable(const char* RowID, const float RowHeight);

}
