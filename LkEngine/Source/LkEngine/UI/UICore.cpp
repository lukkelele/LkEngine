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
    const char* SelectedEntityWindow = UI_SIDEBAR_RIGHT;

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

    void PushID(const char* id)
    {
        ImGui::PushID(id);
    }

    void PopID()
    {
        ImGui::PopID();
        UIContextID--;
    }

    void PopID(const char* id)
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

    void SetInputEnabled(bool enabled)
    {
        ImGuiIO& io = ImGui::GetIO();

        if (enabled)
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

    void Separator(ImVec2 size, ImVec4 color)
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, color);
        ImGui::BeginChild("sep", size);
        ImGui::EndChild();
        ImGui::PopStyleColor();
    }

    bool IsWindowFocused(const char* windowName, const bool checkRootWindow)
    {
        ImGuiWindow* currentNavWindow = GImGui->NavWindow;

        if (checkRootWindow)
        {
            // Get the actual nav window (not e.g a table)
            ImGuiWindow* lastWindow = NULL;
            while (lastWindow != currentNavWindow)
            {
                lastWindow = currentNavWindow;
                currentNavWindow = currentNavWindow->RootWindow;
            }
        }

        return (currentNavWindow == ImGui::FindWindowByName(windowName));
    }

    void Begin(std::string windowTitle, ImGuiWindowFlags windowFlags, bool* open)
    {
        Begin(windowTitle.c_str(), windowFlags, open);
    }

    void Begin(const char* windowTitle, ImGuiWindowFlags windowFlags, bool* open)
    {
        UI::PushID();
        ImGui::Begin(windowTitle, open, windowFlags);
    }

    void Begin(ImGuiWindowFlags windowFlags, bool* open)
    {
        UI::PushID();
        ImGui::Begin(UI::GenerateID(), open, windowFlags);
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

    const char* GetSelectedEntityWindowName()
    {
        return SelectedEntityWindow;
    }

    void BeginSubwindow(const char* windowName, ImGuiWindowFlags windowFlags)
    {
        PushID(std::string("##" + std::string(windowName)).c_str());
        ImGui::Begin(SelectedEntityWindow, NULL, (windowFlags | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse));
    }

    void EndSubwindow()
    {
        ImGui::End();
        PopID();
    }

    void BeginDockSpace(const char* dockspaceID)
    {
		ImGuiStyle& style = ImGui::GetStyle();

		if (UI::DockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
		{
		    UI::CoreViewportFlags |= ImGuiWindowFlags_NoBackground;
			UI::HostWindowFlags |= ImGuiWindowFlags_NoBackground;
		}

		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		ImGui::DockSpace(ImGui::GetID(LkEngine_DockSpace), ImVec2(0, 0), UI::DockspaceFlags);
		style.WindowMinSize.x = minWinSizeX;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::Begin(LkEngine_DockSpace, NULL, UI::HostWindowFlags);
		ImGui::PopStyleColor(1);
		ImGui::PopStyleVar(1);
    }

    void BeginViewport(const char* viewportID, LWindow* WindowRef, ImGuiViewport* viewport)
    {
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);

		GLFWwindow* GlfwWindow= WindowRef->GetGlfwWindow();
		const bool bIsMaximized = (bool)glfwGetWindowAttrib(GlfwWindow, GLFW_MAXIMIZED);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::Begin(UI_CORE_VIEWPORT, NULL, UI::CoreViewportFlags);
		ImGui::PopStyleColor(); 
		ImGui::PopStyleVar(2);
    }

    void PushStyleVar(ImGuiStyleVar styleVar, const ImVec2& var)
    {
        ImGui::PushStyleVar(styleVar, var);
        PushedStyleVars++;
    }

    void PushStyleVar(ImGuiStyleVar styleVar, const glm::vec2& var)
    {
        ImGui::PushStyleVar(styleVar, ImVec2(var.x, var.y));
        PushedStyleVars++;
    }

    void PopStyleVar(uint8_t poppedStyleVars)
    {
        ImGui::PopStyleVar(poppedStyleVars);
        PushedStyleVars = PushedStyleVars - poppedStyleVars;
        LK_CORE_VERIFY(PushedStyleVars >= 0, "UI StyleVar Push/Pop stack invalid!");
    }

    void PushStyleColor(ImGuiCol colorVar, const ImVec4& color)
    {
        ImGui::PushStyleColor(colorVar, color);
        PushedStyleColors++;
    }

    void PushStyleColor(ImGuiCol colorVar, const glm::vec4& color)
    {
        ImGui::PushStyleColor(colorVar, ImVec4(color.r, color.g, color.b, color.a));
        PushedStyleColors++;
    }

    void PopStyleColor(uint8_t pop)
    {
        ImGui::PopStyleColor(pop);
        PushedStyleColors = PushedStyleColors - pop;
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
