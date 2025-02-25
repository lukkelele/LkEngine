#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
//#include <imgui-node-editor/imgui_node_editor.h>

#include <glm/glm.hpp>

#include "Font.h"
#include "LkEngine/Core/Math/Vector.h"
#include "LkEngine/Scene/Components.h"

#if defined(LK_ENGINE_OPENGL)
#	include "LkEngine/Renderer/Backend/OpenGL/OpenGLTexture.h"
#endif

#include "ImGuiFwd.h"

#define LK_UI_DEBUG_DOCKNODES         0
#define LK_UI_DEBUG_WINDOWS_ON_HOVER  0
#include "UIDebug.h"

#include "Style.h"


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
		static constexpr const char* NodeEditor          = "Node Editor";
	}

	enum class EMessageBoxFlag : uint32_t
	{
		OkButton     = LK_BIT(0),
		CancelButton = LK_BIT(1),
		UserFunction = LK_BIT(2),
		AutoSize     = LK_BIT(3),
	};
	LK_ENUM_CLASS_FLAGS(EMessageBoxFlag);
	LK_ENUM_RANGE_FLAGS_BY_FIRST_AND_LAST(EMessageBoxFlag, EMessageBoxFlag::OkButton, EMessageBoxFlag::AutoSize);

}

namespace LkEngine::UI {

	LK_DECLARE_MULTICAST_DELEGATE(FOnMessageBoxCancelled, const char* /* UI Element Name */);
	extern FOnMessageBoxCancelled OnMessageBoxCancelled;

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

	enum class EBorder
	{
		None       = 0,
		Horizontal = LK_BIT(1),
		Vertical   = LK_BIT(2),
		All        = LK_BIT(3),
	};
	LK_ENUM_CLASS_FLAGS(EBorder);
	/* FIXME: Need to fix the namespace nesting issue for LK_ENUM_RANGE_FLAGS */
	//LK_ENUM_RANGE_FLAGS_BY_FIRST_AND_LAST(EBorder, EBorder::None, EBorder::Horizontal)

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

	template<typename T>
	struct SizeConstraint
	{
		static constexpr T Min;
		static constexpr T Max;
	};
	
	template<>
	struct SizeConstraint<ImVec2>
	{
		static constexpr ImVec2 Min{ 0.0f, 0.0f };
		static constexpr ImVec2 Max{ 9999.0f, 9999.0f };
	};

	namespace Internal 
	{
		enum class EDataType : uint32_t
		{
			None,
			S32,     /* int32_t  */
			U32,     /* uint32_t */
			Float,   /* float    */
			Double,  /* double   */
			Bool,    /* bool     */
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
	
		static const FVariableInfo StyleVarInfo[] =
		{
			{ EDataType::Float, 1, (uint32_t)offsetof(LStyle, AlignHorizontal) },
		};

		struct FAlignData
		{
			float Dist = 0.0f;
			uint16_t PassedElements = 0;
		};

		FORCEINLINE const FVariableInfo* GetVariableInfo(const EStyle Idx)
		{
			using T = std::underlying_type_t<std::decay_t<decltype(Idx)>>;
			LK_CORE_ASSERT(((T)Idx >= 0) && (Idx < (T)(EStyle::COUNT)));
			static_assert(LK_ARRAYSIZE(Internal::StyleVarInfo) == (std::underlying_type_t<EStyle>)EStyle::COUNT);
			return &Internal::StyleVarInfo[(T)Idx];
		}

		int GetContextID();
	}

	struct LUIContext
	{
		std::vector<FStyleMod> StyleStack{};
		std::unordered_map<std::string, FMessageBox> MessageBoxes{};
		bool bInGrid = false;

		LStyle Style;
	};
	static LUIContext UIContext;

