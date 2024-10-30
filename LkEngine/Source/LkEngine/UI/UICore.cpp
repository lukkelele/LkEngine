#include "LKpch.h"
#include "UICore.h"

#include "LkEngine/Core/Window.h"
#include "LkEngine/Scene/Entity.h"

#include "LkEngine/Editor/EditorLayer.h"


namespace LkEngine::UI {

    ImGuiWindowFlags CoreViewportFlags = ImGuiWindowFlags_None
        | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse 
        | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar
        | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus 
        | ImGuiWindowFlags_NoInputs;
        //| ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;

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

    ImGuiWindowFlags SidebarDockspaceFlags = ImGuiDockNodeFlags_NoDockingSplitMe | ImGuiDockNodeFlags_NoTabBar;

    ImGuiWindowFlags TabBarFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse 
        | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBringToFrontOnFocus 
        | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

    ImGuiWindowFlags ViewportTextureFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse 
        | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar 
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

    /// TODO: Refactor
    void BeginSubwindow(const char* WindowName, ImGuiWindowFlags WindowFlags)
    {
        LK_MARK_FUNC_NOT_IMPLEMENTED("Needs refactoring");
    #if 0
        PushID(std::string("##" + std::string(WindowName)).c_str());
        ImGui::Begin(SelectedEntityWindow, NULL, (WindowFlags | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse));
    #endif
    }

    void EndSubwindow()
    {
        ImGui::End();
        PopID();
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

}
