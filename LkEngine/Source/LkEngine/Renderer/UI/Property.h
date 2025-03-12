#pragma once

#include "LkEngine/Renderer/UI/UICore.h"
#include "LkEngine/Renderer/UI/Slider.h"


namespace LkEngine::UI {

	FORCEINLINE bool Property(const char* Label, bool& Value, const char* HelpText = "")
	{
		bool Modified = false;

		const bool IsCurrentlyInTable = InTable();
		if (IsCurrentlyInTable)
		{
			ImGui::TableSetColumnIndex(0);
			ShiftCursor(10.0f, 9.0f);
		}

		/* Skip rendering text for '#' and '##' identifier labels. */
		if (Label && Label[0] != '#')
		{
			ImGui::Text(Label);
			if (std::strlen(HelpText) != 0)
			{
				ImGui::SameLine();
				HelpMarker(HelpText);
			}
		}

		if (IsCurrentlyInTable)
		{
			ImGui::TableSetColumnIndex(1);
		}
		else
		{
			ImGui::SameLine();
		}

		ShiftCursorY(4.0f);

		ImGui::PushItemWidth(-1);
		Modified = ImGui::Checkbox(LK_FMT_LIB::format("##{0}", Label).c_str(), &Value);
		ImGui::PopItemWidth();

		if (IsCurrentlyInTable)
		{
			ImGui::TableNextRow();
		}

		return Modified;
	}

	FORCEINLINE bool Property(const char* Label, 
							  float& Value, 
							  float Delta = 0.10f, 
							  float Min = 0.0f, 
							  float Max = 0.0f, 
							  const char* HelpText = "", 
							  const char* HelpSymbol = "(?)")
	{
		const bool IsCurrentlyInTable = InTable();
		if (IsCurrentlyInTable)
		{
			ImGui::TableSetColumnIndex(0);
			ShiftCursor(10.0f, 9.0f);
		}

		/* Skip rendering text for '#' and '##' identifier labels. */
		if (Label && Label[0] != '#')
		{
			ImGui::Text(Label);
			if (std::strlen(HelpText) != 0)
			{
				ImGui::SameLine();
				HelpMarker(HelpText, HelpSymbol);
			}
		}

		if (IsCurrentlyInTable)
		{
			ImGui::TableSetColumnIndex(1);
			ShiftCursorY(4.0f);
		}

		ImGui::PushItemWidth(-1);
		const bool Modified = UI::Draw::DragFloat(LK_FMT_LIB::format("##{}", Label).c_str(), &Value, Delta, Min, Max);
		ImGui::PopItemWidth();

		if (IsCurrentlyInTable)
		{
			ImGui::TableNextRow();
		}

		return Modified;
	}

	/**
	 * Slider property (float 3).
	 */
	FORCEINLINE bool Property(const char* Label, 
							  glm::vec3& Value, 
							  float Delta = 0.10f, 
							  float Min = 0.0f, 
							  float Max = 0.0f, 
							  const char* HelpText = "")
	{
		ImGui::TableSetColumnIndex(0);

		ShiftCursor(10.0f, 9.0f);
		ImGui::Text(Label);

		if (std::strlen(HelpText) != 0)
		{
			ImGui::SameLine();
			HelpMarker(HelpText);
		}

		ImGui::TableSetColumnIndex(1);
		ShiftCursorY(4.0f);

		ImGui::PushItemWidth(-1);
		const bool Modified = UI::Draw::DragFloat3(LK_FMT_LIB::format("##{0}", Label).c_str(), glm::value_ptr(Value), Delta, Min, Max);
		ImGui::PopItemWidth();

		ImGui::TableNextRow();

		return Modified;
	}

