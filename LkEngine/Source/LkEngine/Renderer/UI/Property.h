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
		}

		ShiftCursor(10.0f, 9.0f);
		ImGui::Text(Label);

		if (std::strlen(HelpText) > 0)
		{
			ImGui::SameLine();
			HelpMarker(HelpText);
		}

		if (IsCurrentlyInTable)
		{
			ImGui::TableSetColumnIndex(1);
		}

		ShiftCursorY(4.0f);

		ImGui::PushItemWidth(-1);
		Modified = ImGui::Checkbox(std::format("##{0}", Label).c_str(), &Value);
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
		}

		ShiftCursor(10.0f, 9.0f);
		ImGui::Text(Label);

		if (std::strlen(HelpText) != 0)
		{
			ImGui::SameLine();
			HelpMarker(HelpText, HelpSymbol);
		}

		if (IsCurrentlyInTable)
		{
			ImGui::TableSetColumnIndex(1);
		}
		ShiftCursorY(4.0f);

		ImGui::PushItemWidth(-1);
		const bool Modified = UI::Draw::DragFloat(std::format("##{}", Label).c_str(), &Value, Delta, Min, Max);
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
		const bool Modified = UI::Draw::DragFloat3(std::format("##{0}", Label).c_str(), glm::value_ptr(Value), Delta, Min, Max);
		ImGui::PopItemWidth();

		ImGui::TableNextRow();

		return Modified;
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

		const std::string ID = std::format("##{}", Label);
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
		else
		{
			LK_CORE_VERIFY(false);
		}

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

		const std::string ID = std::format("##{}", Label);
		if (UI::BeginCombo(ID.c_str(), CurrentOption))
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
				else
				{
					LK_CORE_VERIFY(false);
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
		ImGui::NextColumn();
		Draw::Underline();

		return Modified;
	}

	FORCEINLINE bool PropertyDropdown(const char* Label, 
									  const char* const* Options, 
									  const uint16_t ArrSize, 
									  int32_t* Selected, 
									  const char* HelpText = "",
									  const int ComboWidth = -1)
	{
		const bool IsCurrentlyInTable = InTable();
		const bool ShouldAlign = !UIContext.AlignedStack.empty();

		if (IsCurrentlyInTable)
		{
			ImGui::TableSetColumnIndex(0);
		}

		if (ShouldAlign)
		{
			Internal::FAlignData& AlignData = UIContext.AlignedStack.front();
			if (AlignData.PassedElements > 0)
			{
				ImGui::SameLine(0.0f, AlignData.Dist);
			}
			AlignData.PassedElements++;
		}
		else
		{
			ShiftCursor(10.0f, 9.0f);
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

		if (IsCurrentlyInTable)
		{
			ImGui::TableSetColumnIndex(1);
		}
		else
		{
			ImGui::NextColumn();
		}

		if (ShouldAlign)
		{
			Internal::FAlignData& AlignData = UIContext.AlignedStack.front();
			ImGui::SameLine(0.0f, AlignData.Dist);
			AlignData.PassedElements++;
		}
		else
		{
			ShiftCursorY(4.0f);
		}
		ImGui::PushItemWidth(ComboWidth);

		bool Modified = false;

		const std::string ID = std::format("##{}", Label);
		if (UI::BeginCombo(ID.c_str(), CurrentOption))
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

		if (IsCurrentlyInTable)
		{
			ImGui::TableNextRow();
		}
		else
		{
			ImGui::NextColumn();
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
		static_assert((std::is_same_v<ValueType, const char*> || Core::IsPair<ValueType>), "Unsupported type");

		const bool ShouldAlign = !UIContext.AlignedStack.empty();
		const bool IsCurrentlyInTable = InTable();

		if (IsCurrentlyInTable)
		{
			ImGui::TableSetColumnIndex(0);
		}

		if (ShouldAlign)
		{
			Internal::FAlignData& AlignData = UIContext.AlignedStack.front();
			if (AlignData.PassedElements > 0)
			{
				ImGui::SameLine(0.0f, AlignData.Dist);
			}
			AlignData.PassedElements++;
		}
		else
		{
			ShiftCursor(10.0f, 9.0f);
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

		if (IsCurrentlyInTable)
		{
			ImGui::TableSetColumnIndex(1);
		}
		else
		{
			ImGui::NextColumn();
		}

		if (ShouldAlign)
		{
			Internal::FAlignData& AlignData = UIContext.AlignedStack.front();
			ImGui::SameLine(0.0f, AlignData.Dist);
			AlignData.PassedElements++;
		}
		else
		{
			ShiftCursorY(4.0f);
		}
		ImGui::PushItemWidth(ComboWidth);

		bool Modified = false;

		const std::string ID = std::format("##{}", Label);
		if (UI::BeginCombo(ID.c_str(), CurrentOption))
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
				else
				{
					LK_CORE_VERIFY(false);
				}

				const bool IsSelected = (Option == CurrentOption);
				if (ImGui::Selectable(Option, IsSelected))
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

		if (IsCurrentlyInTable)
		{
			ImGui::TableNextRow();
		}
		else
		{
			ImGui::NextColumn();
			Draw::Underline();
		}

		return Modified;
	}

}
