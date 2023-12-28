#include "LKpch.h"
#include "LkEngine/UI/UICore.h"
#include "LkEngine/Editor/EditorLayer.h"

#include <imgui_internal.h>


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

    ImGuiWindowFlags TopbarFlags = ImGuiWindowFlags_MenuBar
        | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoSavedSettings;

    ImGuiWindowFlags SidebarFlags = ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus
        | ImGuiWindowFlags_NoMove;// | ImGuiWindowFlags_NoSavedSettings;

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

    const char* GetSelectedEntityWindowName()
    {
        return SelectedEntityWindow;
    }

    void BeginSubwindow(const char* windowName, ImGuiWindowFlags windowFlags)
    {
        PushID(std::string("##" + std::string(windowName)).c_str());
        ImGui::Begin(SelectedEntityWindow, NULL, (windowFlags | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse));
        ImGui::BeginChild(windowName, ImVec2(0, 0), false, (windowFlags | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse));
    }

    void EndSubwindow()
    {
        ImGui::EndChild();
        ImGui::End();
        PopID();
    }


}