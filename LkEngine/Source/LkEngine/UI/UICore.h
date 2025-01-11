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


/* UI Identifiers. */
#define LK_UI_CORE_VIEWPORT          ::LkEngine::UI::ID::CoreViewport
#define LK_UI_EDITOR_VIEWPORT        ::LkEngine::UI::ID::ViewportTexture
#define LK_UI_DOCKSPACE              ::LkEngine::UI::ID::Dockspace
#define LK_UI_TOPBAR                 ::LkEngine::UI::ID::TopBar
#define LK_UI_SIDEBAR_1              ::LkEngine::UI::ID::Sidebar1
#define LK_UI_SIDEBAR_2              ::LkEngine::UI::ID::Sidebar2
#define LK_UI_BOTTOMBAR              ::LkEngine::UI::ID::BottomBar
#define LK_UI_SCENEMANAGER           ::LkEngine::PanelID::SceneManager
#define LK_UI_CONTENTBROWSER         ::LkEngine::PanelID::ContentBrowser

/* TODO: Move debug macros elsewhere. */
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


#define LK_UI_DEBUG_ON_HOVER(...)                                                   \
		if (Debug::UI::bDisplayWindowSizeOnHover)                                   \
		{                                                                           \
			if (ImGui::IsWindowHovered())                                           \
			{                                                                       \
				const ImVec2 WindowSize = ImGui::GetWindowSize();                   \
				ImGui::SetTooltip("Size (%.2f, %.2f)", WindowSize.x, WindowSize.y); \
			}                                                                       \
		}                                                                           


namespace LkEngine 
{
    class LWindow;
    class LEntity;

	namespace PanelID 
	{
		static constexpr const char* ApplicationSettings = "Application Settings";
		static constexpr const char* EditorSettings      = "Editor Settings";
		static constexpr const char* ContentBrowser      = "Content Browser";
		static constexpr const char* SceneManager        = "Scene Manager";
		static constexpr const char* EditorConsole       = "Editor Console";
		static constexpr const char* ComponentEditor     = "Component Editor";
		static constexpr const char* Tools               = "Tools";
	}

	/**
	 * EVectorAxis
	 */
	enum class EVectorAxis : uint32_t
	{
		X = LK_BIT(0),
		Y = LK_BIT(1),
		Z = LK_BIT(2),
		W = LK_BIT(3)
	};
	LK_ENUM_CLASS_FLAGS(EVectorAxis);

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
		static constexpr const char* BottomBar          = "##LkEngine-BottomBar";
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

	class FScopedID
	{
	public:
		template<typename T>
		FScopedID(const T ID) { ImGui::PushID(ID); }
		~FScopedID() { ImGui::PopID(); }
		FScopedID(const FScopedID&) = delete;
		FScopedID& operator=(const FScopedID&) = delete;
	};

	class FScopedColorStack
	{
	public:
		template <typename ColorType, typename... OtherColors>
		FScopedColorStack(ImGuiCol FirstColorID, ColorType FirstColor, OtherColors&& ... OtherColorPairs)
			: Count((sizeof... (OtherColorPairs) / 2) + 1)
		{
			static_assert((sizeof... (OtherColorPairs) & 1u) == 0, "FScopedColorStack expects a list of pairs of color IDs and colors");
			PushColor(FirstColorID, FirstColor, std::forward<OtherColors>(OtherColorPairs)...);
		}

		~FScopedColorStack() { ImGui::PopStyleColor(Count); }

		FScopedColorStack(const FScopedColorStack&) = delete;
		FScopedColorStack& operator=(const FScopedColorStack&) = delete;

	private:
		int Count = 0;

		template <typename ColorType, typename... OtherColors>
		void PushColor(ImGuiCol ColorID, ColorType Color, OtherColors&& ... OtherColorPairs)
		{
			if constexpr (sizeof... (OtherColorPairs) == 0)
			{
				ImGui::PushStyleColor(ColorID, ImColor(Color).Value);
			}
			else
			{
				ImGui::PushStyleColor(ColorID, ImColor(Color).Value);
				PushColor(std::forward<OtherColors>(OtherColorPairs)...);
			}
		}
	};

	class FScopedFont
	{
	public:
		FScopedFont(ImFont* Font) { ImGui::PushFont(Font); }
		~FScopedFont() { ImGui::PopFont(); }
		FScopedFont(const FScopedFont&) = delete;
		FScopedFont& operator=(const FScopedFont&) = delete;
	};

    const char* GenerateID();
    void PushID();
    void PopID();
    bool IsInputEnabled();
    bool IsMouseEnabled();
    bool IsKeyboardEnabled();
    void SetInputEnabled(bool Enabled);

    void Begin(const char* WindowTitle, bool* Open = nullptr, const ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_None);
    void End();

	void BeginViewport(TObjectPtr<LWindow> Window);

	ImTextureID GetTextureID(TObjectPtr<LTexture2D> Texture);
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

    /**
     * @brief Check if a window is docked, defaults to use the window name.
     */
	template<EFindType FindType = EFindType::Name, typename T = const char*>
	FORCEINLINE static ImVec2 GetWindowSize(T Identifier)
	{
		LK_CORE_ASSERT(false);
		return {};
	}

	template<>
	FORCEINLINE static ImVec2 GetWindowSize<EFindType::ID>(const ImGuiID ID)
	{
		if (ImGuiWindow* Window = ImGui::FindWindowByID(ID); Window != nullptr)
		{
			return Window->Size;
		}

		return {};
	}

	template<>
	FORCEINLINE static ImVec2 GetWindowSize<EFindType::Name>(const char* WindowName)
	{
		if (ImGuiWindow* Window = ImGui::FindWindowByName(WindowName); Window != nullptr)
		{
			return Window->Size;
		}

		return {};
	}

	template<EFindType FindType = EFindType::Name, typename T = const char*>
	FORCEINLINE static bool IsWindowHovered(T Identifier)
	{
		LK_CORE_ASSERT(false);
		return false;
	}

	template<>
	FORCEINLINE static bool IsWindowHovered<EFindType::Name>(const char* WindowName)
	{
		if (ImGuiWindow* Window = ImGui::FindWindowByName(WindowName); Window != nullptr)
		{
			ImGuiContext& G = *ImGui::GetCurrentContext();
			return Window == G.HoveredWindow;
		}

		return false;
    }

	template<>
	FORCEINLINE static bool IsWindowHovered<EFindType::ID>(const ImGuiID ID)
	{
		if (ImGuiWindow* Window = ImGui::FindWindowByID(ID); Window != nullptr)
		{
			ImGuiContext& G = *ImGui::GetCurrentContext();
			return Window == G.HoveredWindow;
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


	/* TODO: Reevaluate this. */
    /* UI Flags. */
    extern ImGuiWindowFlags		CoreViewportFlags;
    extern ImGuiWindowFlags		HostWindowFlags;
    extern ImGuiWindowFlags		SidebarFlags;
    extern ImGuiWindowFlags		MenuBarFlags;
    extern ImGuiWindowFlags		TabBarFlags;
    extern ImGuiWindowFlags		EditorViewportFlags;
    extern ImGuiDockNodeFlags	DockspaceFlags; 

}


