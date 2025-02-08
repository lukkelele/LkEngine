#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui-node-editor/imgui_node_editor.h>

#include <glm/glm.hpp>

#include "Font.h"
#include "LkEngine/Core/Math/Vector.h"
#include "LkEngine/Scene/Components.h"

#if defined(LK_ENGINE_OPENGL)
#	include "LkEngine/Renderer/Backend/OpenGL/OpenGLTexture.h"
#endif

#define LK_UI_DEBUG_DOCKNODES         0
#define LK_UI_DEBUG_WINDOWS_ON_HOVER  0
#include "UIDebug.h"

namespace LkEngine 
{
    class LWindow;
    class LEntity;

	namespace PanelID 
	{
		static constexpr const char* CoreViewport        = "##LkEngine-CoreViewport";
		static constexpr const char* Dockspace           = "##LkEngine-DockSpace";    
		static constexpr const char* EditorViewport      = "##LkEngine-EditorViewport";
		static constexpr const char* TopBar              = "##LkEngine-TopBar";
		static constexpr const char* BottomBar           = "##LkEngine-BottomBar";
		static constexpr const char* Sidebar1            = "Sidebar##1";
		static constexpr const char* Sidebar2            = "Sidebar##2";
		static constexpr const char* ApplicationSettings = "Application Settings";
		static constexpr const char* EditorSettings      = "Editor Settings";
		static constexpr const char* EditorConsole       = "Log##EditorConsole";
		static constexpr const char* ContentBrowser      = "Content Browser";
		static constexpr const char* SceneManager        = "Scene Manager";
		static constexpr const char* ComponentEditor     = "Component Editor";
		static constexpr const char* Tools               = "Tools";
		static constexpr const char* MaterialEditor      = "Material Editor";
		static constexpr const char* ThemeManager        = "Theme Manager";
	}

}

namespace LkEngine::UI {

	/**
	 * Map a property to a type T.
	 * 
	 *  Useful in dropdown menues.
	 *  
	 *  ----------------------
	 *   Example (uint32_t):
	 *  ----------------------
     *      constexpr std::array<UI::TPropertyMapping<uint32_t>, 2> Options = {
	 *          std::pair("White", RGBA32::White),
     *          std::pair("Black", RGBA32::Black),
	 *      };
	 */
	template<typename T>
	using TPropertyMapping = std::pair<const char*, T>;

	/** Slider configuration. */
	namespace Slider
	{
		static constexpr float MIN_UNLIMITED = 0.0f;
		static constexpr float MAX_UNLIMITED = 0.0f;
	}

	/**
	 * EStyle
	 *
	 *  Styles that can be added to UI elements.
	 */
	enum class EStyle : uint32_t
	{
		AlignHorizontal,
		COUNT
	};
	LK_ENUM_CLASS_FLAGS(EStyle);

	namespace Internal 
	{
		int GetContextID();

		struct FAlignData
		{
			float Dist = 0.0f;
			uint16_t PassedElements = 0;
		};

		struct FTableEntry
		{
			const char* ID = nullptr;
		};
	}

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

	enum class EDataType : uint32_t
	{
		None,
		S32,      /* int32_t  */
		U32,      /* uint32_t */
		Float,    /* float    */
		Double,   /* double   */
		Bool,     /* bool     */
		COUNT
	};

	struct FVariableInfo
	{
		EDataType Type = EDataType::None;
		uint32_t Count = 0; 
		uint32_t Offset = 0;

		FORCEINLINE void* GetVariablePtr(void* Parent) const 
		{ 
			return (void*)((unsigned char*)Parent + Offset); 
		}
	};
	
	struct LStyle
	{
		float AlignHorizontal = -1.0f; /* Disabled when -1.0 */
	};

	namespace Internal 		
	{
		static const FVariableInfo StyleVarInfo[] =
		{
			{ EDataType::Float, 1, (uint32_t)offsetof(LStyle, AlignHorizontal) },
		};
	}

	struct FStyleMod
	{
	private:
		using T = std::underlying_type_t<EStyle>;
	public:
		T VarIdx;
		union 
		{ 
			int ArrInt[2]; 
			float ArrFloat[2]; 
		};

		FStyleMod(const EStyle Style, const int Value)     
			: VarIdx((T)Style)
		{ 
			ArrInt[0] = Value; 
		}

