#include "LKpch.h"
#include "ImGuiWidgets.h"

#include "LkEngine/UI/ImGui/LImGui.h"
#include "LkEngine/Renderer/Color.h"
#include "LkEngine/Editor/EditorCore.h"


namespace LkEngine::UI {

	bool Widget::OptionsButton()
	{
		const bool Clicked = ImGui::InvisibleButton("##Options", ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight()));
		const float SpaceAvail = std::min(ImGui::GetItemRectSize().x, ImGui::GetItemRectSize().y);
		static constexpr float DesiredIconSize = 15.0f;
		const float Padding = std::max((SpaceAvail - DesiredIconSize) / 2.0f, 0.0f);

		constexpr uint32_t ButtonColor = RGBA32::Text::Normal;
		const uint8_t Value = uint8_t(ImColor(ButtonColor).Value.x * 255);
		ImGui::SameLine();
		UI::DrawButtonImage(
			FEditorResources::GearIcon,
			IM_COL32(Value, Value, Value, 200),
			IM_COL32(Value, Value, Value, 255),
			IM_COL32(Value, Value, Value, 150),
			UI::RectExpanded(UI::GetItemRect(), -Padding, -Padding)
		);

		return Clicked;
	}

}


/**********************************************
 *             EXPERIMENTAL IMGUI
 **********************************************/
namespace ImGui {

	bool TreeNodeBehaviorIsOpen(const ImGuiID NodeID, ImGuiTreeNodeFlags Flags)
	{
		ImGuiContext& G = *GImGui;
		ImGuiWindow* Window = G.CurrentWindow;

		/* Check the state storage for the node's open/close state. */
		const bool IsOpen = Window->StateStorage.GetInt(NodeID, (Flags & ImGuiTreeNodeFlags_DefaultOpen) ? 1 : 0) != 0;

		return IsOpen;
	}

