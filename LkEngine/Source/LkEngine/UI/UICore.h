#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui-node-editor/imgui_node_editor.h>

#include <glm/glm.hpp>

#include "Font.h"
#include "LkEngine/Scene/Components.h"

#if defined(LK_ENGINE_OPENGL)
#	include "LkEngine/Renderer/Backend/OpenGL/OpenGLTexture.h"
#endif

#define LK_MESSAGE_BOX_OK_BUTTON		LK_BIT(0)
#define LK_MESSAGE_BOX_CANCEL_BUTTON	LK_BIT(1)
#define LK_MESSAGE_BOX_USER_FUNCTION	LK_BIT(2)
#define LK_MESSAGE_BOX_AUTO_SIZE		LK_BIT(3)

/* UI Identifiers. */
#define LK_UI_CORE_VIEWPORT          ::LkEngine::UI::ID::CoreViewport
#define LK_UI_EDITOR_VIEWPORT        ::LkEngine::UI::ID::ViewportTexture
#define LK_UI_DOCKSPACE              ::LkEngine::UI::ID::Dockspace
#define LK_UI_TOPBAR                 ::LkEngine::UI::ID::TopBar
#define LK_UI_SIDEBAR_1              ::LkEngine::UI::ID::Sidebar1
#define LK_UI_SIDEBAR_2              ::LkEngine::UI::ID::Sidebar2
#define LK_UI_BOTTOMBAR              ::LkEngine::UI::ID::BottomBar
#define LK_UI_SCENEMANAGER           ::LkEngine::PanelID::SceneManager

#define LK_UI_DOCK_EDITOR_VIEWPORT   ::LkEngine::UI::DockID_EditorViewport
#define LK_UI_DOCK_TOPBAR            ::LkEngine::UI::DockID_TopBar
#define LK_UI_DOCK_SIDEBAR_1         ::LkEngine::UI::DockID_Sidebar1
#define LK_UI_DOCK_SIDEBAR_2         ::LkEngine::UI::DockID_Sidebar2
#define LK_UI_DOCK_BOTTOMBAR         ::LkEngine::UI::DockID_BottomBar

#define LK_UI_DEBUG_DOCKNODE(Name) \
		if (ImGuiDockNode* DockNode = ImGui::DockBuilderGetNode(ImGui::GetWindowDockID())) \
		{ \
			ImGui::Text("Dock ID: %lld", DockNode->ID); \
			if (DockNode->ParentNode) ImGui::Text("Parent Node: %lld", DockNode->ParentNode->ID); \
			else ImGui::Text("Parent Node: None"); \
			ImGui::Text("Windows: %d", DockNode->Windows.Size); \
			ImGui::Text("Dock Size: (%.2f, %.2f)", DockNode->Size.x, DockNode->Size.y); \
			ImGui::Text("Dock Pos: (%.2f, %.2f)", DockNode->Pos.x, DockNode->Pos.y); \
			ImGui::Separator(); \
		} 


namespace LkEngine 
{
    class LWindow;
    class LEntity;

	namespace PanelID 
	{
		static constexpr const char* ApplicationSettings = "ApplicationSettings";
		static constexpr const char* ContentBrowser      = "ContentBrowser";
		static constexpr const char* SceneManager        = "SceneManager";
		static constexpr const char* EditorConsole       = "EditorConsole";
		static constexpr const char* ComponentEditor     = "ComponentEditor";
	}

}

namespace LkEngine::UI {

	namespace ID 
	{
		static constexpr const char* CoreViewport       = "##LkEngine-CoreViewport";
		static constexpr const char* Dockspace          = "##LkEngine-DockSpace";    
		static constexpr const char* RenderWindow       = "##LkEngine-RenderWindow";
		static constexpr const char* ViewportTexture    = "##LkEngine-ViewportTexture";
		static constexpr const char* TopBar             = "##LkEngine-TopBar";
		static constexpr const char* Sidebar1           = "Sidebar##1";
		static constexpr const char* Sidebar2           = "Sidebar##2";
		static constexpr const char* BottomBar          = "BottomBar##1";
		static constexpr const char* SelectedEntityInfo = "##LkEngine-Selected-Entity-Info"; /* REMOVE */
	}
	
	class FScopedStyle
	{
	public:
		template<typename T>
		FScopedStyle(const ImGuiStyleVar StyleVar, T Value) { ImGui::PushStyleVar(StyleVar, Value); }
		~FScopedStyle() { ImGui::PopStyleVar(); }
		FScopedStyle(const FScopedStyle&) = delete;
		FScopedStyle& operator=(const FScopedStyle&) = delete;
	};