	/**
	 * Property without any value (property label).
	 */
	template<typename T>
	FORCEINLINE bool PropertyLabel(const char* Label, const T Value, const char* HelpText = "")
	{
		if (!Label)
		{
			return false;
		}

		const ImVec2 LabelSize = ImGui::CalcTextSize(Label);
		const ImVec2 ValueSize = ImGui::CalcTextSize(LK_FMT_LIB::format("{}", Value).c_str());

		ImGuiContext& G = *GImGui;
		ImGuiTable* CurrentTable = ImGui::GetCurrentTable();
		const bool IsInTable = (CurrentTable != nullptr);

		if (IsInTable)
		{
			ImGui::TableSetColumnIndex(0);

			const float CenterPosX = (ImGui::GetContentRegionAvail().x * 0.50f);
			ShiftCursorX(CenterPosX - (LabelSize.x * 0.50f));
			ImGui::Text(Label);
		}
		else
		{
			ShiftCursor(10.0f, 0.0f);
			ImGui::Text(Label);
		}

		if (std::strlen(HelpText) != 0)
		{
			ImGui::SameLine();
			HelpMarker(HelpText);
		}

		if (IsInTable)
		{
			ImGui::TableSetColumnIndex(1);

			/* Left align for StretchProp weights. */
			if (CurrentTable->Flags & ImGuiTableFlags_SizingStretchProp)
			{
				ShiftCursorX(G.Style.FramePadding.x * 2.0f);
			}
			else
			{
				const float CenterPosX = (ImGui::GetContentRegionAvail().x * 0.50f);
				ShiftCursorX(CenterPosX - (ValueSize.x * 0.50f));
			}

			ImGui::PushItemWidth(-1);
			ImGui::Text("%s", LK_FMT_LIB::format("{}", Value).c_str());
			ImGui::PopItemWidth();

			ImGui::TableNextRow();
		}
		else
		{
			ShiftCursor(10.0f, 0.0f);
			ImGui::PushItemWidth(-1);
			ImGui::Text("%s", LK_FMT_LIB::format("{}", Value).c_str());
			ImGui::PopItemWidth();
		}

		return false; /* Modified */
	}

	FORCEINLINE bool BeginCombo(const char* Label, const char* PreviewValue, ImGuiComboFlags Flags = ImGuiComboFlags_None)
	{
		const bool Opened = ImGui::BeginCombo(Label, PreviewValue, Flags);
		DrawItemActivityOutline();

		return Opened;
	}

	FORCEINLINE void EndCombo()
	{
		ImGui::EndCombo();
		UIContext.NextItemData.ComboFlags = ImGuiComboFlags_None;
	}