		FStyleMod(const EStyle Style, const float Value)
			: VarIdx((T)Style)
		{ 
			ArrFloat[0] = Value; 
		}

		FStyleMod(const EStyle Style, const LVector2& Value)
			: VarIdx((T)Style)
		{ 
			ArrFloat[0] = Value.X; 
			ArrFloat[1] = Value.Y; 
		}
	};

	struct LUIContext
	{
		std::vector<FStyleMod> StyleStack{};
		std::deque<Internal::FTableEntry> TableStack{};
		std::deque<Internal::FAlignData> AlignedStack{};
		std::unordered_map<std::string, FMessageBox> MessageBoxes{};

		LStyle Style;
	};
	static LUIContext UIContext;

	FORCEINLINE bool InTable()
	{
		return !UIContext.TableStack.empty();
	}

	FORCEINLINE const FVariableInfo* GetVariableInfo(const EStyle Idx)
	{
		using T = std::underlying_type_t<std::decay_t<decltype(Idx)>>;
		LK_CORE_ASSERT(((T)Idx >= 0) && (Idx < (T)(EStyle::COUNT)));
		static_assert(LK_ARRAYSIZE(Internal::StyleVarInfo) == (std::underlying_type_t<EStyle>)EStyle::COUNT);
		/* TODO: Disable warning C33011. */
		return &Internal::StyleVarInfo[(T)Idx];
	}

	FORCEINLINE void PushStyle(const EStyle Style, const float Value)
	{
		const FVariableInfo* VarInfo = GetVariableInfo(Style);
		if (VarInfo->Type != EDataType::Float)
		{
		}

		float* VarPtr = (float*)VarInfo->GetVariablePtr(&UIContext.Style);
		UIContext.StyleStack.push_back(FStyleMod(Style, *VarPtr));
		*VarPtr = Value;
	}

	FORCEINLINE void PopStyle(const int StylesToPop = 1)
	{
		LK_CORE_ASSERT(false, "TODO");
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

	class FScopedStyleStack
	{
	public:

		template <typename ValueType, typename... OtherStylePairs>
		FScopedStyleStack(const ImGuiStyleVar FirstStyleVar, const ValueType FirstValue, OtherStylePairs&& ... OtherPairs)
			: StackCount((sizeof...(OtherPairs) / 2) + 1)
		{
			static_assert((sizeof...(OtherPairs) & 1u) == 0);
			PushStyle(FirstStyleVar, FirstValue, std::forward<OtherStylePairs>(OtherPairs)...);
		}

		~FScopedStyleStack() { ImGui::PopStyleVar(StackCount); }

		FScopedStyleStack(const FScopedStyleStack&) = delete;
		FScopedStyleStack& operator=(const FScopedStyleStack&) = delete;

	private:
		int StackCount = 0;

		template <typename ValueType, typename... OtherStylePairs>
		FORCEINLINE void PushStyle(const ImGuiStyleVar StyleVar, const ValueType Value, OtherStylePairs&& ... OtherPairs)
		{
			if constexpr (sizeof...(OtherPairs) == 0)
			{
				ImGui::PushStyleVar(StyleVar, Value);
			}
			else
			{
				ImGui::PushStyleVar(StyleVar, Value);
				PushStyle(std::forward<OtherStylePairs>(OtherPairs)...);
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


	/****************************
	 *          Flags           *
	 ****************************/
	inline static constexpr ImGuiDockNodeFlags DockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode
		| ImGuiDockNodeFlags_NoDockingInCentralNode;

	inline static ImGuiWindowFlags CoreViewportFlags = ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus
		| ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoInputs
		| ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoNavFocus;

	inline static ImGuiWindowFlags HostWindowFlags = ImGuiWindowFlags_NoTitleBar 
		| ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus
		| ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoInputs;

    inline static constexpr ImGuiWindowFlags MenuBarFlags = ImGuiWindowFlags_MenuBar
        | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar 
		| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoCollapse;

    inline static constexpr ImGuiWindowFlags SidebarFlags = ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus
		| ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoMove;

    inline static constexpr ImGuiWindowFlags TabBarFlags = ImGuiWindowFlags_NoTitleBar 
		| ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking 
		| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

	inline static constexpr ImGuiWindowFlags EditorViewportFlags = ImGuiWindowFlags_NoTitleBar 
		| ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar 
		| ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize;

}


