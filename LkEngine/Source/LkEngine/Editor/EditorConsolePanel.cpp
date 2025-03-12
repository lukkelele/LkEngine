#include "LKpch.h"
#include "EditorConsolePanel.h"

#include "LkEngine/Core/Application.h"
#include "LkEngine/Core/Time/Time.h"

#include "LkEngine/Renderer/UI/FontAwesome.h"
#include "LkEngine/Project/Project.h"

#include "ThemeManagerPanel.h"


namespace LkEngine {

	static LEditorConsolePanel* Instance = nullptr;

	namespace 
	{
		const ImVec4 DebugTint   = ImVec4(0.961f, 0.173f, 0.659f,  1.0f);
		const ImVec4 InfoTint    = ImVec4(0.00f,  0.45f,  1.0f,    1.0f);
		const ImVec4 WarningTint = ImVec4(1.00f,  0.90f,  0.060f,  1.0f);
		const ImVec4 ErrorTint   = ImVec4(1.00f,  0.32f,  0.30f,   1.0f);
	}

	LEditorConsolePanel::LEditorConsolePanel()
	{
		LK_CORE_VERIFY(Instance == nullptr);
		Instance = this;
		MessageBuffer.reserve(300);
	}

	LEditorConsolePanel::~LEditorConsolePanel()
	{
		Instance = nullptr;
	}

	void LEditorConsolePanel::RenderUI(bool& IsOpen)
	{
		if (ImGui::Begin(PanelID::EditorConsole, &IsOpen))
		{
			ImVec2 ConsoleSize = ImGui::GetContentRegionAvail();
			ConsoleSize.y -= 32.0f;

			RenderMenu(ImVec2(ConsoleSize.x, 28.0f));
			RenderConsole(ConsoleSize);
		}
		ImGui::End();
	}

	void LEditorConsolePanel::OnProjectChanged(const TObjectPtr<LProject>& InProject)
	{
		LK_CORE_DEBUG_TAG("EditorConsole", "OnProjectChanged: {}", InProject ? InProject->GetConfiguration().Name : "None");
		std::scoped_lock<std::mutex> ScopedLock(MessageBufferMutex);
		MessageBuffer.clear();
	}

	void LEditorConsolePanel::RenderMenu(const ImVec2& InSize)
	{
		UI::FScopedStyleStack Frame(ImGuiStyleVar_FrameBorderSize, 0.0f, 
									ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::BeginChild("Toolbar", InSize);

		if (ImGui::Button("Clear", { 75.0f, 28.0f }))
		{
			std::scoped_lock<std::mutex> ScopedLock(MessageBufferMutex);
			MessageBuffer.clear();
		}

		ImGui::SameLine();

		const ImGuiStyle &Style = ImGui::GetStyle();

		const std::string ClearOnPlayText = LK_FMT_LIB::format("{} Clear on Play", (bClearOnPlay ? LK_ICON_CHECK : LK_ICON_TIMES));
		ImVec4 TextColor = bClearOnPlay ? Style.Colors[ImGuiCol_Text] : Style.Colors[ImGuiCol_TextDisabled];
		if (UI::ColoredButton(ClearOnPlayText.c_str(), GetToolbarButtonColor(bClearOnPlay), TextColor, ImVec2(144.0f, 28.0f)))
		{
			bClearOnPlay = !bClearOnPlay;
		}

		{
			static constexpr ImVec2 ButtonSize(28.0f, 28.0f);

			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 5 * ButtonSize.x, 0.0f);
			TextColor = (VerbosityFilters & EConsoleVerbosity::Debug) ? DebugTint: Style.Colors[ImGuiCol_TextDisabled];
			if (UI::ColoredButton(LK_ICON_MINUS_SQUARE, GetToolbarButtonColor(VerbosityFilters & EConsoleVerbosity::Debug), TextColor, ButtonSize))
			{
				VerbosityFilters ^= (int16_t)EConsoleVerbosity::Debug;
			}

			ImGui::SameLine();
			TextColor = (VerbosityFilters & EConsoleVerbosity::Info) ? InfoTint : Style.Colors[ImGuiCol_TextDisabled];
			if (UI::ColoredButton(LK_ICON_INFO_CIRCLE, GetToolbarButtonColor(VerbosityFilters & EConsoleVerbosity::Info), TextColor, ButtonSize))
			{
				VerbosityFilters ^= (int16_t)EConsoleVerbosity::Info;
			}

			ImGui::SameLine();
			TextColor = (VerbosityFilters & EConsoleVerbosity::Warning) ? WarningTint : Style.Colors[ImGuiCol_TextDisabled];
			if (UI::ColoredButton(LK_ICON_EXCLAMATION_TRIANGLE, GetToolbarButtonColor(VerbosityFilters & EConsoleVerbosity::Warning), TextColor, ButtonSize))
			{
				VerbosityFilters ^= (int16_t)EConsoleVerbosity::Warning;
			}

			ImGui::SameLine();
			TextColor = (VerbosityFilters & EConsoleVerbosity::Error) ? ErrorTint : Style.Colors[ImGuiCol_TextDisabled];
			if (UI::ColoredButton(LK_ICON_EXCLAMATION_CIRCLE, GetToolbarButtonColor(VerbosityFilters & EConsoleVerbosity::Error), TextColor, ButtonSize))
			{
				VerbosityFilters ^= (int16_t)EConsoleVerbosity::Error;
			}
		}

		ImGui::EndChild();
	}

