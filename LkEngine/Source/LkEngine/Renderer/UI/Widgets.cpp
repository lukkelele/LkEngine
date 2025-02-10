#include "LKpch.h"
#include "Widgets.h"

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

