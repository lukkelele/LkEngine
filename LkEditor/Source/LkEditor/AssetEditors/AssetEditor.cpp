//#include "LKpch.h"
#include "AssetEditor.h"

namespace LkEngine {

	LAssetEditor::LAssetEditor(std::string_view EditorName)
		: ID(EditorName)
		, Title(EditorName)
	{
		LOBJECT_REGISTER();
	}

	void LAssetEditor::RenderUI()
	{
		if (!bIsOpen)
		{
			return;
		}

		const bool WasOpen = bIsOpen;

		ImGui::Begin(Title.c_str(), &bIsOpen, ImGuiWindowFlags_None);
		Render();
		ImGui::End();

		if (WasOpen && !bIsOpen)
		{
			OnClose();
		}
	}

	void LAssetEditor::SetOpen(const bool IsOpen)
	{
		LK_CORE_DEBUG_TAG(std::format("{}", StringUtils::RemovePrefix(GetClass()->GetName(), 'L')), "Set {}", IsOpen ? "open" : "closed");
		bIsOpen = IsOpen;
		bIsOpen ? OnOpen() : OnClose();
	}

	void LAssetEditor::SetTitle(std::string_view NewTitle)
	{
		Title = NewTitle;
	}

}
