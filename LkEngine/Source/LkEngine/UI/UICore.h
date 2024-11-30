#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui-node-editor/imgui_node_editor.h>

#include <glm/glm.hpp>

#include "LkEngine/Scene/Components.h"

#define LK_MESSAGE_BOX_OK_BUTTON		LK_BIT(0)
#define LK_MESSAGE_BOX_CANCEL_BUTTON	LK_BIT(1)
#define LK_MESSAGE_BOX_USER_FUNCTION	LK_BIT(2)
#define LK_MESSAGE_BOX_AUTO_SIZE		LK_BIT(3)


namespace LkEngine 
{
    class LWindow;
    class LEntity;

    static constexpr const char* LkEngine_DockSpace = "##LkEngine-DockSpace";
    
	/// UPDATE/REFACTOR
    static constexpr const char* UI_CORE_VIEWPORT = "##lkengine-core-Viewport";
    static constexpr const char* UI_RENDER_WINDOW = "##lkengine-render-window";
    static constexpr const char* UI_TOP_BAR = "##lkengine-top-bar";
    static constexpr const char* UI_BOTTOM_BAR = "##lkengine-lower-bar";
    static constexpr const char* UI_SELECTED_ENTITY_INFO = "##lkengine-selected-entity-info";
}

namespace LkEngine::UI {

    static constexpr const char* Sidebar1 = "##LkEngine-SIDEBAR-1";
    static constexpr const char* Sidebar2 = "##LkEngine-SIDEBAR-2";

    /* UI Flags. */
    extern ImGuiWindowFlags		CoreViewportFlags;
    extern ImGuiWindowFlags		HostWindowFlags;
    extern ImGuiWindowFlags		SidebarFlags;
    extern ImGuiWindowFlags		SidebarDockspaceFlags;
    extern ImGuiWindowFlags		MenuBarFlags;
    extern ImGuiWindowFlags		TabBarFlags;
    extern ImGuiWindowFlags		ViewportTextureFlags;
    extern ImGuiDockNodeFlags	DockspaceFlags; 

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
    void Separator(ImVec2 Size, ImVec4 Color);
    bool IsInputEnabled();
    bool IsMouseEnabled();
    bool IsKeyboardEnabled();
    void SetInputEnabled(bool Enabled);

    void Begin(std::string WindowTitle, const ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_None, bool* Open = nullptr);
    void Begin(const char* WindowTitle, const ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_None, bool* Open = nullptr);
    void Begin(ImGuiWindowFlags WindowFlags, bool* Open = nullptr);

    void End();

    /* Core Viewport, also known as Main Window */
    void BeginCoreViewport();
    void EndCoreViewport();

    /// REFACTOR
    /**
     * Insert UI widgets inside already existing windows on the screen.
     * Is determined by SelectedEntityWindow
     */
    bool IsWindowFocused(const char* WindowName, const bool CheckRootWindow);
    //const char* GetSelectedEntityWindowName();

    void BeginDockSpace(const char* DockspaceID);
    void BeginViewport(const char* ViewportID, LWindow* WindowRef, ImGuiViewport* Viewport);

    /// FIXME
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
    // ~

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

    constexpr const char* VIEWPORT_TEXTURE = "LkEngine-ViewportTexture";

    void PushStyleVar(const ImGuiStyleVar StyleVar, const ImVec2& Var);
    void PushStyleVar(const ImGuiStyleVar StyleVar, const glm::vec2& Var);
    void PopStyleVar(uint8_t VarsToPop = 1);

    void PushStyleColor(const ImGuiCol ColorVar, const ImVec4& Color);
    void PushStyleColor(const ImGuiCol ColorVar, const glm::vec4& Color);
    void PopStyleColor(const uint8_t VarsToPop = 1);
    void PopStyleStack();

	enum class EFontSize
	{
		Regular = 0,
		Smaller,
		Small,
		Large,
		Larger,
		Title,
		Header,
	};

	enum class EFont
	{
		Default = 0,
		SourceSansPro,
		Roboto,
	};

	struct FFontConfiguration
	{
		std::string FontName{};
		std::string_view FilePath{};
		float Size = 18.0f;

		bool MergeWithLast = false;
		const ImWchar* GlyphRanges = nullptr;
	};

	struct FFontEntry
	{
		std::string Name{};

		bool operator==(const FFontEntry& Other) const
		{
			return (Name == Other.Name);
		}
	};

	namespace Font 
	{
		void Add(const FFontConfiguration& FontConfig, bool IsDefault = false);
		void Push(const std::string& FontName);
		void Pop();
		ImFont* Get(const std::string& FontName);
	}

}

namespace std 
{
    template<>
    struct hash<LkEngine::UI::FFontEntry>
    {
        std::size_t operator()(const LkEngine::UI::FFontEntry& Entry) const noexcept
        {
			std::size_t Hash = 0;
            for (const char Character : Entry.Name)
            {
				/* Common hash multiplier. */
                Hash = (Hash * 31 + static_cast<unsigned char>(Character));
            }

            return Hash;
        }
    };
}

