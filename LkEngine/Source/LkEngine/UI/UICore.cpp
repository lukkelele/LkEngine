#include "LKpch.h"
#include "UICore.h"

#include "LkEngine/Core/Window.h"
#include "LkEngine/Core/IO/FileSystem.h"
#include "LkEngine/Scene/Entity.h"

#include "LkEngine/Editor/EditorLayer.h"


namespace LkEngine::UI {

	static std::unordered_map<std::string, FMessageBox> MessageBoxes;
	static std::unordered_map<FFontEntry, ImFont*> Fonts;

    ImGuiWindowFlags CoreViewportFlags = ImGuiWindowFlags_None
        | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse 
        | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar
        | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoScrollWithMouse
        | ImGuiWindowFlags_NoInputs;

    ImGuiWindowFlags HostWindowFlags = ImGuiWindowFlags_None
        | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse 
        | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus 
        | ImGuiWindowFlags_NoInputs;

    ImGuiDockNodeFlags DockspaceFlags = ImGuiDockNodeFlags_None
        | ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode 
        | ImGuiDockNodeFlags_NoDockingOverMe | ImGuiDockNodeFlags_NoTabBar;

    ImGuiWindowFlags MenuBarFlags = ImGuiWindowFlags_MenuBar
        | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoCollapse;

    ImGuiWindowFlags SidebarFlags = ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus 
        | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoMove;

    ImGuiWindowFlags SidebarDockspaceFlags = ImGuiDockNodeFlags_NoDockingSplit | ImGuiDockNodeFlags_NoTabBar;

    ImGuiWindowFlags TabBarFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse 
        | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBringToFrontOnFocus 
        | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

    ImGuiWindowFlags ViewportTextureFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse 
        | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
        | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize;

    static uint32_t Counter = 0;
    static int UIContextID = 0;
    static char IDBuffer[16] = "##";
    static char LabelIDBuffer[1024];

    static int PushedStyleVars = 0;
    static int PushedStyleColors = 0;

    const char* GenerateID()
    {
        LK_ITOA(Counter++, IDBuffer + 2, sizeof(IDBuffer) - 2, 16);
        return IDBuffer;
    }

    void PushID()
    {
        ImGui::PushID(UIContextID++);
        Counter = 0;
    }

    void PushID(const char* ID)
    {
        ImGui::PushID(ID);
    }

    void PopID()
    {
        ImGui::PopID();
        UIContextID--;
    }

    void PopID(const char* ID)
    {
        ImGui::PopID();
    }

    bool IsInputEnabled()
    {
        const ImGuiIO& io = ImGui::GetIO();
        return (io.ConfigFlags & ImGuiConfigFlags_NoMouse) == 0 && (io.ConfigFlags & ImGuiConfigFlags_NavNoCaptureKeyboard) == 0;
    }

    bool IsMouseEnabled()
    {
        const ImGuiIO& io = ImGui::GetIO();
        return (io.ConfigFlags & ImGuiConfigFlags_NoMouse) == 0;
    }

    bool IsKeyboardEnabled()
    {
        const ImGuiIO& io = ImGui::GetIO();
        return (io.ConfigFlags & ImGuiConfigFlags_NavNoCaptureKeyboard) == 0;
    }

    void SetInputEnabled(const bool Enabled)
    {
        ImGuiIO& io = ImGui::GetIO();

        if (Enabled)
        {
            io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
            io.ConfigFlags &= ~ImGuiConfigFlags_NavNoCaptureKeyboard;
        }
        else
        {
            io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
            io.ConfigFlags |= ImGuiConfigFlags_NavNoCaptureKeyboard;
        }
    }

