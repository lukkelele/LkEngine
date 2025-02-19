#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"
#include "LkEngine/Asset/Asset.h"

#include <imgui/imgui.h>


namespace LkEngine {

	static_assert(std::is_same_v<uint32_t, ImGuiID>);

	class LEditorContext
	{
	public:
		LEditorContext() = default;
		LEditorContext(const LEditorContext& Other) = delete;
		LEditorContext(LEditorContext&& Other) = delete;
		LEditorContext operator=(const LEditorContext& Other) = delete;
		LEditorContext operator=(LEditorContext&& Other) = delete;

		static void Cache();

	public:
		/* Focus data. */
		bool bEditorViewportFocused = false;
		bool bLastFrameEditorViewportFocused = false;
		uint32_t FocusedWindowID = 0;
		const char* FocusedWindowName = "";
		uint32_t LastFrameFocusedWindowID = 0;
		const char* LastFrameFocusedWindowName = "";

		/* Hover data. */
		bool bEditorViewportHovered = false;
		bool bLastFrameEditorViewportHovered = false;
		uint32_t HoveredWindowID = 0;
		const char* HoveredWindowName = "";
		uint32_t LastFrameHoveredWindowID = 0;
		const char* LastFrameHoveredWindowName = "";

		/* Keyboard and mouse input. */
		struct FInputData
		{
			bool bKeyboardEnabled = false;
			bool bMouseEnabled = false;

			uint32_t LastToggleFocusedWindowID = 0;
			const char* LastToggleFocusedWindowName = "";
		};
		FInputData Input{};
	};

	static LEditorContext EditorContext;

}
