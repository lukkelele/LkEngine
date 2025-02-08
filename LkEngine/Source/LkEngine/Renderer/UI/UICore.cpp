#include "LKpch.h"
#include "UICore.h"

#include "LkEngine/Core/Window.h"
#include "LkEngine/Core/IO/FileSystem.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Scene/Entity.h"

#include "LkEngine/Editor/EditorLayer.h"

#define LK_UI_USE_DOCKBUILDER 1

namespace LkEngine::UI {

	static bool bDockspaceInitialized = false;

    static uint32_t Counter = 0;
    static int UIContextID = 0;
    static char IDBuffer[16 + 2 + 1] = "##";
    static char LabelIDBuffer[1024 + 1];

    const char* GenerateID()
    {
		snprintf(IDBuffer + 2, 16, "%u", Counter++);
        return IDBuffer;
    }

    void PushID()
    {
        ImGui::PushID(UIContextID++);
        Counter = 0;
    }

    void PopID()
    {
        ImGui::PopID();
        UIContextID--;
    }

	int Internal::GetContextID() { return UIContextID; }

    bool IsInputEnabled()
    {
        const ImGuiIO& IO = ImGui::GetIO();
        return (((IO.ConfigFlags & ImGuiConfigFlags_NoMouse) == 0) 
				&& (IO.ConfigFlags & ImGuiConfigFlags_NavNoCaptureKeyboard) == 0);
    }

    bool IsMouseEnabled()
    {
        const ImGuiIO& IO = ImGui::GetIO();
        return (IO.ConfigFlags & ImGuiConfigFlags_NoMouse) == 0;
    }

    bool IsKeyboardEnabled()
    {
        const ImGuiIO& IO = ImGui::GetIO();
        return (IO.ConfigFlags & ImGuiConfigFlags_NavNoCaptureKeyboard) == 0;
    }

    void SetInputEnabled(const bool Enabled)
    {
        ImGuiIO& IO = ImGui::GetIO();

        if (Enabled)
        {
            IO.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
            IO.ConfigFlags &= ~ImGuiConfigFlags_NavNoCaptureKeyboard;
        }
        else
        {
            IO.ConfigFlags |= ImGuiConfigFlags_NoMouse;
            IO.ConfigFlags |= ImGuiConfigFlags_NavNoCaptureKeyboard;
        }
	}

    void Begin(const char* WindowTitle, bool* Open, ImGuiWindowFlags WindowFlags)
    {
        UI::PushID();
        ImGui::Begin(WindowTitle, Open, WindowFlags);

		if (ImGuiWindow* ThisWindow = ImGui::GetCurrentWindow(); ThisWindow != nullptr)
		{
			//if ((ThisWindow->Flags & ImGuiWindowFlags_NoCollapse) && ThisWindow->SkipItems)
			if (ThisWindow->SkipItems)
			{
				UI::End();
				return;
			}
		}
    }

    void End()
    {
        ImGui::End();
        UI::PopID();
    }
    
