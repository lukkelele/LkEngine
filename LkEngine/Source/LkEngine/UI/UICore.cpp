#include "LKpch.h"
#include "UICore.h"

#include "LkEngine/Core/Window.h"
#include "LkEngine/Core/IO/FileSystem.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Scene/Entity.h"

#include "LkEngine/Editor/EditorLayer.h"


#define LK_UI_USE_DOCKBUILDER 1

namespace LkEngine::UI {

	static std::unordered_map<std::string, FMessageBox> MessageBoxes;

	static bool bDockspaceInitialized = false;

	ImGuiID DockID_EditorViewport;
	ImGuiID DockID_TopBar;
	ImGuiID DockID_Sidebar1;
	ImGuiID DockID_Sidebar2;
	ImGuiID DockID_BottomBar;

    ImGuiWindowFlags CoreViewportFlags = ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus
		| ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoInputs
		| ImGuiWindowFlags_NoDocking;

    ImGuiWindowFlags HostWindowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar
		| ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus
		| ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoDocking;

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
		| ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus
		| ImGuiWindowFlags_NoDocking;

    static uint32_t Counter = 0;
    static int UIContextID = 0;
    static char IDBuffer[16] = "##";
    static char LabelIDBuffer[1024];

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

		if (ImGui::DockBuilderGetNode(DockspaceID) == nullptr)
		{
			/* Remove existing layout. */
			LK_UI_DEBUG("Removing existing docking layout");
			ImGui::DockBuilderRemoveNode(DockspaceID);

			/* Add empty node. */
			ImGuiDockNodeFlags DockFlags = ImGuiDockNodeFlags_DockSpace;
			ImGui::DockBuilderAddNode(DockspaceID, DockFlags);
			ImGui::DockBuilderSetNodeSize(DockspaceID, Viewport->Size);

			ImGuiID DockID_Main = DockspaceID;
			ImGuiID DockID_Left = ImGui::DockBuilderSplitNode(DockID_Main, ImGuiDir_Left, 0.15f, nullptr, &DockID_Main);
			ImGuiID DockID_Right = ImGui::DockBuilderSplitNode(DockID_Main, ImGuiDir_Right, 0.15f, nullptr, &DockID_Main);
			ImGuiID DockID_Bottom = ImGui::DockBuilderSplitNode(DockID_Main, ImGuiDir_Down, 0.20f, nullptr, &DockID_Main);
			ImGuiID DockID_Top = ImGui::DockBuilderSplitNode(DockID_Main, ImGuiDir_Up, 0.04f, nullptr, &DockID_Main);

			ImGui::DockBuilderDockWindow(LK_UI_SIDEBAR_1,       DockID_Left);
			ImGui::DockBuilderDockWindow(LK_UI_EDITOR_VIEWPORT, DockID_Main);
			ImGui::DockBuilderDockWindow(LK_UI_SIDEBAR_2,       DockID_Right);
			ImGui::DockBuilderDockWindow(LK_UI_BOTTOMBAR,       DockID_Bottom);
			ImGui::DockBuilderDockWindow(LK_UI_TOPBAR,          DockID_Top);

			DockID_EditorViewport = DockID_Main;
			DockID_Sidebar1 = DockID_Left;
			DockID_Sidebar2 = DockID_Right;
			DockID_BottomBar = DockID_Bottom;

			/* Finish the dockspace. */
			LK_UI_INFO("Building dockspace");
			ImGui::DockBuilderFinish(DockspaceID);

			/* Disable splitting over entire viewport. */
			if (ImGuiDockNode* DockNode = ImGui::DockBuilderGetNode(ImGui::GetID(LK_UI_DOCKSPACE)))
			{
				DockNode->LocalFlags |= ImGuiDockNodeFlags_NoDockingOverMe;
				DockNode->LocalFlags |= ImGuiDockNodeFlags_NoDockingSplit;
			}

			/* Disable docking over editor viewport. */
			if (ImGuiDockNode* DockNode = ImGui::DockBuilderGetNode(LK_UI_DOCK_EDITOR_VIEWPORT); DockNode != nullptr)
			{
				DockNode->LocalFlags |= ImGuiDockNodeFlags_NoDocking;
			}

			bDockspaceInitialized = true;
		}

		if (!bDockspaceInitialized)
		{
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