	bool TreeNodeWithIcon(::LkEngine::TObjectPtr<::LkEngine::LTexture2D> Icon,
						  const ImGuiID ID,
						  ImGuiTreeNodeFlags TreeNodeFlags,
						  const char* Label, 
						  const char* LabelEnd,
						  const ImColor IconTint)
	{
		ImGuiWindow* Window = GetCurrentWindow();
		if (Window->SkipItems)
		{
			return false;
		}

		ImGuiContext& G = *GImGui;
		ImGuiLastItemData& LastItem = G.LastItemData;
		const ImGuiStyle& Style = G.Style;

		const bool DisplayFrame = (TreeNodeFlags & ImGuiTreeNodeFlags_Framed) != 0;
		const ImVec2 Padding = (DisplayFrame || (TreeNodeFlags & ImGuiTreeNodeFlags_FramePadding)) 
			? Style.FramePadding 
			: ImVec2(Style.FramePadding.x, ImMin(Window->DC.CurrLineTextBaseOffset, Style.FramePadding.y));

		if (!LabelEnd)
		{
			LabelEnd = FindRenderedTextEnd(Label);
		}
		const ImVec2 LabelSize = CalcTextSize(Label, LabelEnd, false);

		// We vertically grow up to current line height up the typical widget height.
		const float FrameHeight = ImMax(ImMin(Window->DC.CurrLineSize.y, G.FontSize + Style.FramePadding.y * 2), 
										LabelSize.y + Padding.y * 2);
		ImRect FrameBB;
		FrameBB.Min.x = (TreeNodeFlags & ImGuiTreeNodeFlags_SpanFullWidth) 
			? Window->WorkRect.Min.x 
			: Window->DC.CursorPos.x;
		FrameBB.Min.y = Window->DC.CursorPos.y;
		FrameBB.Max.x = Window->WorkRect.Max.x;
		FrameBB.Max.y = Window->DC.CursorPos.y + FrameHeight;

		if (DisplayFrame)
		{
			FrameBB.Min.x -= IM_FLOOR(Window->WindowPadding.x * 0.5f - 1.0f);
			FrameBB.Max.x += IM_FLOOR(Window->WindowPadding.x * 0.5f);
		}

		/* Collapser arrow width + spacing. */
		const float TextOffsetX = G.FontSize + (DisplayFrame ? Padding.x * 3 : Padding.x * 2);

		/* Latch before ItemSize changes it. */
		const float TextOffsetY = ImMax(Padding.y, Window->DC.CurrLineTextBaseOffset); 

		/* Include collapser. */
		const float TextWidth = G.FontSize + (LabelSize.x > 0.0f ? LabelSize.x + Padding.x * 2 : 0.0f);

		ImVec2 TextPos(Window->DC.CursorPos.x + TextOffsetX, Window->DC.CursorPos.y + TextOffsetY);
		ItemSize(ImVec2(TextWidth, FrameHeight), Padding.y);

		// For regular tree nodes, we arbitrary allow to click past 2 worth of ItemSpacing
		ImRect InteractBB = FrameBB;
		if (!DisplayFrame && (TreeNodeFlags & (ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth)) == 0)
		{
			InteractBB.Max.x = FrameBB.Min.x + TextWidth + Style.ItemSpacing.x * 2.0f;
		}

		// Store a flag for the current depth to tell if we will allow closing this node when navigating one of its child.
		// For this purpose we essentially compare if g.NavIdIsAlive went from 0 to 1 between TreeNode() and TreePop().
		// This is currently only support 32 level deep and we are fine with (1 << Depth) overflowing into a zero.
		const bool IsLeaf = (TreeNodeFlags & ImGuiTreeNodeFlags_Leaf) != 0;
		bool IsOpen = TreeNodeBehaviorIsOpen(ID);
		if (IsOpen && !G.NavIdIsAlive 
			&& (TreeNodeFlags & ImGuiTreeNodeFlags_NavLeftJumpsBackHere) 
			&& !(TreeNodeFlags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
		{
			/// FIXME
			//Window->DC.TreeJumpToParentOnPopMask |= (1 << Window->DC.TreeDepth);
		}

		const bool bAddItem = ItemAdd(InteractBB, ID);
		LastItem.StatusFlags |= ImGuiItemStatusFlags_HasDisplayRect;
		LastItem.DisplayRect = FrameBB;

		if (!bAddItem)
		{
			if (IsOpen && !(TreeNodeFlags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
			{
				TreePushOverrideID(ID);
			}
			auto& g = G; /* For the IMGUI_TEST_ENGINE macro. */
			IMGUI_TEST_ENGINE_ITEM_INFO(LastItem.ID, Label, LastItem.StatusFlags 
										| (IsLeaf ? 0 : ImGuiItemStatusFlags_Openable) 
										| (IsOpen ? ImGuiItemStatusFlags_Opened : 0));
			return IsOpen;
		}

		ImGuiButtonFlags ButtonFlags = ImGuiTreeNodeFlags_None;
		if (TreeNodeFlags & ImGuiTreeNodeFlags_AllowItemOverlap)
		{
			//ButtonFlags |= ImGuiButtonFlags_AllowItemOverlap;
			ButtonFlags |= ImGuiButtonFlags_AllowOverlap;
		}
		if (!IsLeaf)
		{
			ButtonFlags |= ImGuiButtonFlags_PressedOnDragDropHold;
		}

		// We allow clicking on the arrow section with keyboard modifiers held, in order to easily
		// allow browsing a tree while preserving selection with code implementing multi-selection patterns.
		// When clicking on the rest of the tree node we always disallow keyboard modifiers.
		const float ArrowHit_x1 = (TextPos.x - TextOffsetX) - Style.TouchExtraPadding.x;
		const float ArrowHit_x2 = (TextPos.x - TextOffsetX) + (G.FontSize + Padding.x * 2.0f) + Style.TouchExtraPadding.x;
		const bool IsMouseXOverArrow = ((G.IO.MousePos.x >= ArrowHit_x1) && (G.IO.MousePos.x < ArrowHit_x2));
		if (Window != G.HoveredWindow || !IsMouseXOverArrow)
		{
			//ButtonFlags |= ImGuiButtonFlags_NoKeyModifiers;
			ButtonFlags |= ImGuiButtonFlags_NoKeyModsAllowed;
		}

		// Open behaviors can be altered with the _OpenOnArrow and _OnOnDoubleClick flags.
		// Some alteration have subtle effects (e.g. toggle on MouseUp vs MouseDown events) due to requirements for multi-selection and drag and drop support.
		// - Single-click on label = Toggle on MouseUp (default, when _OpenOnArrow=0)
		// - Single-click on arrow = Toggle on MouseDown (when _OpenOnArrow=0)
		// - Single-click on arrow = Toggle on MouseDown (when _OpenOnArrow=1)
		// - Double-click on label = Toggle on MouseDoubleClick (when _OpenOnDoubleClick=1)
		// - Double-click on arrow = Toggle on MouseDoubleClick (when _OpenOnDoubleClick=1 and _OpenOnArrow=0)
		// It is rather standard that arrow click react on Down rather than Up.
		// We set ImGuiButtonFlags_PressedOnClickRelease on OpenOnDoubleClick because we want the item to be active on the initial MouseDown in order for drag and drop to work.
		if (IsMouseXOverArrow)
		{
			ButtonFlags |= ImGuiButtonFlags_PressedOnClick;
		}
		else if (TreeNodeFlags & ImGuiTreeNodeFlags_OpenOnDoubleClick)
		{
			ButtonFlags |= ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick;
		}
		else
		{
			ButtonFlags |= ImGuiButtonFlags_PressedOnClickRelease;
		}

		const bool Selected = (TreeNodeFlags & ImGuiTreeNodeFlags_Selected) != 0;
		const bool WasSelected = Selected;

		bool bHovered, bHeld;
		const bool Pressed = ButtonBehavior(InteractBB, ID, &bHovered, &bHeld, ButtonFlags);
		bool Toggled = false;
		if (!IsLeaf)
		{
			if (Pressed && (G.DragDropHoldJustPressedId != ID))
			{
				if ((TreeNodeFlags & (ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick)) == 0 
					|| (G.NavActivateId == ID))
				{
					Toggled = true;
				}
				if (TreeNodeFlags & ImGuiTreeNodeFlags_OpenOnArrow)
				{
					//Toggled |= IsMouseXOverArrow && !G.NavDisableMouseHover;
				}
				if ((TreeNodeFlags & ImGuiTreeNodeFlags_OpenOnDoubleClick) && G.IO.MouseDoubleClicked[0])
				{
					Toggled = true;
				}
			}
			else if (Pressed && (G.DragDropHoldJustPressedId == ID))
			{
				IM_ASSERT(ButtonFlags & ImGuiButtonFlags_PressedOnDragDropHold);
				// When using Drag and Drop "hold to open" we keep the node highlighted 
				// after opening, but never close it again.
				if (!IsOpen)
				{
					Toggled = true;
				}
			}

			if ((G.NavId == ID) && (G.NavMoveDir == ImGuiDir_Left) && IsOpen)
			{
				Toggled = true;
				NavMoveRequestCancel();
			}
			if ((G.NavId == ID) && (G.NavMoveDir == ImGuiDir_Right) && !IsOpen)
			{
				Toggled = true;
				NavMoveRequestCancel();
			}

			if (Toggled)
			{
				IsOpen = !IsOpen;
				Window->DC.StateStorage->SetInt(ID, IsOpen);
				LastItem.StatusFlags |= ImGuiItemStatusFlags_ToggledOpen;
			}
		}

		if (TreeNodeFlags & ImGuiTreeNodeFlags_AllowItemOverlap)
		{
			SetItemAllowOverlap();
		}

		/* NOTE: This is not used in the latest ImGui docking branch (2024-12-30). */
	#if 0
		if (Selected != WasSelected) //-V547
		{
			//LastItem.StatusFlags |= ImGuiItemStatusFlags_ToggledSelection;
		}
	#endif


		/* Render. */
		const ImU32 ArrowColor = Selected ? ::LkEngine::RGBA32::BackgroundDark : ::LkEngine::RGBA32::Muted;

		//ImGuiNavHighlightFlags NavHighlightFlags = ImGuiNavHighlightFlags_TypeThin;
		//decltype(ImGuiNavHighlightFlags_Compact) NavHighlightFlags = ImGuiNavHighlightFlags_Compact;
		ImGuiNavRenderCursorFlags NavHighlightFlags = ImGuiNavHighlightFlags_Compact;
		if (DisplayFrame)
		{
			/* Framed type. */
			/* FIXME: Fix this nested ternary mess. */
			const ImU32 bg_col = GetColorU32((bHeld && bHovered) 
											 ? ImGuiCol_HeaderActive 
											 : (bHovered && !Selected && !bHeld && !Pressed && !Toggled) 
												? ImGuiCol_HeaderHovered 
												: ImGuiCol_Header);

			RenderFrame(FrameBB.Min, FrameBB.Max, bg_col, true, Style.FrameRounding);
			RenderNavHighlight(FrameBB, ID, NavHighlightFlags);
			if (TreeNodeFlags & ImGuiTreeNodeFlags_Bullet)
			{
				RenderBullet(Window->DrawList, 
							 ImVec2(TextPos.x - TextOffsetX * 0.60f, TextPos.y + G.FontSize * 0.50f), 
							 ArrowColor);
			}
			else if (!IsLeaf)
			{
				RenderArrow(Window->DrawList, 
							ImVec2(TextPos.x - TextOffsetX + Padding.x, TextPos.y), 
							ArrowColor, 
							(IsOpen ? ImGuiDir_Down : ImGuiDir_Right), 
							1.0f);
			}
			/* Leaf without bullet, left - adjusted text. */
			else
			{
				TextPos.x -= TextOffsetX;
			}

			if (TreeNodeFlags & ImGuiTreeNodeFlags_ClipLabelForTrailingButton)
			{
				FrameBB.Max.x -= G.FontSize + Style.FramePadding.x;
			}

			/* Draw Icon. */
			if (Icon)
			{
				// Store item data.
				const ImGuiID ItemID = LastItem.ID;
				const ImGuiItemFlags ItemFlags = LastItem.ItemFlags;
				const ImGuiItemStatusFlags ItemStatusFlags = LastItem.StatusFlags;
				const ImRect ItemRect = LastItem.Rect;

				// Draw icon image (messes up last item data).
				const float Pad = 3.0f;
				const float ArrowWidth = 20.0f + 1.0f;
				ImVec2 CursorPos = ImGui::GetCursorPos();
				::LkEngine::UI::ShiftCursorY(-FrameHeight + Pad);
				::LkEngine::UI::ShiftCursorX(ArrowWidth);
				::LkEngine::UI::Image(
					Icon,
					ImVec2(FrameHeight - Pad * 2.0f, FrameHeight - Pad * 2.0f),
					ImVec2(0, 0),
					ImVec2(1, 1),
					IconTint
				);

				// Restore item data.
				ImGui::SetLastItemData(ItemID, ItemFlags, ItemStatusFlags, ItemRect);

				TextPos.x += FrameHeight + 2.0f;
			}

			TextPos.y -= 1.0f;

			if (G.LogEnabled)
			{
				const char LogPrefix[] = "\n##";
				const char LogSuffix[] = "##";
				LogRenderedText(&TextPos, LogPrefix, LogPrefix + 3);
				RenderTextClipped(TextPos, FrameBB.Max, Label, LabelEnd, &LabelSize);
				LogRenderedText(&TextPos, LogSuffix, LogSuffix + 2);
			}
			else
			{
				RenderTextClipped(TextPos, FrameBB.Max, Label, LabelEnd, &LabelSize);
			}
		}
		else
		{
			/* Unframed treenode type. */
			if (bHovered || Selected)
			{
				/* FIXME: Fix this nested ternary mess. */
				const ImU32 bg_col = GetColorU32((bHeld && bHovered) 
												 ? ImGuiCol_HeaderActive 
												 : (bHovered && !Selected && !bHeld && !Pressed && !Toggled) 
													? ImGuiCol_HeaderHovered 
													: ImGuiCol_Header);
				RenderFrame(FrameBB.Min, FrameBB.Max, bg_col, false);
				RenderNavHighlight(FrameBB, ID, NavHighlightFlags);
			}

			if (TreeNodeFlags & ImGuiTreeNodeFlags_Bullet)
			{
				RenderBullet(Window->DrawList, 
							 ImVec2(TextPos.x - TextOffsetX * 0.50f, TextPos.y + G.FontSize * 0.50f), 
							 ArrowColor);
			}
			else if (!IsLeaf)
			{
				RenderArrow(Window->DrawList, 
							ImVec2(TextPos.x - TextOffsetX + Padding.x, TextPos.y + G.FontSize * 0.15f), 
							ArrowColor, 
							(IsOpen ? ImGuiDir_Down : ImGuiDir_Right),
							0.70f);
			}

			/* Draw Icon. */
			if (Icon)
			{
				// Store item data.
				const ImGuiID ItemID = LastItem.ID;
				const ImGuiItemFlags ItemFlags = LastItem.ItemFlags;
				const ImGuiItemStatusFlags ItemStatusFlags = LastItem.StatusFlags;
				const ImRect ItemRect = LastItem.Rect;

				// Draw icon image (messes up last item data).
				static constexpr float Pad = 3.0f;
				static constexpr float ArrowWidth = 20.0f + 1.0f;
				auto CursorPos = ImGui::GetCursorPos();
				::LkEngine::UI::ShiftCursorY(-FrameHeight + Pad);
				::LkEngine::UI::ShiftCursorX(ArrowWidth);
				::LkEngine::UI::Image(
					Icon,
					ImVec2(FrameHeight - Pad * 2.0f, FrameHeight - Pad * 2.0f),
					ImVec2(0, 0),
					ImVec2(1, 1), 
					IconTint
				);

				// Restore item data.
				ImGui::SetLastItemData(ItemID, ItemFlags, ItemStatusFlags, ItemRect);

				TextPos.x += FrameHeight + 2.0f;
			}

			TextPos.y -= 1.0f;


			if (G.LogEnabled)
			{
				LogRenderedText(&TextPos, ">");
			}
			RenderText(TextPos, Label, LabelEnd, false);
		}

		if (IsOpen && !(TreeNodeFlags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
		{
			TreePushOverrideID(ID);
		}

		{
			auto& g = G; /* For the IMGUI_TEST_ENGINE macro. */
			IMGUI_TEST_ENGINE_ITEM_INFO(
				ID, 
				Label, 
				Window->DC.ItemFlags | (IsLeaf ? 0 : ImGuiItemStatusFlags_Openable) | (IsOpen ? ImGuiItemStatusFlags_Opened : 0)
			);
		}

		return IsOpen;
	}


}