    void BeginViewport(TObjectPtr<LWindow> Window)
    {
		if (UI::DockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
		{
			/* TODO: Evaluate if both needed. */
		    UI::CoreViewportFlags |= ImGuiWindowFlags_NoBackground;
			UI::HostWindowFlags |= ImGuiWindowFlags_NoBackground;
		}

        ImGuiViewport* Viewport = ImGui::GetMainViewport();

		FScopedStyle WindowRounding(ImGuiStyleVar_WindowRounding, 0.0f);
		FScopedStyle WindowBorderSize(ImGuiStyleVar_WindowBorderSize, 0.0f);
		FScopedStyle WindowPadding(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		FScopedColor MenuBarBg(ImGuiCol_MenuBarBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::SetNextWindowPos(Viewport->Pos);
        ImGui::SetNextWindowSize(Viewport->Size);
        ImGui::SetNextWindowViewport(Viewport->ID);
        ImGui::Begin(PanelID::CoreViewport, nullptr, HostWindowFlags);
		ImGuiID DockspaceID = ImGui::GetID(PanelID::Dockspace);

		/**
		 * The sidebar nodes do not get the same sizes even though the size ratios
		 * for the dock splitting is the same. A 0.04 increment in the right sidebar
		 * is needed to make the sizes the same.
		 */
		if (ImGui::DockBuilderGetNode(DockspaceID) == nullptr)
		{
			LK_CORE_INFO("Creating dockspace layout");

			/* Remove existing layout. */
			LK_CORE_TRACE("Removing existing docking layout");
			ImGui::DockBuilderRemoveNode(DockspaceID);

			/* Add empty node. */
			ImGuiDockNodeFlags DockFlags = ImGuiDockNodeFlags_DockSpace 
				| ImGuiDockNodeFlags_NoWindowMenuButton;
			ImGui::DockBuilderAddNode(DockspaceID, DockFlags);
			ImGui::DockBuilderSetNodeSize(DockspaceID, Viewport->Size);

			ImGuiID DockID_Main = DockspaceID;
			ImGuiID DockID_Left = ImGui::DockBuilderSplitNode(DockID_Main, ImGuiDir_Left, 0.18f, nullptr, &DockID_Main);

			ImGuiID DockID_Right = ImGui::DockBuilderSplitNode(DockID_Main, ImGuiDir_Right, 0.22f, nullptr, &DockID_Main);
			ImGuiID DockID_Right_Top = ImGui::DockBuilderSplitNode(DockID_Right, ImGuiDir_Up, 0.52f, nullptr, &DockID_Right);

			ImGuiID DockID_Bottom = ImGui::DockBuilderSplitNode(DockID_Main, ImGuiDir_Down, 0.32f, nullptr, &DockID_Main);
			ImGuiID DockID_Bottom_Right = ImGui::DockBuilderSplitNode(DockID_Bottom, ImGuiDir_Right, 0.42f, nullptr, &DockID_Bottom);

			ImGuiID DockID_Top = ImGui::DockBuilderSplitNode(DockID_Main, ImGuiDir_Up, 0.04f, nullptr, &DockID_Main);

			ImGui::DockBuilderDockWindow(PanelID::Sidebar1,       DockID_Left);
			ImGui::DockBuilderDockWindow(PanelID::EditorViewport, DockID_Main);
			ImGui::DockBuilderDockWindow(PanelID::Sidebar2,       DockID_Right_Top);
			ImGui::DockBuilderDockWindow(PanelID::ContentBrowser, DockID_Bottom);
			ImGui::DockBuilderDockWindow(PanelID::TopBar,         DockID_Top);

			/* Dock the scene manager to the right sidebar. */
			ImGui::DockBuilderDockWindow(PanelID::SceneManager,   DockID_Right);

			/* Bottom bar. */
			ImGui::DockBuilderDockWindow(PanelID::EditorConsole,  DockID_Bottom_Right);

			/* Finish the dockspace. */
			ImGui::DockBuilderFinish(DockspaceID);

			/* Disable splitting over entire viewport. */
			if (ImGuiDockNode* DockNode = ImGui::DockBuilderGetNode(ImGui::GetID(PanelID::Dockspace)))
			{
				DockNode->LocalFlags |= ImGuiDockNodeFlags_NoDockingOverMe;
				DockNode->LocalFlags |= ImGuiDockNodeFlags_NoDockingSplit;
			}

			ImGuiDockNode* Sidebar1Node = ImGui::DockBuilderGetNode(DockID_Left);
			ImGuiDockNode* Sidebar2Node = ImGui::DockBuilderGetNode(DockID_Right_Top);
			LK_CORE_VERIFY(Sidebar1Node);
			LK_CORE_VERIFY(Sidebar2Node);

			bDockspaceInitialized = true;
		}

		if (!bDockspaceInitialized)
		{
			LK_CORE_DEBUG("Initializing dockspace layout");
			ImGuiDockNode* DockspaceNode = ImGui::DockBuilderGetNode(DockspaceID);
			LK_CORE_VERIFY(DockspaceNode, "Dockspace node is nullptr");
			DockspaceNode->LocalFlags |= ImGuiDockNodeFlags_NoDockingOverMe;
			DockspaceNode->LocalFlags |= ImGuiDockNodeFlags_NoDockingSplit;

			/* Disable docking in the central node. */
			ImGuiDockNode* CentralNode = FindCentralNode(ImGui::GetID(PanelID::Dockspace));
			LK_CORE_VERIFY(CentralNode, "Cannot find central node");
			CentralNode->LocalFlags |= ImGuiDockNodeFlags_NoDocking;

			bDockspaceInitialized = true;
		}

		/* Submit the dockspace. */
		ImGui::DockSpace(DockspaceID, ImVec2(0, 0), UI::DockspaceFlags);
		ImGui::End(); /* PanelID::CoreViewport */

		ImGui::SetNextWindowPos(Viewport->Pos);
		ImGui::SetNextWindowSize(Viewport->Size);
		ImGui::SetNextWindowViewport(Viewport->ID);
		ImGui::Begin(PanelID::CoreViewport, nullptr, UI::CoreViewportFlags);
    }

	ImGuiDockNode* FindCentralNode(const ImGuiID DockspaceID)
	{
		ImGuiDockNode* RootNode = ImGui::DockBuilderGetNode(DockspaceID);
		if (!RootNode)
		{
			return nullptr;
		}

		std::queue<ImGuiDockNode*> NodeQueue;
		NodeQueue.push(RootNode);

		while (!NodeQueue.empty())
		{
			ImGuiDockNode* CurrentNode = NodeQueue.front();
			NodeQueue.pop();

			if (CurrentNode->IsCentralNode())
			{
				return CurrentNode;
			}

			/* Add child nodes to the queue for further exploration. */
			if (CurrentNode->ChildNodes[0])
			{
				NodeQueue.push(CurrentNode->ChildNodes[0]);
			}

			if (CurrentNode->ChildNodes[1])
			{
				NodeQueue.push(CurrentNode->ChildNodes[1]);
			}
		}

		/* No central node found. */
		return nullptr;
	}

	ImTextureID GetTextureID(TObjectPtr<LTexture2D> Texture)
	{
		if (LRendererAPI::Get() == ERendererAPI::OpenGL)
		{
			const TObjectPtr<LOpenGLTexture2D> OpenGLTexture = Texture.As<LOpenGLTexture2D>();
			const GLuint TextureID = OpenGLTexture->GetRendererID();

			if (TextureID == 0)
			{
				return (ImTextureID)0;
			}

			return static_cast<ImTextureID>(TextureID);
		}

		return (ImTextureID)0;
	}

}
