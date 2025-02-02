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

	ImGuiWindowFlags CoreViewportFlags = ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus
		| ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoInputs
		| ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoNavFocus;

	ImGuiWindowFlags HostWindowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar
		| ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus
		| ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoInputs;

	ImGuiDockNodeFlags DockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode
		| ImGuiDockNodeFlags_NoDockingInCentralNode;

    ImGuiWindowFlags MenuBarFlags = ImGuiWindowFlags_MenuBar
        | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoCollapse;

    ImGuiWindowFlags SidebarFlags = ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus
		| ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoMove;

    ImGuiWindowFlags TabBarFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse 
        | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBringToFrontOnFocus 
        | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

	ImGuiWindowFlags EditorViewportFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
		| ImGuiWindowFlags_NoResize;// | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus;

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
        ImGui::Begin(LK_UI_CORE_VIEWPORT, nullptr, HostWindowFlags);
		ImGuiID DockspaceID = ImGui::GetID(LK_UI_DOCKSPACE);

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
			ImGuiDockNodeFlags DockFlags = ImGuiDockNodeFlags_DockSpace;
			ImGui::DockBuilderAddNode(DockspaceID, DockFlags);
			ImGui::DockBuilderSetNodeSize(DockspaceID, Viewport->Size);

			ImGuiID DockID_Main = DockspaceID;
			ImGuiID DockID_Left = ImGui::DockBuilderSplitNode(DockID_Main, ImGuiDir_Left, 0.18f, nullptr, &DockID_Main);
			ImGuiID DockID_Right = ImGui::DockBuilderSplitNode(DockID_Main, ImGuiDir_Right, 0.22f, nullptr, &DockID_Main);
			ImGuiID DockID_Right_Top = ImGui::DockBuilderSplitNode(DockID_Right, ImGuiDir_Up, 0.52f, nullptr, &DockID_Right);
			ImGuiID DockID_Bottom = ImGui::DockBuilderSplitNode(DockID_Main, ImGuiDir_Down, 0.32f, nullptr, &DockID_Main);
			ImGuiID DockID_Top = ImGui::DockBuilderSplitNode(DockID_Main, ImGuiDir_Up, 0.04f, nullptr, &DockID_Main);

			ImGui::DockBuilderDockWindow(LK_UI_SIDEBAR_1,       DockID_Left);
			ImGui::DockBuilderDockWindow(LK_UI_EDITOR_VIEWPORT, DockID_Main);
			ImGui::DockBuilderDockWindow(LK_UI_SIDEBAR_2,       DockID_Right_Top);
			ImGui::DockBuilderDockWindow(LK_UI_CONTENTBROWSER,  DockID_Bottom);
			ImGui::DockBuilderDockWindow(LK_UI_TOPBAR,          DockID_Top);

			/* Dock the scene manager to the right sidebar. */
			ImGui::DockBuilderDockWindow(LK_UI_SCENEMANAGER,    DockID_Right);

			/* Dock the editor console to the bottom bar, together with the content browser. */
			ImGui::DockBuilderDockWindow(LK_UI_EDITORCONSOLE,   DockID_Bottom);
			ImGui::DockBuilderDockWindow(LK_UI_CONTENTBROWSER,  DockID_Bottom);

			/* Finish the dockspace. */
			ImGui::DockBuilderFinish(DockspaceID);

			/* Disable splitting over entire viewport. */
			if (ImGuiDockNode* DockNode = ImGui::DockBuilderGetNode(ImGui::GetID(LK_UI_DOCKSPACE)))
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
			ImGuiDockNode* CentralNode = FindCentralNode(ImGui::GetID(LK_UI_DOCKSPACE));
			LK_CORE_VERIFY(CentralNode, "Cannot find central node");
			CentralNode->LocalFlags |= ImGuiDockNodeFlags_NoDocking;

			bDockspaceInitialized = true;
		}

		/* Submit the dockspace. */
		ImGui::DockSpace(DockspaceID, ImVec2(0, 0), UI::DockspaceFlags);
		ImGui::End(); /* LK_UI_CORE_VIEWPORT */

		ImGui::SetNextWindowPos(Viewport->Pos);
		ImGui::SetNextWindowSize(Viewport->Size);
		ImGui::SetNextWindowViewport(Viewport->ID);
		ImGui::Begin(LK_UI_CORE_VIEWPORT, nullptr, UI::CoreViewportFlags);
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
