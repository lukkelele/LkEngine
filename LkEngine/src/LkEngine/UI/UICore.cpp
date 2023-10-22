#include "LKpch.h"
#include "LkEngine/UI/UICore.h"
#include <imgui_internal.h>


namespace LkEngine::UI {

    static int s_UIContextID = 0;
    static uint32_t s_Counter = 0;
    static char s_IDBuffer[16] = "##";
    static char s_LabelIDBuffer[1024];

    const char* GenerateID()
    {
        _itoa_s(s_Counter++, s_IDBuffer + 2, sizeof(s_IDBuffer) - 2, 16);
        return s_IDBuffer;
    }

    void PushID()
    {
        ImGui::PushID(s_UIContextID++);
        s_Counter = 0;
    }

    void PopID()
    {
        ImGui::PopID();
        s_UIContextID--;
    }

    bool IsInputEnabled()
    {
        const auto& io = ImGui::GetIO();
        return (io.ConfigFlags & ImGuiConfigFlags_NoMouse) == 0 && (io.ConfigFlags & ImGuiConfigFlags_NavNoCaptureKeyboard) == 0;
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

}