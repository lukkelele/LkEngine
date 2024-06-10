#pragma once

#include "LkEngine/Core/Base.h"

#include "LkEngine/Input/Mouse.h"
#include "LkEngine/Input/Keycodes.h"

#include "LkEngine/Core/Event/KeyEvent.h"
#include "LkEngine/Core/Event/MouseEvent.h"

#include "LkEngine/Scene/Scene.h"


namespace LkEngine {

    //class Scene;

	struct KeyData
	{
		KeyCode Key;
		KeyState State = KeyState::None;
		KeyState OldState = KeyState::None;
	};

	struct ButtonData
	{
		MouseButton Button;
		KeyState State = KeyState::None;
		KeyState OldState = KeyState::None;
	};

    class Input
    {
    public:
		static void Init();

		static void Update();
		static void SetScene(const Ref<Scene>& scene); 

		static float GetMouseX() { return Mouse::GetMouseX(); }
		static float GetMouseY() { return Mouse::GetMouseY(); }

		static bool IsKeyPressed(Key key);
		static bool IsKeyHeld(Key key);
		static bool IsKeyDown(Key key);
		static bool IsKeyReleased(Key key);

		static bool IsMouseButtonPressed(MouseButton button);
		static bool IsMouseButtonHeld(MouseButton button);
		static bool IsMouseButtonDown(MouseButton button);
		static bool IsMouseButtonReleased(MouseButton button);

		static std::pair<float, float> GetMousePosition();

		static void SetCursorMode(CursorMode mode);
		static CursorMode GetCursorMode();

		static void UpdateKeyState(KeyCode key, KeyState newState);
		static void UpdateButtonState(MouseButton button, KeyState newState);

		static void TransitionPressedKeys();
		static void TransitionPressedButtons();

    private:
        inline static Ref<Scene> m_Scene = nullptr;

		inline static std::map<KeyCode, KeyData> m_KeyData;
		inline static std::map<MouseButton, ButtonData> m_MouseData;
    };

}