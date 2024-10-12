#pragma once

#include "LkEngine/Core/Core.h"
#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/LObjectPtr.h"

#include "LkEngine/Input/Mouse.h"
#include "LkEngine/Input/Keycodes.h"

#include "LkEngine/Core/Event/KeyEvent.h"
#include "LkEngine/Core/Event/MouseEvent.h"


namespace LkEngine {

    class LScene;

	using ButtonState = KeyState;

	struct FKeyData
	{
		KeyCode Key{};
		KeyState State = KeyState::None;
		KeyState OldState = KeyState::None;
	};

	struct FButtonData
	{
		EMouseButton Button = EMouseButton::None;
		KeyState State = KeyState::None;
		KeyState OldState = KeyState::None;
	};

    class Input
    {
    public:
		static void Init();

		static void Update();
		static void SetScene(const TObjectPtr<LScene>& InScene); 

		FORCEINLINE static float GetMouseX() 
		{ 
			return Mouse::GetMouseX(); 
		}

		FORCEINLINE static float GetMouseY() 
		{ 
			return Mouse::GetMouseY(); 
		}

		static bool IsKeyPressed(Key key);
		static bool IsKeyHeld(Key key);
		static bool IsKeyDown(Key key);
		static bool IsKeyReleased(const KeyCode key);

		static bool IsMouseButtonPressed(const EMouseButton Button);
		static bool IsMouseButtonHeld(const EMouseButton Button);
		static bool IsMouseButtonDown(const EMouseButton Button);
		static bool IsMouseButtonReleased(const EMouseButton Button);
		static bool IsAnyMouseButtonPressed();

		static std::pair<float, float> GetMousePosition();

		static void SetCursorMode(ECursorMode mode);
		static ECursorMode GetCursorMode();

		static void UpdateKeyState(KeyCode key, KeyState NewState);
		static void UpdateButtonState(const EMouseButton Button, KeyState NewState);

		static void TransitionPressedKeys();
		static void TransitionPressedButtons();

		FORCEINLINE static EMouseButton GetLastMouseButton() 
		{ 
			return LastMouseButton; 
		}

    private:
        inline static TObjectPtr<LScene> Scene = nullptr;

		inline static EMouseButton LastMouseButton = EMouseButton::None;

		inline static std::map<KeyCode, FKeyData> m_KeyData;
		inline static std::map<EMouseButton, FButtonData> m_MouseData;
    };

}