	FORCEINLINE void PushStyle(const EStyle Style, const float Value)
	{
		const Internal::FVariableInfo* VarInfo = Internal::GetVariableInfo(Style);
		if (VarInfo->Type != Internal::EDataType::Float)
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

	FORCEINLINE bool InTable()
	{
		return (ImGui::GetCurrentTable() != nullptr);
	}

	void ShowMessageBox(const char* Title,
						const std::function<void()>& RenderFunction,
						uint32_t Flags = (uint32_t)EMessageBoxFlag::AutoSize,
						const uint32_t Width = 600,
						const uint32_t Height = 0,
						const uint32_t MinWidth = 140,
						const uint32_t MinHeight = 60,
						const uint32_t MaxWidth = -1,
						const uint32_t MaxHeight = -1);

	void RenderMessageBoxes();

    const char* GenerateID();
    void PushID();
    void PopID();

	void Separator(const ImVec2& Size, const ImVec4& Color);

    bool IsInputEnabled();
    bool IsMouseEnabled();
    bool IsKeyboardEnabled();
    void SetInputEnabled(const bool Enabled);
	void SetMouseEnabled(const bool Enabled);

    bool Begin(const char* WindowTitle, bool* Open = nullptr, const ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_None);
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

	template<EFindType FindType = EFindType::Name, typename T = const char*>
	FORCEINLINE static bool IsWindowAbove(T Window1, T Window2)
	{
		LK_CORE_ASSERT(false);
		return false;
	}

	template<>
	FORCEINLINE static bool IsWindowAbove<EFindType::Name>(const char* Window1Name, const char* Window2Name)
	{
		ImGuiWindow* Window1 = ImGui::FindWindowByName(Window1Name);
		ImGuiWindow* Window2 = ImGui::FindWindowByName(Window2Name);
		if (Window1 && !Window2)
		{
			return true;
		}
		if (!Window1 && Window2)
		{
			return false;
		}

		ImGuiWindow* CurrentWindow = nullptr;
		ImGuiContext& G = *ImGui::GetCurrentContext();
		for (int Idx = G.Windows.Size - 1; Idx >= 0; Idx)
		{
			CurrentWindow = G.Windows[Idx];
			if (CurrentWindow == Window1)
			{
				return true;
			}
			if (CurrentWindow == Window2)
			{
				return false;
			}
		}

		return false;
    }

	template<>
	FORCEINLINE static bool IsWindowAbove<EFindType::ID>(const ImGuiID Window1ID, const ImGuiID Window2ID)
	{
		ImGuiWindow* Window1 = ImGui::FindWindowByID(Window1ID);
		ImGuiWindow* Window2 = ImGui::FindWindowByID(Window2ID);
		if (Window1 && !Window2)
		{
			return true;
		}
		if (!Window1 && Window2)
		{
			return false;
		}

		ImGuiWindow* CurrentWindow = nullptr;
		ImGuiContext& G = *ImGui::GetCurrentContext();
		for (int Idx = G.Windows.Size - 1; Idx >= 0; Idx)
		{
			CurrentWindow = G.Windows[Idx];
			if (CurrentWindow == Window1)
			{
				return true;
			}
			if (CurrentWindow == Window2)
			{
				return false;
			}
		}

		return false;
    }

	template<EFindType FindType = EFindType::Name, typename T = const char*>
	FORCEINLINE static bool IsAnyWindowAbove(T Window)
	{
		LK_CORE_ASSERT(false);
		return false;
	}

	template<>
	FORCEINLINE static bool IsAnyWindowAbove<EFindType::Name>(const char* WindowName)
	{
		ImGuiWindow* Window = ImGui::FindWindowByName(WindowName);
		if (!Window)
		{
			return false;
		}

		ImGuiContext& G = *ImGui::GetCurrentContext();
		return ((G.WindowsFocusOrder.Size > 0) && (G.WindowsFocusOrder[G.WindowsFocusOrder.Size - 1]));
    }

	template<>
	FORCEINLINE static bool IsAnyWindowAbove<EFindType::ID>(const ImGuiID WindowID)
	{
		ImGuiWindow* Window = ImGui::FindWindowByID(WindowID);
		if (!Window)
		{
			return false;
		}

		ImGuiContext& G = *ImGui::GetCurrentContext();
		return ((G.WindowsFocusOrder.Size > 0) && (G.WindowsFocusOrder[G.WindowsFocusOrder.Size - 1]));
    }

	template<EFindType FindType = EFindType::Name, typename T = const char*>
	FORCEINLINE static ImGuiWindow* GetWindowAbove(T Window1, T Window2)
	{
		LK_CORE_ASSERT(false);
		return nullptr;
	}

	template<>
	FORCEINLINE static ImGuiWindow* GetWindowAbove<EFindType::Name>(const char* Window1Name, const char* Window2Name)
	{
		ImGuiWindow* Window1 = ImGui::FindWindowByName(Window1Name);
		ImGuiWindow* Window2 = ImGui::FindWindowByName(Window2Name);
		if (Window1 && !Window2)
		{
			return Window1;
		}
		if (!Window1 && Window2)
		{
			return Window2;
		}

		ImGuiWindow* CurrentWindow = nullptr;
		ImGuiContext& G = *ImGui::GetCurrentContext();
		for (int Idx = G.Windows.Size - 1; Idx >= 0; Idx)
		{
			CurrentWindow = G.Windows[Idx];
			if (CurrentWindow == Window1)
			{
				return Window1;
			}
			if (CurrentWindow == Window2)
			{
				return Window2;
			}
		}

		return nullptr;
    }

	template<>
	FORCEINLINE static ImGuiWindow* GetWindowAbove<EFindType::ID>(const ImGuiID Window1ID, const ImGuiID Window2ID)
	{
		ImGuiWindow* Window1 = ImGui::FindWindowByID(Window1ID);
		ImGuiWindow* Window2 = ImGui::FindWindowByID(Window2ID);
		if (Window1 && !Window2)
		{
			return Window1;
		}
		if (!Window1 && Window2)
		{
			return Window2;
		}

		ImGuiWindow* CurrentWindow = nullptr;
		ImGuiContext& G = *ImGui::GetCurrentContext();
		for (int Idx = G.Windows.Size - 1; Idx >= 0; Idx)
		{
			CurrentWindow = G.Windows[Idx];
			if (CurrentWindow == Window1)
			{
				return Window1;
			}
			if (CurrentWindow == Window2)
			{
				return Window2;
			}
		}

		return nullptr;
    }

    /**
     * @brief Check if a window is docked, defaults to use the window name.
     */
	template<EFindType FindType = EFindType::Name, typename T = const char*>
	FORCEINLINE static ImVec2 GetWindowSize(T Identifier)
	{
		LK_CORE_ASSERT(false);
		return ImVec2();
	}

	template<>
	FORCEINLINE static ImVec2 GetWindowSize<EFindType::ID>(const ImGuiID ID)
	{
		if (ImGuiWindow* Window = ImGui::FindWindowByID(ID); Window != nullptr)
		{
			return Window->Size;
		}

		return ImVec2();
	}

	template<>
	FORCEINLINE static ImVec2 GetWindowSize<EFindType::Name>(const char* WindowName)
	{
		if (ImGuiWindow* Window = ImGui::FindWindowByName(WindowName); Window != nullptr)
		{
			return Window->Size;
		}

		return ImVec2();
	}

	FORCEINLINE static ImGuiID GetWindowID(const char* WindowName)
	{
		if (ImGuiWindow* Window = ImGui::FindWindowByName(WindowName); Window != nullptr)
		{
			return Window->ID;
		}

		return 0;
	}

	FORCEINLINE static const char* GetWindowName(const ImGuiID ID)
	{
		if (ImGuiWindow* Window = ImGui::FindWindowByID(ID); Window != nullptr)
		{
			return Window->Name;
		}

		return "NULL";
	}

	FORCEINLINE bool IsItemDisabled()
	{
		return ImGui::GetItemFlags() & ImGuiItemFlags_Disabled;
	}

	FORCEINLINE bool IsItemHovered(const float DelayInSeconds = 0.10f, ImGuiHoveredFlags Flags = ImGuiHoveredFlags_None)
	{
		return ImGui::IsItemHovered() && (GImGui->HoveredIdTimer > DelayInSeconds); /* HoveredIdNotActiveTimer. */
	}

	FORCEINLINE ImRect GetItemRect()
	{
		return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
	}

	FORCEINLINE ImRect RectExpanded(const ImRect& Rect, const float x, const float y)
	{
		ImRect Result = Rect;
		Result.Min.x -= x;
		Result.Min.y -= y;
		Result.Max.x += x;
		Result.Max.y += y;
		return Result;
	}

	FORCEINLINE ImRect RectOffset(const ImRect& Rect, const float x, const float y)
	{
		ImRect Result = Rect;
		Result.Min.x += x;
		Result.Min.y += y;
		Result.Max.x += x;
		Result.Max.y += y;
		return Result;
	}

	FORCEINLINE void ShiftCursorX(const float Distance)
	{
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + Distance);
	}