	class FScopedColor
	{
	public:
		template<typename T>
		FScopedColor(const ImGuiCol ColorID, T Color) { ImGui::PushStyleColor(ColorID, ImColor(Color).Value); }
		~FScopedColor() { ImGui::PopStyleColor(); }
		FScopedColor(const FScopedColor&) = delete;
		FScopedColor& operator=(const FScopedColor&) = delete;
	};

	/* TODO: Reevaluate this. */
    /* UI Flags. */
    extern ImGuiWindowFlags		CoreViewportFlags;
    extern ImGuiWindowFlags		HostWindowFlags;
    extern ImGuiWindowFlags		SidebarFlags;
    extern ImGuiWindowFlags		MenuBarFlags;
    extern ImGuiWindowFlags		TabBarFlags;
    extern ImGuiWindowFlags		EditorViewportFlags;
    extern ImGuiDockNodeFlags	DockspaceFlags; 

	extern ImGuiID DockID_EditorViewport;
	extern ImGuiID DockID_TopBar;
	extern ImGuiID DockID_Sidebar1;
	extern ImGuiID DockID_Sidebar2;
	extern ImGuiID DockID_BottomBar;

	/**
	 * @struct FMessageBox
	 */
	struct FMessageBox
	{
		std::string Title = "";
		std::string Body = "";
		uint32_t Flags = 0;
		uint32_t Width = 0;
		uint32_t Height = 0;
		uint32_t MinWidth = 0;
		uint32_t MinHeight = 0;
		uint32_t MaxWidth = -1;
		uint32_t MaxHeight = -1;
		std::function<void()> UserRenderFunction;
		bool bShouldOpen = true;
		bool bIsOpen = false;
	};

    const char* GenerateID();
    void PushID();
    void PushID(const char* ID);
    void PopID();
    void PopID(const char* ID);
    bool IsInputEnabled();
    bool IsMouseEnabled();
    bool IsKeyboardEnabled();
    void SetInputEnabled(bool Enabled);

    void Begin(const char* WindowTitle, bool* Open = nullptr, const ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_None);
    void End();

	void BeginViewport(TObjectPtr<LWindow> Window);

	ImGuiDockNode* FindCentralNode(const ImGuiID DockspaceID);

	/**
	 * EFindType
	 * 
	 *  Identifier type to use when attempting to find an object.
	 */
	enum class EFindType
	{
		ID,
		Name,
	};

    /**
     * @brief Check if a window is focused, defaults to use the window name.
     */
	template<EFindType FindType = EFindType::Name, typename T = const char*>
    FORCEINLINE static bool IsWindowFocused(T Identifier, const bool CheckRootWindow = true)
	{
		LK_CORE_ASSERT(false);
		return false;
	}

	template<>
    FORCEINLINE static bool IsWindowFocused(const char* WindowName, const bool CheckRootWindow)
    {
        ImGuiWindow* CurrentNavWindow = GImGui->NavWindow;
        if (CheckRootWindow)
        {
			/* Get the nav window and not something else (like a table). */
            ImGuiWindow* LastWindow = nullptr;
            while (LastWindow != CurrentNavWindow)
            {
                LastWindow = CurrentNavWindow;
                CurrentNavWindow = CurrentNavWindow->RootWindow;
            }
        }

        return (CurrentNavWindow == ImGui::FindWindowByName(WindowName));
    }

	template<>
    FORCEINLINE static bool IsWindowFocused(const ImGuiID ID, const bool CheckRootWindow)
    {
        ImGuiWindow* CurrentNavWindow = GImGui->NavWindow;
        if (CheckRootWindow)
        {
			/* Get the nav window and not something else (like a table). */
            ImGuiWindow* LastWindow = nullptr;
            while (LastWindow != CurrentNavWindow)
            {
                LastWindow = CurrentNavWindow;
                CurrentNavWindow = CurrentNavWindow->RootWindow;
            }
        }

        return (CurrentNavWindow == ImGui::FindWindowByID(ID));
    }

    /**
     * @brief Check if a window is docked, defaults to use the window name.
     */
	template<EFindType FindType = EFindType::Name, typename T = const char*>
	FORCEINLINE static bool IsWindowDocked(T Identifier)
	{
		LK_CORE_ASSERT(false);
		return false;
	}

	template<>
	FORCEINLINE static bool IsWindowDocked<EFindType::ID>(const ImGuiID ID)
	{
		if (ImGuiWindow* Window = ImGui::FindWindowByID(ID); Window != nullptr)
		{
			return (Window->DockNode != nullptr);
		}

		return false;
	}

