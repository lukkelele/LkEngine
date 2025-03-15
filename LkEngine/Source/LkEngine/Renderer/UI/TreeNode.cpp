#include "LKpch.h"
#include "TreeNode.h"

#include "LkEngine/Editor/EditorGlobals.h"
#include "LkEngine/Renderer/Color.h"


/**************************************
 *           CUSTOM IMGUI
 **************************************/
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
						  const ImColor IconTint,
						  const ImGuiDir OpenDirection)
	{
		ImGuiWindow* Window = GetCurrentWindow();
		if (!Window || Window->SkipItems)
		{
			return false;
		}

		ImGuiContext& G = *GImGui;
		ImGuiLastItemData& LastItem = G.LastItemData;
		const ImGuiStyle& Style = G.Style;

		const bool IsFramed = (TreeNodeFlags & ImGuiTreeNodeFlags_Framed) != 0;
		const ImVec2 Padding = (IsFramed || (TreeNodeFlags & ImGuiTreeNodeFlags_FramePadding)) 
			? Style.FramePadding 
			: ImVec2(Style.FramePadding.x, ImMin(Window->DC.CurrLineTextBaseOffset, Style.FramePadding.y));

		if (!LabelEnd)
		{
			LabelEnd = FindRenderedTextEnd(Label);
		}
		const ImVec2 LabelSize = CalcTextSize(Label, LabelEnd, false);

		const float FrameHeight = ImMax(ImMin(Window->DC.CurrLineSize.y, G.FontSize + Style.FramePadding.y * 2), 
										LabelSize.y + Padding.y * 2);
		ImRect FrameBB;
		FrameBB.Min.x = (TreeNodeFlags & ImGuiTreeNodeFlags_SpanFullWidth) 
			? Window->WorkRect.Min.x 
			: Window->DC.CursorPos.x;
		FrameBB.Min.y = Window->DC.CursorPos.y;
		FrameBB.Max.x = Window->WorkRect.Max.x;
		FrameBB.Max.y = Window->DC.CursorPos.y + FrameHeight;

		if (IsFramed)
		{
			FrameBB.Min.x -= IM_FLOOR(Window->WindowPadding.x * 0.50f - 1.0f);
			FrameBB.Max.x += IM_FLOOR(Window->WindowPadding.x * 0.50f);
		}

		const float TextOffsetX = G.FontSize + (IsFramed ? Padding.x * 3 : Padding.x * 2);
		const float TextOffsetY = ImMax(Padding.y, Window->DC.CurrLineTextBaseOffset); 
		const float TextWidth = G.FontSize + (LabelSize.x > 0.0f ? LabelSize.x + Padding.x * 2 : 0.0f);

		ImVec2 TextPos(Window->DC.CursorPos.x + TextOffsetX, Window->DC.CursorPos.y + TextOffsetY);
		ItemSize(ImVec2(TextWidth, FrameHeight), Padding.y);

		/* For regular tree nodes, allow to click past two item spacings. */
		ImRect InteractBB = FrameBB;
		if (!IsFramed && (TreeNodeFlags & (ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth)) == 0)
		{
			InteractBB.Max.x = FrameBB.Min.x + TextWidth + Style.ItemSpacing.x * 2.0f;
		}

		const bool IsLeaf = (TreeNodeFlags & ImGuiTreeNodeFlags_Leaf) != 0;
		const bool PushTreeNodeOnOpen = !(TreeNodeFlags & ImGuiTreeNodeFlags_NoTreePushOnOpen);

		/* Determine if to render the arrow/bullet. */
		const bool ShouldRenderArrow = (OpenDirection != ImGuiDir_None);
		//const bool ShouldRenderArrow = true;

		LK_CORE_ASSERT(ID != 0, "ID is 0");
		bool IsOpen = TreeNodeBehaviorIsOpen(ID, ImGuiTreeNodeFlags_None);

		const bool ItemAdded = ItemAdd(InteractBB, ID);
		LastItem.StatusFlags |= ImGuiItemStatusFlags_HasDisplayRect;
		LastItem.DisplayRect = FrameBB;

		if (!ItemAdded)
		{
			if (IsOpen && PushTreeNodeOnOpen)
			{
				TreePushOverrideID(ID);
			}

			return IsOpen;
		}

		ImGuiButtonFlags ButtonFlags = ImGuiTreeNodeFlags_None;
		if (TreeNodeFlags & ImGuiTreeNodeFlags_AllowItemOverlap)
		{
			ButtonFlags |= ImGuiButtonFlags_AllowOverlap;
		}
		if (!IsLeaf)
		{
			ButtonFlags |= ImGuiButtonFlags_PressedOnDragDropHold;
		}

		const float ArrowHit_x1 = (TextPos.x - TextOffsetX) - Style.TouchExtraPadding.x;
		const float ArrowHit_x2 = (TextPos.x - TextOffsetX) + (G.FontSize + Padding.x * 2.0f) + Style.TouchExtraPadding.x;
		const bool IsMouseXOverArrow = ((G.IO.MousePos.x >= ArrowHit_x1) && (G.IO.MousePos.x < ArrowHit_x2));
		if (Window != G.HoveredWindow || !IsMouseXOverArrow)
		{
			ButtonFlags |= ImGuiButtonFlags_NoKeyModsAllowed;
		}

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
					Toggled |= IsMouseXOverArrow; // && !G.NavDisableMouseHover;
				}
				if ((TreeNodeFlags & ImGuiTreeNodeFlags_OpenOnDoubleClick) && G.IO.MouseDoubleClicked[0])
				{
					Toggled = true;
				}
			}
			else if (Pressed && (G.DragDropHoldJustPressedId == ID))
			{
				IM_ASSERT(ButtonFlags & ImGuiButtonFlags_PressedOnDragDropHold);
				/* Keep the node highlighted after opening when drag'n'dropping. */
				if (!IsOpen)
				{
					Toggled = true;
				}
			}

			if (G.NavId == ID)
			{
				if (IsOpen)
				{
					/* Left. */
					if (G.NavMoveDir == ImGuiDir_Left)
					{
						Toggled = true;
						NavMoveRequestCancel();
					}
				}
				else
				{
					/* Right. */
					if (G.NavMoveDir == ImGuiDir_Right)
					{
						Toggled = true;
						NavMoveRequestCancel();
					}
				}
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

		const ImU32 ArrowColor = Selected ? ::LkEngine::RGBA32::BackgroundDark : ::LkEngine::RGBA32::Muted;

		static constexpr float Pad = 3.0f;
		static constexpr float ArrowWidth = 20.0f + 1.0f; /* 21 <-> 24 */
		const float IconSize = FrameHeight - Pad * 2.0f;

		const ImU32 BgColor = GetColorU32((bHeld && bHovered) 
										  ? ImGuiCol_HeaderActive 
										  : (bHovered && !Selected && !bHeld && !Pressed && !Toggled) 
											? ImGuiCol_HeaderHovered 
											: ImGuiCol_Header);

		ImGuiNavRenderCursorFlags NavHighlightFlags = ImGuiNavHighlightFlags_Compact;

		if (IsFramed)
		{
			RenderFrame(FrameBB.Min, FrameBB.Max, BgColor, true, Style.FrameRounding);
			RenderNavHighlight(FrameBB, ID, NavHighlightFlags);
			if (TreeNodeFlags & ImGuiTreeNodeFlags_Bullet)
			{
				RenderBullet(Window->DrawList, 
							 ImVec2(TextPos.x - TextOffsetX * 0.60f, TextPos.y + G.FontSize * 0.50f), 
							 ArrowColor);
			}
			else if (!IsLeaf && ShouldRenderArrow)
			{
				RenderArrow(Window->DrawList, 
							ImVec2(TextPos.x - TextOffsetX + Padding.x, TextPos.y), 
							ArrowColor, 
							(IsOpen ? OpenDirection : ImGuiDir_Right),
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

			if (Icon)
			{
				const ImGuiID ItemID = LastItem.ID;
				const ImGuiItemFlags ItemFlags = LastItem.ItemFlags;
				const ImGuiItemStatusFlags ItemStatusFlags = LastItem.StatusFlags;
				const ImRect ItemRect = LastItem.Rect;

				using namespace LkEngine;
				//::LkEngine::UI::ShiftCursorY(-FrameHeight + Pad);
				::LkEngine::UI::ShiftCursorY(-FrameHeight - Pad - G.FontSize * 0.10f);
				::LkEngine::UI::ShiftCursorX(ArrowWidth + Pad);
				::LkEngine::UI::Image(
					Icon,
					ImVec2(IconSize, IconSize),
					ImVec2(0, 0),
					ImVec2(1, 1), 
					IconTint,
					UI::Debug::ContentBrowser::bDrawOutlinerBorders ? UI::Debug::ContentBrowser::OutlinerBorderColor : ImVec4(0.0f, 0.0f, 0.0f, 0.0f)
				);

				/* Restore item data. */
				ImGui::SetLastItemData(ItemID, ItemFlags, ItemStatusFlags, ItemRect);

				TextPos.x += FrameHeight + 2.0f;
			}

			TextPos.y -= 1.0f;
			RenderTextClipped(TextPos, FrameBB.Max, Label, LabelEnd, &LabelSize);
		}
		else
		{
			if (bHovered || Selected)
			{
				RenderFrame(FrameBB.Min, FrameBB.Max, BgColor, false);
				RenderNavHighlight(FrameBB, ID, NavHighlightFlags);
			}

			/* Bullet. */
			if (TreeNodeFlags & ImGuiTreeNodeFlags_Bullet)
			{
				RenderBullet(Window->DrawList, 
							 ImVec2(TextPos.x - TextOffsetX * 0.50f, TextPos.y + G.FontSize * 0.50f), 
							 ArrowColor);
			}
			/* Arrow. */
			else if (!IsLeaf && ShouldRenderArrow)
			{
				RenderArrow(Window->DrawList, 
							ImVec2(TextPos.x - TextOffsetX + Padding.x, TextPos.y + G.FontSize * 0.15f), 
							ArrowColor, 
							(IsOpen ? OpenDirection : ImGuiDir_Right),
							0.70f);
			}

			if (Icon)
			{
				const ImGuiID ItemID = LastItem.ID;
				const ImGuiItemFlags ItemFlags = LastItem.ItemFlags;
				const ImGuiItemStatusFlags ItemStatusFlags = LastItem.StatusFlags;
				const ImRect ItemRect = LastItem.Rect;

				using namespace LkEngine;
				::LkEngine::UI::ShiftCursorY(-FrameHeight - Pad - G.FontSize * 0.10f);
				//::LkEngine::UI::ShiftCursorY(-FrameHeight + Pad);
				::LkEngine::UI::ShiftCursorX(ArrowWidth + Pad);
				::LkEngine::UI::Image(
					Icon,
					ImVec2(IconSize, IconSize),
					ImVec2(0, 0),
					ImVec2(1, 1),
					IconTint,
					UI::Debug::ContentBrowser::bDrawOutlinerBorders ? UI::Debug::ContentBrowser::OutlinerBorderColor : ImVec4(0.0f, 0.0f, 0.0f, 0.0f)
				);

				/* Restore item data. */
				ImGui::SetLastItemData(ItemID, ItemFlags, ItemStatusFlags, ItemRect);

				TextPos.x += FrameHeight + 2.0f;
			}

			TextPos.y -= 1.0f;
			RenderText(TextPos, Label, LabelEnd, false);
		}

		if (IsOpen && PushTreeNodeOnOpen)
		{
			TreePushOverrideID(ID);
		}

		return IsOpen;
	}

}