	FORCEINLINE void ShiftCursorY(const float Distance)
	{
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + Distance);
	}

	FORCEINLINE void ShiftCursor(const float InX, const float InY)
	{
		const ImVec2 Cursor = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(Cursor.x + InX, Cursor.y + InY));
	}

	FORCEINLINE void DrawItemActivityOutline()
	{
		/* TODO */
	}

	FORCEINLINE void HelpMarker(const char* HelpDesc, const char* HelpSymbol = "(?)")
	{
		static constexpr float WrapPosOffset = 35.0f;
		ImGui::TextDisabled(HelpSymbol);
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * WrapPosOffset);
			ImGui::TextUnformatted(HelpDesc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	FORCEINLINE void SetTooltip(std::string_view Text, 
								const float DelayInSeconds = 0.10f, 
								const bool AllowWhenDisabled = true, 
								const ImVec2 Padding = ImVec2(5, 5))
	{
		if (IsItemHovered(DelayInSeconds, AllowWhenDisabled ? ImGuiHoveredFlags_AllowWhenDisabled : ImGuiHoveredFlags_None))
		{
			UI::FScopedStyle WindowPadding(ImGuiStyleVar_WindowPadding, Padding);
			UI::FScopedColor TextColor(ImGuiCol_Text, RGBA32::Text::Brighter);
			ImGui::SetTooltip(Text.data());
		}
	}

	FORCEINLINE void PopupMenuHeader(const std::string& Text, 
									 const bool IndentAfter = true, 
									 const bool UnindentBefore = false)
	{
		if (UnindentBefore)
		{
			ImGui::Unindent();
		}

		static constexpr ImColor TextColor = ImColor(170, 170, 170);
		ImGui::TextColored(TextColor.Value, Text.c_str());
		ImGui::Separator();
		if (IndentAfter)
		{
			ImGui::Indent();
		}
	};

	FORCEINLINE void Separator(const ImVec2& Size, const ImVec4& Color)
	{
		ImGui::PushStyleColor(ImGuiCol_ChildBg, Color);
		ImGui::BeginChild("##Separator", Size);
		ImGui::EndChild();
		ImGui::PopStyleColor();
	}

	FORCEINLINE void VSeparator(const float Height = 10.0f)
	{
		ImGui::Dummy(ImVec2(0, Height));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0, Height));
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
		| ImGuiWindowFlags_NoDocking;

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

	namespace Draw 
	{
		FORCEINLINE void Underline(bool FullWidth = false, const float OffsetX = 0.0f, const float OffsetY = -1.0f)
		{
			if (FullWidth)
			{
				if (ImGui::GetCurrentWindow()->DC.CurrentColumns != nullptr)
				{
					ImGui::PushColumnsBackground();
				}
				else if (ImGui::GetCurrentTable() != nullptr)
				{
					ImGui::TablePushBackgroundChannel();
				}
			}

			const float Width = FullWidth ? ImGui::GetWindowWidth() : ImGui::GetContentRegionAvail().x;
			const ImVec2 Cursor = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddLine(
				ImVec2(Cursor.x + OffsetX, Cursor.y + OffsetY),
				ImVec2(Cursor.x + Width, Cursor.y + OffsetY),
				RGBA32::BackgroundDark,
				1.0f
			);

			if (FullWidth)
			{
				if (ImGui::GetCurrentWindow()->DC.CurrentColumns != nullptr)
				{
					ImGui::PopColumnsBackground();
				}
				else if (ImGui::GetCurrentTable() != nullptr)
				{
					ImGui::TablePopBackgroundChannel();
				}
			}
		}
	}

	FORCEINLINE void Spring(const float Weight = 1.0f)
	{
		const ImVec2 AvailableSpace = ImGui::GetContentRegionAvail();
		const float SpringSize = AvailableSpace.x * Weight;
		ImGui::Dummy(ImVec2(SpringSize, 0.0f));
	}

	FORCEINLINE void AlignHorizontalCenter(const float ItemWidth)
	{
		const ImVec2 AvailableSpace = ImGui::GetContentRegionAvail();
		const float Offset = (AvailableSpace.x - ItemWidth) / 2.0f;
		if (Offset > 0.0f)
		{
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + Offset);
		}
	}

	FORCEINLINE bool ColoredButton(const char* Label, const ImVec4& BgColor, const ImVec2& ButtonSize)
	{
		FScopedColor ButtonCol(ImGuiCol_Button, BgColor);
		return ImGui::Button(Label, ButtonSize);
	}

	FORCEINLINE bool ColoredButton(const char* Label, const ImVec4& BgColor, const ImVec4& FgColor, const ImVec2& ButtonSize)
	{
		FScopedColor TextColor(ImGuiCol_Text, FgColor);
		FScopedColor ButtonColor(ImGuiCol_Button, BgColor);
		return ImGui::Button(Label, ButtonSize);
	}

	FORCEINLINE void DrawButtonImage(const TObjectPtr<LTexture2D>& ImageNormal, 
									 const TObjectPtr<LTexture2D>& ImageHovered, 
									 const TObjectPtr<LTexture2D>& ImagePressed, 
									 const ImU32 TintNormal, 
									 const ImU32 TintHovered, 
									 const ImU32 TintPressed, 
									 const ImVec2& RectMin, 
									 const ImVec2& RectMax)
	{
		ImDrawList* DrawList = ImGui::GetWindowDrawList();
		if (ImGui::IsItemActive())
		{
			DrawList->AddImage(GetTextureID(ImagePressed), RectMin, RectMax, ImVec2(0, 0), ImVec2(1, 1), TintPressed);
		}
		else if (ImGui::IsItemHovered())
		{
			DrawList->AddImage(GetTextureID(ImageHovered), RectMin, RectMax, ImVec2(0, 0), ImVec2(1, 1), TintHovered);
		}
		else
		{
			DrawList->AddImage(GetTextureID(ImageNormal), RectMin, RectMax, ImVec2(0, 0), ImVec2(1, 1), TintNormal);
		}
	};

	FORCEINLINE void DrawButtonImage(const TObjectPtr<LTexture2D>& Image, 
									 const ImU32 TintNormal, 
									 const ImU32 TintHovered, 
									 const ImU32 TintPressed, 
									 const ImRect& Rectangle)
	{
		DrawButtonImage(Image, Image, Image, TintNormal, TintHovered, TintPressed, Rectangle.Min, Rectangle.Max);
	};

}


