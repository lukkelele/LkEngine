#pragma once

#include "UICore.h"


/**************************************
 *           CUSTOM IMGUI
 **************************************/
namespace ImGui {

	bool TreeNodeBehaviorIsOpen(const ImGuiID NodeID, ImGuiTreeNodeFlags Flags = ImGuiTreeNodeFlags_None);

	/**
	 * TreeNodeWithIcon
	 *
	 *  OpenDirection is the direction which the opened node points, down by default.
	 */
	bool TreeNodeWithIcon(::LkEngine::TObjectPtr<::LkEngine::LTexture2D> Icon,
						  const ImGuiID ID,
						  ImGuiTreeNodeFlags TreeNodeFlags,
						  const char* Label,
						  const char* LabelEnd,
						  const ImColor IconTint = IM_COL32_WHITE,
						  const ImGuiDir OpenDirection = ImGuiDir_Down);
}


namespace LkEngine::UI {

	FORCEINLINE bool BeginTreeNode(const char* NodeName, const bool DefaultOpen)
	{
		ImGuiTreeNodeFlags TreeNodeFlags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
		if (DefaultOpen)
		{
			TreeNodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
		}

		return ImGui::TreeNodeEx(NodeName, TreeNodeFlags);
	}

	FORCEINLINE void EndTreeNode()
	{
		ImGui::TreePop();
	}

	FORCEINLINE bool TreeNode(const std::string& ID, 
							  const std::string& Label, 
							  const ImGuiTreeNodeFlags Flags = 0, 
							  const TObjectPtr<LTexture2D>& Icon = nullptr,
							  const ImGuiDir OpenDirection = ImGuiDir_Down)
	{
		ImGuiWindow* Window = ImGui::GetCurrentWindow();
		if (!Window || Window->SkipItems)
		{
			return false;
		}

		return ImGui::TreeNodeWithIcon(
			Icon, 
			Window->GetID(ID.c_str()), 
			Flags, 
			Label.c_str(),  /* Label Start. */
			nullptr,        /* Label End.   */
			IM_COL32_WHITE, /* Icon Tint.   */
			OpenDirection   /* Arrow direction on opened node. */
		);
	}

}

