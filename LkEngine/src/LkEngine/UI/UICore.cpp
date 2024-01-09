#include "LKpch.h"
#include "UICore.h"
#include "LkEngine/Core/Window.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Editor/Editor.h"



namespace LkEngine::UI {

	ImGuiWindowFlags CoreViewportFlags = ImGuiWindowFlags_NoDocking
		| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoInputs;

    ImGuiWindowFlags HostWindowFlags = ImGuiWindowFlags_NoDocking
        | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoInputs;

    ImGuiDockNodeFlags DockspaceFlags = ImGuiDockNodeFlags_None
        | ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_NoDockingOverMe
        | ImGuiDockNodeFlags_NoDockingOverOther | ImGuiDockNodeFlags_NoDockingOverEmpty
        | ImGuiDockNodeFlags_NoDockingSplitMe | ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoTabBar;

    ImGuiWindowFlags MenuBarFlags = ImGuiWindowFlags_MenuBar
        | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoCollapse;

    ImGuiWindowFlags SidebarFlags = ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus
        | ImGuiWindowFlags_NoMove;

    ImGuiWindowFlags TabBarFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking
        | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

    static uint32_t s_Counter = 0;
    static int s_UIContextID = 0;
    static char s_IDBuffer[16] = "##";
    static char s_LabelIDBuffer[1024];
    const char* SelectedEntityWindow = UI_SIDEBAR_RIGHT;
    static bool LastPushedIDWasString = false;

    const char* GenerateID()
    {
        LK_ITOA(s_Counter++, s_IDBuffer + 2, sizeof(s_IDBuffer) - 2, 16);
        return s_IDBuffer;
    }

    void PushID()
    {
        ImGui::PushID(s_UIContextID++);
        s_Counter = 0;
    }

    void PushID(const char* id)
    {
        ImGui::PushID(id);
        LastPushedIDWasString = true;
    }

    void PopID()
    {
        ImGui::PopID();
        if (LastPushedIDWasString)
        {
            LastPushedIDWasString = false;
            return;
        }
        s_UIContextID--;
    }

    void PopID(const char* id)
    {
        ImGui::PopID();
        LastPushedIDWasString = false;
    }

    bool IsInputEnabled()
    {
        const auto& io = ImGui::GetIO();
        return (io.ConfigFlags & ImGuiConfigFlags_NoMouse) == 0 && (io.ConfigFlags & ImGuiConfigFlags_NavNoCaptureKeyboard) == 0;
    }

    bool IsMouseEnabled()
    {
        const auto& io = ImGui::GetIO();
        return (io.ConfigFlags & ImGuiConfigFlags_NoMouse) == 0;
    }

    bool IsKeyboardEnabled()
    {
        const auto& io = ImGui::GetIO();
        return (io.ConfigFlags & ImGuiConfigFlags_NavNoCaptureKeyboard) == 0;
    }

    void SetInputEnabled(bool enabled)
    {
        auto& io = ImGui::GetIO();

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
        return currentNavWindow == ImGui::FindWindowByName(windowName);
    }

    void Begin(std::string windowTitle, ImGuiWindowFlags windowFlags, bool* open)
    {
        Begin(windowTitle.c_str(), windowFlags, open);
    }

    void Begin(const char* windowTitle, ImGuiWindowFlags windowFlags, bool* open)
    {
        //UI::PushID(windowTitle);
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
		auto& style = ImGui::GetStyle();

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

    void BeginViewport(const char* viewportID, Window* window, ImGuiViewport* viewport)
    {
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);

		auto* glfwWindow = window->GetGlfwWindow();
		bool isMaximized = (bool)glfwGetWindowAttrib(glfwWindow, GLFW_MAXIMIZED);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::Begin(UI_CORE_VIEWPORT, NULL, UI::CoreViewportFlags);
		ImGui::PopStyleColor(); 
		ImGui::PopStyleVar(2);
    }

}