	template<>
	FORCEINLINE static bool IsWindowDocked<EFindType::Name>(const char* WindowName)
	{
		if (ImGuiWindow* Window = ImGui::FindWindowByName(WindowName); Window != nullptr)
		{
			return (Window->DockNode != nullptr);
		}

		return false;
	}

    void Image(const TObjectPtr<LTexture2D>& texture, 
               const ImVec2& Size, 
               const ImVec2& UV0 = ImVec2(0, 0), 
               const ImVec2& UV1 = ImVec2(1, 1), 
               const ImVec4& TintColor = ImVec4(1, 1, 1, 1), 
               const ImVec4& BorderColor = ImVec4(0, 0, 0, 0));

    void Image(const TObjectPtr<LTexture2D>& Image, 
               const glm::vec2& Size, 
               const glm::vec2& UV0 = glm::vec2(0, 0), 
               const glm::vec2& UV1 = glm::vec2(1, 1), 
               const glm::vec4& TintColor = glm::vec4(1, 1, 1, 1), 
               const glm::vec4& BorderColor = glm::vec4(0, 0, 0, 0));

    void Image(const TObjectPtr<LImage2D>& Image, 
               const ImVec2& Size, 
               const ImVec2& UV0 = ImVec2(0, 0), 
               const ImVec2& UV1 = ImVec2(1, 1), 
               const ImVec4& TintColor = ImVec4(1, 1, 1, 1), 
               const ImVec4& BorderColor = ImVec4(0, 0, 0, 0));

    void Image(const TObjectPtr<LImage2D>& Image, 
               const glm::vec2& Size, 
               const glm::vec2& UV0 = glm::vec2(0, 0), 
               const glm::vec2& UV1 = glm::vec2(1, 1), 
               const glm::vec4& TintColor = glm::vec4(1, 1, 1, 1), 
               const glm::vec4& BorderColor = glm::vec4(0, 0, 0, 0));

	void RenderMessageBoxes();
	void ShowMessageBox(const char* Title,
						const std::function<void()>& RenderFunction,
						const uint32_t Width = 600,
						const uint32_t Height = 0,
						const uint32_t MinWidth = 0,
						const uint32_t MinHeight = 0,
						const uint32_t MaxWidth = -1,
						const uint32_t MaxHeight = -1,
						uint32_t Flags = LK_MESSAGE_BOX_AUTO_SIZE);

	ImTextureID GetTextureID(TObjectPtr<LTexture2D> Texture);

	inline void DrawButtonImage(const TObjectPtr<LTexture2D>& ImageNormal, 
								const TObjectPtr<LTexture2D>& ImageHovered, 
								const TObjectPtr<LTexture2D>& ImagePressed, 
								ImU32 tintNormal, 
								ImU32 tintHovered, 
								ImU32 tintPressed, 
								ImVec2 RectMin, 
								ImVec2 RectMax)
	{
		ImDrawList* DrawList = ImGui::GetWindowDrawList();
		if (ImGui::IsItemActive())
		{
			DrawList->AddImage(GetTextureID(ImagePressed), RectMin, RectMax, ImVec2(0, 0), ImVec2(1, 1), tintPressed);
		}
		else if (ImGui::IsItemHovered())
		{
			DrawList->AddImage(GetTextureID(ImageHovered), RectMin, RectMax, ImVec2(0, 0), ImVec2(1, 1), tintHovered);
		}
		else
		{
			DrawList->AddImage(GetTextureID(ImageNormal), RectMin, RectMax, ImVec2(0, 0), ImVec2(1, 1), tintNormal);
		}
	};

	inline void DrawButtonImage(const TObjectPtr<LTexture2D>& Image, ImU32 TintNormal, ImU32 TintHovered, ImU32 TintPressed, ImRect Rectangle)
	{
		DrawButtonImage(Image, Image, Image, TintNormal, TintHovered, TintPressed, Rectangle.Min, Rectangle.Max);
	};


	/* MOVE THESE UTILITY FUNCTIONS ELSEWHERE */
	inline ImRect GetItemRect()
	{
		return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
	}

	inline ImRect RectExpanded(const ImRect& Rect, const float x, const float y)
	{
		ImRect Result = Rect;
		Result.Min.x -= x;
		Result.Min.y -= y;
		Result.Max.x += x;
		Result.Max.y += y;
		return Result;
	}

	inline ImRect RectOffset(const ImRect& Rect, const float x, const float y)
	{
		ImRect Result = Rect;
		Result.Min.x += x;
		Result.Min.y += y;
		Result.Max.x += x;
		Result.Max.y += y;
		return Result;
	}

}