    void Separator(const ImVec2 Size, const ImVec4 Color)
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, Color);
        ImGui::BeginChild("sep", Size);
        ImGui::EndChild();
        ImGui::PopStyleColor();
    }

    bool IsWindowFocused(const char* WindowName, const bool CheckRootWindow)
    {
        ImGuiWindow* CurrentNavWindow = GImGui->NavWindow;

        if (CheckRootWindow)
        {
            // Get the actual nav window (not e.g a table)
            ImGuiWindow* LastWindow = NULL;
            while (LastWindow != CurrentNavWindow)
            {
                LastWindow = CurrentNavWindow;
                CurrentNavWindow = CurrentNavWindow->RootWindow;
            }
        }

        return (CurrentNavWindow == ImGui::FindWindowByName(WindowName));
    }

    void Begin(std::string WindowTitle, ImGuiWindowFlags WindowFlags, bool* Open)
    {
        Begin(WindowTitle.c_str(), WindowFlags, Open);
    }

    void Begin(const char* WindowTitle, ImGuiWindowFlags WindowFlags, bool* Open)
    {
        UI::PushID();
        ImGui::Begin(WindowTitle, Open, WindowFlags);
    }

    void Begin(ImGuiWindowFlags WindowFlags, bool* Open)
    {
        UI::PushID();
        ImGui::Begin(UI::GenerateID(), Open, WindowFlags);
    }

    void End()
    {
        ImGui::End();
        UI::PopID();
    }
    
    void BeginCoreViewport()
    {
        UI::PushID(UI_CORE_VIEWPORT);
		ImGui::Begin(UI_CORE_VIEWPORT, nullptr, CoreViewportFlags);
    }

    void EndCoreViewport()
    {
        ImGui::End();
        UI::PopID(UI_CORE_VIEWPORT);
    }

    void BeginDockSpace(const char* DockspaceID)
    {
		ImGuiStyle& Style = ImGui::GetStyle();

		if (UI::DockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
		{
		    UI::CoreViewportFlags |= ImGuiWindowFlags_NoBackground;
			UI::HostWindowFlags |= ImGuiWindowFlags_NoBackground;
		}

		float MinWinSizeX = Style.WindowMinSize.x;
		Style.WindowMinSize.x = 370.0f;
		ImGui::DockSpace(ImGui::GetID(LkEngine_DockSpace), ImVec2(0, 0), UI::DockspaceFlags);
		Style.WindowMinSize.x = MinWinSizeX;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::Begin(LkEngine_DockSpace, NULL, UI::HostWindowFlags);
		ImGui::PopStyleColor(1);
		ImGui::PopStyleVar(1);
    }

    void BeginViewport(const char* ViewportID, LWindow* WindowRef, ImGuiViewport* Viewport)
    {
		ImGui::SetNextWindowPos(Viewport->Pos);
		ImGui::SetNextWindowSize(Viewport->Size);
		ImGui::SetNextWindowViewport(Viewport->ID);

		GLFWwindow* GlfwWindow= WindowRef->GetGlfwWindow();
		const bool bIsMaximized = (bool)glfwGetWindowAttrib(GlfwWindow, GLFW_MAXIMIZED);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::Begin(UI_CORE_VIEWPORT, NULL, UI::CoreViewportFlags);
		ImGui::PopStyleColor(); 
		ImGui::PopStyleVar(2);
    }

	void ShowMessageBox(const char* Title, 
						const std::function<void()>& RenderFunction, 
						const uint32_t Width,
						const uint32_t Height,
						const uint32_t MinWidth,
						const uint32_t MinHeight,
						const uint32_t MaxWidth,
						const uint32_t MaxHeight,
						uint32_t Flags)
	{
		FMessageBox& MessageBoxRef = MessageBoxes[Title];
		MessageBoxRef.Title = LK_FORMAT_STRING("{0}##MessageBoxRef{1}", Title, MessageBoxes.size() + 1);
		MessageBoxRef.UserRenderFunction = RenderFunction;
		MessageBoxRef.Flags = LK_MESSAGE_BOX_USER_FUNCTION | Flags;
		MessageBoxRef.Width = Width;
		MessageBoxRef.Height = Height;
		MessageBoxRef.MinWidth = MinWidth;
		MessageBoxRef.MinHeight = MinHeight;
		MessageBoxRef.MaxWidth = MaxWidth;
		MessageBoxRef.MaxHeight = MaxHeight;
		MessageBoxRef.bShouldOpen = true;
	}

	void RenderMessageBoxes()
	{
		/* Cannot use 'MessageBox' as a variable name since WinUser.h occupies it as a macro. */
		for (auto& [Key, MessageBoxRef] : MessageBoxes)
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

			if (MessageBoxRef.Width != 0 || MessageBoxRef.Height != 0)
			{
				const ImVec2 Center = ImGui::GetMainViewport()->GetCenter();
				ImGui::SetNextWindowPos(Center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
				ImGui::SetNextWindowSize(
					ImVec2{ (float)MessageBoxRef.Width, (float)MessageBoxRef.Height }, 
					ImGuiCond_Appearing
				);
			}

			ImGuiWindowFlags WindowFlags = 0;
			if(MessageBoxRef.Flags & LK_MESSAGE_BOX_AUTO_SIZE)
			{
				WindowFlags |= ImGuiWindowFlags_AlwaysAutoResize;
			}
			else
			{
				ImGui::SetNextWindowSizeConstraints(
					ImVec2{ (float)MessageBoxRef.MinWidth, (float)MessageBoxRef.MinHeight }, 
					ImVec2{ (float)MessageBoxRef.MaxWidth, (float)MessageBoxRef.MaxHeight }
				);
			}

			if (ImGui::BeginPopupModal(MessageBoxRef.Title.c_str(), &MessageBoxRef.bIsOpen, WindowFlags))
			{
				if (MessageBoxRef.Flags & LK_MESSAGE_BOX_USER_FUNCTION)
				{
					LK_CORE_VERIFY(MessageBoxRef.UserRenderFunction, "Message box is missing a render function");
					MessageBoxRef.UserRenderFunction();
				}
				else
				{
					ImGui::TextWrapped(MessageBoxRef.Body.c_str());

					if (MessageBoxRef.Flags & LK_MESSAGE_BOX_OK_BUTTON)
					{
						if (ImGui::Button("Ok"))
						{
							ImGui::CloseCurrentPopup();
						}

						if (MessageBoxRef.Flags & LK_MESSAGE_BOX_CANCEL_BUTTON)
						{
							ImGui::SameLine();
						}
					}

					if ((MessageBoxRef.Flags & LK_MESSAGE_BOX_CANCEL_BUTTON) && ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
					}
				}

				ImGui::EndPopup();
			}
		}
	}

	void PushStyleVar(const ImGuiStyleVar StyleVar, const ImVec2& Var)
    {
        ImGui::PushStyleVar(StyleVar, Var);
        PushedStyleVars++;
    }

    void PushStyleVar(const ImGuiStyleVar StyleVar, const glm::vec2& Var)
    {
        ImGui::PushStyleVar(StyleVar, ImVec2(Var.x, Var.y));
        PushedStyleVars++;
    }

    void PopStyleVar(const uint8_t VarsToPop)
    {
        ImGui::PopStyleVar(VarsToPop);
        PushedStyleVars = PushedStyleVars - VarsToPop;
        LK_CORE_VERIFY(PushedStyleVars >= 0, "UI StyleVar Push/Pop stack invalid!");
    }

    void PushStyleColor(const ImGuiCol ColorVar, const ImVec4& Color)
    {
        ImGui::PushStyleColor(ColorVar, Color);
        PushedStyleColors++;
    }

    void PushStyleColor(const ImGuiCol ColorVar, const glm::vec4& Color)
    {
        ImGui::PushStyleColor(ColorVar, ImVec4(Color.r, Color.g, Color.b, Color.a));
        PushedStyleColors++;
    }

    void PopStyleColor(const uint8_t VarsToPop)
    {
        ImGui::PopStyleColor(VarsToPop);
        PushedStyleColors = PushedStyleColors - VarsToPop;
        LK_CORE_VERIFY(PushedStyleColors >= 0, "UI Color Push/Pop stack invalid!");
    }

    void PopStyleStack()
    {
        if (PushedStyleColors > 0)
        {
            PopStyleColor(PushedStyleColors);
        }

        if (PushedStyleVars > 0)
        {
            PopStyleVar(PushedStyleVars);
        }
    }

	void Font::Add(const FFontConfiguration& FontConfig, bool IsDefault)
	{
		LK_CORE_ASSERT(LFileSystem::Exists(FontConfig.FilePath), "Invalid font filepath");

		using EntryPair = std::pair<FFontEntry, ImFont*>;
		auto FindFont = [&FontConfig](const EntryPair& FontEntry) -> bool
		{
			return (FontEntry.first.Name == FontConfig.FontName);
		};
		if (auto Iter = std::find_if(Fonts.begin(), Fonts.end(), FindFont); Iter != Fonts.end())
		{
			LK_CORE_WARN("Failed to add font '{}', the name is already taken by another font", FontConfig.FontName);
			return;
		}

		ImFontConfig ImguiFontConfig;
		ImguiFontConfig.MergeMode = FontConfig.MergeWithLast;
		auto& IO = ImGui::GetIO();
		ImFont* Font = IO.Fonts->AddFontFromFileTTF(
			FontConfig.FilePath.data(), 
			FontConfig.Size, 
			&ImguiFontConfig, 
			(FontConfig.GlyphRanges == nullptr ? IO.Fonts->GetGlyphRangesDefault() : FontConfig.GlyphRanges)
		);
		LK_CORE_VERIFY(Font, "Failed to load font");

		FFontEntry FontEntry = {
			.Name = FontConfig.FontName
		};
		Fonts[FontEntry] = Font;

		if (IsDefault)
		{
			IO.FontDefault = Font;
		}
	}

	void Font::Push(const std::string& FontName)
	{
		using EntryPair = std::pair<FFontEntry, ImFont*>;
		auto FindFont = [&FontName](const EntryPair& FontEntry) -> bool
		{
			return (FontEntry.first.Name == FontName);
		};
		if (auto Iter = std::find_if(Fonts.begin(), Fonts.end(), FindFont); Iter != Fonts.end())
		{
			ImGui::PushFont(Iter->second);
			return;
		}

		/* Use default font if no font was found. */
		const auto& IO = ImGui::GetIO();
		ImGui::PushFont(IO.FontDefault);
	}

	void Font::Pop()
	{
		ImGui::PopFont();
	}

	ImFont* Font::Get(const std::string& FontName)
	{
		using EntryPair = std::pair<FFontEntry, ImFont*>;
		auto FindFont = [&FontName](const EntryPair& FontEntry) -> bool
		{
			return (FontEntry.first.Name == FontName);
		};
		if (auto Iter = std::find_if(Fonts.begin(), Fonts.end(), FindFont); Iter != Fonts.end())
		{
			return Iter->second;
		}

		LK_VERIFY(false, "Failed to find font '{}'", FontName);
		return nullptr;
	}

}