	void LEditorConsolePanel::RenderConsole(const ImVec2& InSize)
	{
		static const char* ConsoleColumns[] = { "Type", "Timestamp", "Message" };

		/* TODO: The call to UI::Table should support std::array (same as UI::BeginPropertyGrid)
		 *       and table flags, see LThemeManagerPanel. */
		UI::Table("Console", ConsoleColumns, 3, InSize, [&]()
		{
			std::scoped_lock<std::mutex> ScopedLock(MessageBufferMutex);
			
			const float ScrollY = ImGui::GetScrollY();
			if (ScrollY < bPreviousScrollY)
			{
				bEnableScrollToLatest = false;
			}

			if (ScrollY >= ImGui::GetScrollMaxY())
			{
				bEnableScrollToLatest = true;
			}

			bPreviousScrollY = ScrollY;

			static constexpr float RowHeight = 26.0f;
			static constexpr float OffsetDistY = 6.0f; /* Distance to separate loglevel colors in 'Type' column. */
			static constexpr float TextPadX = 6.0f;

			/* TODO: Get theme and set the table row colors accordingly. */
			//const ETheme CurrentTheme = LThemeManagerPanel::GetTheme();

			for (uint32_t Idx = 0; Idx < MessageBuffer.size(); Idx++)
			{
				const FConsoleMessage& Message = MessageBuffer[Idx];
				if (!(VerbosityFilters & Message.Flags))
				{
					continue;
				}

				ImGui::PushID(&Message);
				bool RowClicked = false;

				{
					uint8_t PushedColors = 0;
				#if 0
					switch (CurrentTheme)
					{
						case ETheme::Dark:
						Fallback:
						{
							ImGui::PushStyleColor(ImGuiCol_TableRowBg, RGBA32::Background);
							PushedColors++;
						#if 0
							ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, RGBA32::BackgroundDark);
							PushedColors++;
						#endif
							break;
						}

						case ETheme::Light:
						{
							ImGui::PushStyleColor(ImGuiCol_TableRowBg, RGBA32::Gray);
							PushedColors++;
							ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, RGBA32::LightGray);
							PushedColors++;
							break;
						}

						default:
						{
							//LK_CORE_ASSERT(false, "Unsupported theme: {}", static_cast<int>(CurrentTheme));
							/* Use dark theme as fallback. */
							goto Fallback;
						}
					}
				#else
					ImGui::PushStyleColor(ImGuiCol_TableRowBg, RGBA32::Background);
					PushedColors++;
				#endif

					RowClicked = UI::TableRowClickable(Message.ShortMessage.c_str(), RowHeight);
					ImGui::PopStyleColor(PushedColors);
				}

				if (RowClicked)
				{
					LK_CONSOLE_INFO("Clicked row: {}", Idx);
				}

				/* Column: Type (Verbosity/Log Color) */
				UI::ShiftCursorY(OffsetDistY);
				UI::Separator(ImVec2(TextPadX, RowHeight - 2 * OffsetDistY), GetMessageColor(Message));
				ImGui::SameLine();
				UI::ShiftCursorY(-OffsetDistY);
				ImGui::Text(GetMessageType(Message));

				/* Column: Timestamp */
				ImGui::TableNextColumn();
				UI::ShiftCursorX(TextPadX);
				static constexpr const char* TimeFormat = "%H:%M:%S";
				std::stringstream TimeString;
				TimeString << std::put_time(std::localtime(&Message.Time), TimeFormat);
				ImGui::Text(TimeString.str().c_str());

				/* Column: Messages */
				ImGui::TableNextColumn();
				UI::ShiftCursorX(TextPadX);
				ImGui::Text(Message.ShortMessage.c_str());

				if (bScrollToLatest && (Idx == MessageBuffer.size() - Idx))
				{
					ImGui::ScrollToItem();
					bScrollToLatest = false;
				}

				if (RowClicked)
				{
					ImGui::OpenPopup("Detailed Message");
					const LVector2 WindowSize  = LApplication::Get().GetWindow().GetSize();
					const LVector2 WindowPos = LApplication::Get().GetWindow().GetPosition();
					ImGui::SetNextWindowSize(ImVec2(WindowSize.X * 0.50f, WindowSize.Y * 0.50f));
					ImGui::SetNextWindowPos(
						ImVec2(WindowPos.X + WindowSize.X / 2.0f, WindowPos.Y + WindowSize.Y / 2.50f),  /* Position. */
						ImGuiCond_None, 
						ImVec2(0.50f, 0.50f) /* Pivot. */
					);

					bDetailedPanelOpen = true;
				}

				if (bDetailedPanelOpen)
				{
					UI::FScopedStyle WindowPadding(ImGuiStyleVar_WindowPadding, ImVec2(4.0f, 4.0f));
					UI::FScopedStyle FramePadding(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 8.0f));

					if (ImGui::BeginPopupModal("Detailed Message", &bDetailedPanelOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
					{
						ImGui::TextWrapped(Message.LongMessage.c_str());
						if (ImGui::Button("Copy To Clipboard", ImVec2(132.0f, 28.0f)))
						{
							ImGui::SetClipboardText(Message.LongMessage.c_str());
						}
						ImGui::EndPopup();
					}
				}

				ImGui::PopID();
			}
		});
	}

	const char* LEditorConsolePanel::GetMessageType(const FConsoleMessage& Message) 
	{
		if (Message.Flags & EConsoleVerbosity::Debug)   return "Debug";
		if (Message.Flags & EConsoleVerbosity::Info)    return "Info";
		if (Message.Flags & EConsoleVerbosity::Warning) return "Warning";
		if (Message.Flags & EConsoleVerbosity::Error)   return "Error";

		return "Unknown";
	}

	const ImVec4& LEditorConsolePanel::GetMessageColor(const FConsoleMessage& Message) 
	{
		if (Message.Flags & EConsoleVerbosity::Debug)   return DebugTint;
		if (Message.Flags & EConsoleVerbosity::Warning) return WarningTint;
		if (Message.Flags & EConsoleVerbosity::Error)   return ErrorTint;

		return InfoTint;
	}

	ImVec4 LEditorConsolePanel::GetToolbarButtonColor(const bool Value) const
	{
		const ImGuiStyle& Style = ImGui::GetStyle();
		return (Value ? Style.Colors[ImGuiCol_Header] : Style.Colors[ImGuiCol_FrameBg]);
	}

	void LEditorConsolePanel::PushMessage(const FConsoleMessage& Message)
	{
		if (Instance == nullptr)
		{
			LK_CORE_WARN_TAG("EditorConsole", "No console instance exists, cannot queue message: {}", GetMessageType(Message));
			return;
		}

		{
			std::scoped_lock<std::mutex> ScopedLock(Instance->MessageBufferMutex);
			Instance->MessageBuffer.push_back(Message);
		}

		if (Instance->bEnableScrollToLatest)
		{
			Instance->bScrollToLatest = true;
		}
	}

	void LEditorConsolePanel::SerializeToYaml(YAML::Emitter& Out) const
	{
		LK_UNUSED(Out);
	}

	void LEditorConsolePanel::DeserializeFromYaml(const YAML::Node& Data)
	{
		LK_UNUSED(Data);
	}

}
