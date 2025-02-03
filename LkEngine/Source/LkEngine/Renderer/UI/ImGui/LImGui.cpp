#include "LKpch.h"
#include "LImGui.h"

#include "LkEngine/Renderer/UI/ImGui/ImGuiWidgets.h"


namespace LkEngine::UI {

	namespace Internal 
	{
		std::vector<ImGuiID> GridIdStack{};
	}

	FOnMessageBoxCancelled OnMessageBoxCancelled;

	void Separator(const ImVec2& Size, const ImVec4& Color)
	{
		ImGui::PushStyleColor(ImGuiCol_ChildBg, Color);
		ImGui::BeginChild("##Separator", Size);
		ImGui::EndChild();
		ImGui::PopStyleColor();
	}

	void ShowMessageBox(const char* Title, 
						const std::function<void()>& RenderFunction, 
						uint32_t Flags,
						const uint32_t Width,
						const uint32_t Height,
						const uint32_t MinWidth,
						const uint32_t MinHeight,
						const uint32_t MaxWidth,
						const uint32_t MaxHeight)
	{
		FMessageBox& MessageBoxRef = Internal::MessageBoxes[Title];
		MessageBoxRef.Title = std::format("{0}##MessageBoxRef{1}", Title, Internal::MessageBoxes.size() + 1);
		MessageBoxRef.UserRenderFunction = RenderFunction;
		MessageBoxRef.Flags = Flags | EMessageBoxFlag::UserFunction;
		MessageBoxRef.Width = Width;
		MessageBoxRef.Height = Height;
		MessageBoxRef.MinWidth = MinWidth;
		MessageBoxRef.MinHeight = MinHeight;
		MessageBoxRef.MaxWidth = MaxWidth;
		MessageBoxRef.MaxHeight = MaxHeight;
		MessageBoxRef.bShouldOpen = true;
	}

	/* TODO: Mark the message box as volatile to not cache it in the .ini file. */
	void RenderMessageBoxes()
	{
		/* Cannot use 'MessageBox' as a variable name since WinUser.h occupies it as a macro. */
		for (auto& [Key, MessageBoxRef] : Internal::MessageBoxes)
		{
			if (MessageBoxRef.bShouldOpen && !ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId))
			{
				ImGui::OpenPopup(MessageBoxRef.Title.c_str());
				MessageBoxRef.bShouldOpen = false;
				MessageBoxRef.bIsOpen = true;
			}

			if (!MessageBoxRef.bIsOpen)
			{
				continue;
			}

			if (!ImGui::IsPopupOpen(MessageBoxRef.Title.c_str()))
			{
				MessageBoxRef.bIsOpen = false;
				continue;
			}

			if ((MessageBoxRef.Width != 0) || (MessageBoxRef.Height != 0))
			{
				const ImVec2 Center = ImGui::GetMainViewport()->GetCenter();
				ImGui::SetNextWindowPos(Center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
				ImGui::SetNextWindowSize(
					ImVec2((float)MessageBoxRef.Width, (float)MessageBoxRef.Height),
					ImGuiCond_Appearing
				);
			}

			const bool HasOkButton = (MessageBoxRef.Flags & EMessageBoxFlag::OkButton);
			const bool HasCancelButton = (MessageBoxRef.Flags & EMessageBoxFlag::CancelButton);
			static const ImVec2 OkButtonSize = ImVec2(34, 30);
			static const ImVec2 CancelButtonSize = ImVec2(54, OkButtonSize.y);

			ImGuiStyle& Style = ImGui::GetStyle();
			ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoSavedSettings;

			if (MessageBoxRef.Flags & EMessageBoxFlag::AutoSize)
			{
				WindowFlags |= ImGuiWindowFlags_AlwaysAutoResize;

				if (HasOkButton || HasCancelButton)
				{
					const ImVec2 MinSizeConstraint = ImVec2(
						(float)MessageBoxRef.MinWidth + 2.0f * (OkButtonSize.x + Style.FramePadding.x + Style.WindowPadding.x), 
						(float)MessageBoxRef.MinHeight + 2.0f * (OkButtonSize.y + Style.FramePadding.y + Style.WindowPadding.y)
					);
					ImGui::SetNextWindowSizeConstraints( 
						MinSizeConstraint,
						ImVec2((float)MessageBoxRef.MaxWidth, (float)MessageBoxRef.MaxHeight)
					);
				}
			}
			else
			{
				if (HasOkButton || HasCancelButton)
				{
					ImGui::SetNextWindowSizeConstraints(
						ImVec2((float)MessageBoxRef.MinWidth, (float)MessageBoxRef.MinHeight + 2.0f * (OkButtonSize.y + Style.FramePadding.y + Style.WindowPadding.y)),
						ImVec2((float)MessageBoxRef.MaxWidth, (float)MessageBoxRef.MaxHeight)
					);
				}
				else
				{
					ImGui::SetNextWindowSizeConstraints(
						ImVec2((float)MessageBoxRef.MinWidth, (float)MessageBoxRef.MinHeight), 
						ImVec2((float)MessageBoxRef.MaxWidth, (float)MessageBoxRef.MaxHeight)
					);
				}
			}

			if (ImGui::BeginPopupModal(MessageBoxRef.Title.c_str(), &MessageBoxRef.bIsOpen, WindowFlags))
			{
				const ImVec2 WindowSize = ImGui::GetContentRegionAvail();

				if (MessageBoxRef.Flags & EMessageBoxFlag::UserFunction)
				{
					LK_CORE_VERIFY(MessageBoxRef.UserRenderFunction, "Message box is missing a render function");
					MessageBoxRef.UserRenderFunction();

					if (HasOkButton)
					{
						if (!HasCancelButton)
						{
							UI::ShiftCursor(
								(WindowSize.x - (OkButtonSize.x + Style.FramePadding.x + Style.WindowPadding.x)), 
								(WindowSize.y - (OkButtonSize.y + 2.0f * (Style.FramePadding.y + Style.WindowPadding.y + Style.ItemSpacing.y)))
							);
						}
						else
						{
							UI::ShiftCursor(
								(WindowSize.x - (OkButtonSize.x + CancelButtonSize.x + Style.FramePadding.x + Style.WindowPadding.x)), 
								(WindowSize.y - (OkButtonSize.y + 2.0f * (Style.FramePadding.y + Style.WindowPadding.y + Style.ItemSpacing.y)))
							);
						}

						if (HasCancelButton)
						{
							if (ImGui::Button("Cancel", CancelButtonSize))
							{
								/* TODO: Cancel callback. */
								OnMessageBoxCancelled.Broadcast(MessageBoxRef.Title.c_str());
								ImGui::CloseCurrentPopup();
							}
							ImGui::SameLine();
						}

						if (ImGui::Button("OK", OkButtonSize))
						{
							ImGui::CloseCurrentPopup();
						}
					}

					if (!HasOkButton && HasCancelButton)
					{
						UI::ShiftCursor(
							(WindowSize.x - (CancelButtonSize.x + Style.FramePadding.x + Style.WindowPadding.x)), 
							(WindowSize.y - (CancelButtonSize.y + 2.0f * (Style.FramePadding.y + Style.WindowPadding.y + Style.ItemSpacing.y)))
						);

						if (ImGui::Button("Cancel", CancelButtonSize))
						{
							/* TODO: Cancel callback. */
							OnMessageBoxCancelled.Broadcast(MessageBoxRef.Title.c_str());
							ImGui::CloseCurrentPopup();
						}
					}
				}

				ImGui::EndPopup();
			}
		}
	}

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
