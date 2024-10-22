#pragma once

#include "LkEngine/Core/Core.h"
#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "LkEngine/Input/Mouse.h"
#include "LkEngine/Input/MouseCodes.h"
#include "LkEngine/Input/Keycodes.h"

#include "LkEngine/Core/Event/KeyEvent.h"
#include "LkEngine/Core/Event/MouseEvent.h"


namespace LkEngine {

    class LScene;

    class LInput
    {
    public:
		static void Initialize();

		static void Update();
		static void SetScene(const TObjectPtr<LScene>& InScene); 

		FORCEINLINE static float GetMouseX() { return LMouse::GetMouseX(); }
		FORCEINLINE static float GetMouseY() { return LMouse::GetMouseY(); }

		static bool IsKeyPressed(EKey Key);
		static bool IsKeyHeld(EKey Key);
		static bool IsKeyDown(EKey Key);
		static bool IsKeyReleased(const EKey Key);

		static bool IsMouseButtonPressed(const EMouseButton Button);
		static bool IsMouseButtonHeld(const EMouseButton Button);
		static bool IsMouseButtonDown(const EMouseButton Button);
		static bool IsMouseButtonReleased(const EMouseButton Button);
		static bool IsAnyMouseButtonPressed();

		static std::pair<float, float> GetMousePosition();

		static void SetCursorMode(ECursorMode mode);
		static ECursorMode GetCursorMode();

		static FKeyData& UpdateKeyState(const EKey Key, EKeyState NewState);
		static FMouseButtonData& UpdateButtonState(const EMouseButton Button, const EMouseButtonState NewState);

		static void TransitionPressedKeys();
		static void TransitionPressedButtons();

		FORCEINLINE static FMouseButtonData& GetMouseData(const EMouseButton MouseButton)
		{
			LK_CORE_ASSERT(MouseDataMap.contains(MouseButton));
			return (MouseDataMap.at(MouseButton));
		}

		FORCEINLINE static EMouseButton GetLastMouseButton() 
		{ 
			return LastMouseButton; 
		}

    private:
        inline static TObjectPtr<LScene> Scene = nullptr;

		inline static EMouseButton LastMouseButton = EMouseButton::None;

		inline static std::map<EKey, FKeyData> KeyDataMap{};
		inline static std::map<EMouseButton, FMouseButtonData> MouseDataMap{};
    };

}