	template<typename TEnum>
	FORCEINLINE bool PropertyDropdown(const char* Label, 
									  const char** Options, 
									  const uint16_t ArrSize, 
									  TEnum& Selected, 
									  const char* HelpText = "")
	{
		using T = std::underlying_type_t<TEnum>;
		const T SelectedIndex = static_cast<std::underlying_type_t<TEnum>>(Selected);

		const char* CurrentOption = Options[SelectedIndex];
		ShiftCursor(10.0f, 9.0f);
		ImGui::Text(Label);

		if (std::strlen(HelpText) != 0)
		{
			ImGui::SameLine();
			HelpMarker(HelpText);
		}

		ImGui::NextColumn();
		ShiftCursorY(4.0f);
		ImGui::PushItemWidth(-1);

		bool Modified = false;

		const std::string ID = LK_FMT_LIB::format("##{}", Label);
		if (UI::BeginCombo(ID.c_str(), CurrentOption))
		{
			for (uint16_t Idx = 0; Idx < ArrSize; Idx++)
			{
				const bool IsSelected = (CurrentOption == Options[Idx]);
				if (ImGui::Selectable(Options[Idx], IsSelected))
				{
					CurrentOption = Options[Idx];
					Selected = static_cast<TEnum>(Idx);
					Modified = true;
				}

				if (IsSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			UI::EndCombo();
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();
		Draw::Underline();

		return Modified;
	}

	template<std::size_t ArrSize, typename T, typename TEnum>
	FORCEINLINE bool PropertyDropdown(const char* Label, 
									  const std::array<T, ArrSize>& Options,
									  TEnum& Selected, 
									  const char* HelpText = "",
									  const int ComboWidth = -1)
	{
		using ValueType = std::remove_cvref_t<T>;
		static_assert((std::is_same_v<ValueType, const char*> || Core::IsPair<ValueType>), "Unsupported type");

		using PrimitiveType = std::underlying_type_t<TEnum>;
		const PrimitiveType SelectedIndex = static_cast<PrimitiveType>(Selected);

		const char* CurrentOption = nullptr;
		if constexpr (std::is_same_v<ValueType, const char*>)
		{
			CurrentOption = Options.at(SelectedIndex);
		}
		else if constexpr (Core::IsPair<ValueType>)
		{
			static_assert(Core::IsPairWithFirstArgConstChar<ValueType>, "The first pair argument (the label) is not const char*, which is required");
			/* Select the label from the std::pair (first argument). */
			CurrentOption = Options.at(SelectedIndex).first;
		}

		ImGuiContext& G = *GImGui;
		const float NextItemWidth = G.NextItemData.Width;

		ImGuiTable* CurrentTable = ImGui::GetCurrentTable();
		const bool IsInTable = (CurrentTable != nullptr);

		if (UIContext.bInGrid)
		{
			ImGui::TableSetColumnIndex(0);
			ImGui::Text(Label);
			if (std::strlen(HelpText) != 0)
			{
				ImGui::SameLine();
				HelpMarker(HelpText);
			}
		}
		else if (IsInTable)
		{
			ImGui::Text(Label);
			if (std::strlen(HelpText) != 0)
			{
				ImGui::SameLine();
				HelpMarker(HelpText);
			}
		}
		else
		{
			ShiftCursor((2.0f + G.Style.FramePadding.x), (G.Style.FramePadding.y * 1.0f));
			ImGui::Text(Label);
			if (std::strlen(HelpText) != 0)
			{
				ImGui::SameLine();
				HelpMarker(HelpText);
			}
		}

		if (UIContext.bInGrid)
		{
			ImGui::TableSetColumnIndex(1);
		}
		else if (IsInTable)
		{
		}
		else
		{
			ImGui::SameLine();
			ShiftCursorY(-G.Style.FramePadding.y * 0.50f);
		}

		if (NextItemWidth > 0.0f)
		{
			ImGui::PushItemWidth(NextItemWidth);
		}
		else
		{
			ImGui::PushItemWidth(ComboWidth);
		}

		/* Dropdown modification flag. */
		bool Modified = false;

		ImGuiComboFlags ComboFlags = ImGuiComboFlags_None;
		if (Options.size() > 30)
		{
			ComboFlags |= ImGuiComboFlags_HeightLarge;
		}
		ComboFlags |= UIContext.NextItemData.ComboFlags;

		const std::string ID = LK_FMT_LIB::format("##{}", Label);
		if (UI::BeginCombo(ID.c_str(), CurrentOption, ComboFlags))
		{
			for (uint16_t Idx = 0; Idx < ArrSize; Idx++)
			{
				const char* Option = nullptr;
				if constexpr (std::is_same_v<ValueType, const char*>)
				{
					Option = Options[Idx];
				}
				else if constexpr (Core::IsPair<ValueType>)
				{
					Option = Options.at(Idx).first;
				}

				const bool IsSelected = (Option == CurrentOption);
				if (ImGui::Selectable(Option, IsSelected))
				{
					CurrentOption = Options[Idx];
					Selected = static_cast<TEnum>(Idx);
					Modified = true;
				}

				if (IsSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			UI::EndCombo();
		}

		ImGui::PopItemWidth();

		return Modified;
	}

	FORCEINLINE bool PropertyDropdown(const char* Label, 
									  const char* const* Options, 
									  const uint16_t ArrSize, 
									  int32_t* Selected, 
									  const char* HelpText = "",
									  const int ComboWidth = -1)
	{
		ImGuiContext& G = *GImGui;
		const float NextItemWidth = G.NextItemData.Width;

		ImGuiTable* CurrentTable = ImGui::GetCurrentTable();
		const bool IsInTable = (CurrentTable != nullptr);

		if (UIContext.bInGrid)
		{
			ImGui::TableSetColumnIndex(0);
		}
		else if (IsInTable)
		{
		}
		else
		{
			ImGui::SameLine();
		}

		const char* CurrentOption = Options[*Selected];

		/* Skip rendering text for '#' and '##' identifier labels. */
		if (Label && Label[0] != '#')
		{
			ImGui::Text(Label);
			if (std::strlen(HelpText) != 0)
			{
				ImGui::SameLine();
				HelpMarker(HelpText);
			}
		}

		if (UIContext.bInGrid)
		{
			ImGui::TableSetColumnIndex(1);
		}
		else if (IsInTable)
		{
		}
		else
		{
			ImGui::SameLine(0.0f, 4.0f);
		}

		if (NextItemWidth > 0.0f)
		{
			ImGui::PushItemWidth(NextItemWidth);
		}
		else
		{
			ImGui::PushItemWidth(ComboWidth);
		}

		bool Modified = false;

		ImGuiComboFlags ComboFlags = ImGuiComboFlags_None;
		if (ArrSize > 30)
		{
			ComboFlags |= ImGuiComboFlags_HeightLarge;
		}
		ComboFlags |= UIContext.NextItemData.ComboFlags;

		const std::string ID = LK_FMT_LIB::format("##{}", Label);
		if (UI::BeginCombo(ID.c_str(), CurrentOption, ComboFlags))
		{
			for (uint16_t Idx = 0; Idx < ArrSize; Idx++)
			{
				const bool IsSelected = (CurrentOption == Options[Idx]);
				if (ImGui::Selectable(Options[Idx], IsSelected))
				{
					CurrentOption = Options[Idx];
					*Selected = Idx;
					Modified = true;
				}

				if (IsSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			UI::EndCombo();
		}

		ImGui::PopItemWidth();

		if (UIContext.bInGrid)
		{
			ImGui::TableNextRow();
		}
		if (IsInTable)
		{
		}
		else
		{
			Draw::Underline();
		}

		return Modified;
	}

	template<std::size_t ArrSize, typename T>
	FORCEINLINE bool PropertyDropdown(const char* Label, 
									  const std::array<T, ArrSize>& Options,
									  int32_t* Selected, 
									  const char* HelpText = "",
									  const int ComboWidth = -1)
	{
		using ValueType = std::remove_cvref_t<T>;
		static_assert((std::is_same_v<ValueType, const char*> || Core::IsPair<ValueType>), "The type used in the array is not supported");

		ImGuiContext& G = *GImGui;
		const float NextItemWidth = G.NextItemData.Width;

		ImGuiTable* CurrentTable = ImGui::GetCurrentTable();
		const bool IsInTable = (CurrentTable != nullptr);

		if (UIContext.bInGrid)
		{
			ImGui::TableSetColumnIndex(0);
			UI::ShiftCursorX(Slider::TablePaddingX);
		}
		else if (IsInTable)
		{
		}
		else
		{
		}

		const char* CurrentOption = nullptr;
		if constexpr (std::is_same_v<ValueType, const char*>)
		{
			CurrentOption = Options.at(*Selected);
		}
		else if constexpr (Core::IsPair<ValueType>)
		{
			static_assert(Core::IsPairWithFirstArgConstChar<ValueType>, "The first pair argument (the label) is not const char*, which is required");
			/* Select the label from the std::pair (first argument). */
			CurrentOption = Options.at(*Selected).first;
		}

		/* Skip rendering text for '#' and '##' identifier labels. */
		if (Label && Label[0] != '#')
		{
			ImGui::Text(Label);
			if (std::strlen(HelpText) != 0)
			{
				ImGui::SameLine();
				HelpMarker(HelpText);
			}
		}

		if (UIContext.bInGrid)
		{
			ImGui::TableSetColumnIndex(1);
		}
		else if (IsInTable)
		{
		}
		else
		{
			ImGui::SameLine(0.0f, 4.0f);
		}

		if (NextItemWidth > 0.0f)
		{
			ImGui::PushItemWidth(NextItemWidth);
		}
		else
		{
			ImGui::PushItemWidth(ComboWidth);
		}

		bool Modified = false;

		ImGuiComboFlags ComboFlags = ImGuiComboFlags_None;
		if (Options.size() > 30)
		{
			ComboFlags |= ImGuiComboFlags_HeightLarge;
		}
		ComboFlags |= UIContext.NextItemData.ComboFlags;

		const std::string ID = LK_FMT_LIB::format("##{}", Label);
		if (UI::BeginCombo(ID.c_str(), CurrentOption, ComboFlags))
		{
			for (uint16_t Idx = 0; Idx < ArrSize; Idx++)
			{
				const char* Option = nullptr;
				if constexpr (std::is_same_v<ValueType, const char*>)
				{
					Option = Options[Idx];
				}
				else if constexpr (Core::IsPair<ValueType>)
				{
					Option = Options.at(Idx).first;
				}

				const ImVec2 LabelSize = ImGui::CalcTextSize(Option);

				const bool IsSelected = (Option == CurrentOption);
				if (ImGui::Selectable(Option, IsSelected, ImGuiSelectableFlags_None, ImVec2(0.0f, LabelSize.y + G.Style.FramePadding.y * 1.50f)))
				{
					CurrentOption = Option;
					*Selected = Idx;
					Modified = true;
				}

				if (IsSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			UI::EndCombo();
		}

		ImGui::PopItemWidth();

		if (UIContext.bInGrid)
		{
			ImGui::TableNextRow();
		}
		else if (IsInTable)
		{
		}
		else
		{
		}

		return Modified;
	}

}
