#include "LKpch.h"
#include "Input.h"

#include "LkEngine/Core/Application.h"
#include "LkEngine/Scene/Scene.h"


#include "LkEngine/Editor/EditorLayer.h"

namespace LkEngine {

	void LInput::Initialize()
	{
		LKeyboard& Keyboard = LKeyboard::Get();
        Keyboard.Initialize();

		MouseDataMap.insert({ EMouseButton::Button0, FMouseButtonData() });
		MouseDataMap.insert({ EMouseButton::Button1, FMouseButtonData() });
		MouseDataMap.insert({ EMouseButton::Button2, FMouseButtonData() });
		MouseDataMap.insert({ EMouseButton::Button3, FMouseButtonData() });
		MouseDataMap.insert({ EMouseButton::Button4, FMouseButtonData() });
		MouseDataMap.insert({ EMouseButton::Button5, FMouseButtonData() });
	}

	void LInput::Update()
	{
		/* HeldData holds the initial recorded timestamp when the held event 
		 * was triggered and the second entry is updated continously. */
		for (auto& [Key, HeldData] : KeyHeldMap)
		{
			KeyDataMap[Key].RepeatCount++;
			HeldData.second = std::chrono::steady_clock::now();
		}
	}

	bool LInput::IsMouseButtonPressed(const EMouseButton Button)
	{
		return ((MouseDataMap.find(Button) != MouseDataMap.end()) && (MouseDataMap[Button].State == EMouseButtonState::Pressed));
	}

	bool LInput::IsMouseButtonHeld(const EMouseButton Button)
	{
		return ((MouseDataMap.find(Button) != MouseDataMap.end()) && (MouseDataMap[Button].State == EMouseButtonState::Held));
	}

	bool LInput::IsMouseButtonDown(const EMouseButton Button)
	{
		const int MouseButtonState = glfwGetMouseButton(
			LApplication::Get().Get().GetWindow().GetGlfwWindow(), 
			static_cast<int32_t>(Button)
		);
		return (MouseButtonState == GLFW_PRESS);
	}

	bool LInput::IsMouseButtonReleased(const EMouseButton Button)
	{
		return (MouseDataMap.find(Button) != MouseDataMap.end() && MouseDataMap[Button].State == EMouseButtonState::Released);
	}

	bool LInput::IsAnyMouseButtonPressed()
	{
		auto IsMouseButtonPressed = [&](const std::pair<EMouseButton, FMouseButtonData>& InButtonData)
		{
			return (InButtonData.second.State == EMouseButtonState::Pressed);
		};
		auto Iter = std::find_if(MouseDataMap.begin(), MouseDataMap.end(), IsMouseButtonPressed);

		return Iter != MouseDataMap.end();
	}

	bool LInput::IsKeyPressed(const EKey Key)
	{
		return ((KeyDataMap.find(Key) != KeyDataMap.end()) && (KeyDataMap[Key].State == EKeyState::Pressed));
	}

	bool LInput::IsKeyHeld(const EKey Key)
	{
		return ((KeyDataMap.find(Key) != KeyDataMap.end()) && (KeyDataMap[Key].State == EKeyState::Held));
	}

	bool LInput::IsKeyDown(const EKey Key)
	{
		LWindow& Window = LApplication::Get().GetWindow();
		const int KeyState = glfwGetKey(Window.GetGlfwWindow(), static_cast<int32_t>(Key));
		return ((KeyState == GLFW_PRESS) || (KeyState == GLFW_REPEAT));
	}

	bool LInput::IsKeyReleased(const EKey Key)
	{
		return ((KeyDataMap.find(Key) != KeyDataMap.end()) && (KeyDataMap[Key].State == EKeyState::Released));
	}

	void LInput::SetCursorMode(const ECursorMode CursorMode)
	{
		LK_CORE_TRACE_TAG("Input", "SetCursorMode: {}", Enum::ToString(CursorMode));
		LWindow& Window = LApplication::Get().GetWindow();
		glfwSetInputMode(
			LApplication::Get().GetWindow().GetGlfwWindow(),
			GLFW_CURSOR, 
			GLFW_CURSOR_NORMAL + static_cast<int>(CursorMode)
		);

		UI::SetInputEnabled(CursorMode == ECursorMode::Normal);
	}

	ECursorMode LInput::GetCursorMode()
	{
		const int InputMode = glfwGetInputMode(LApplication::Get().GetWindow().GetGlfwWindow(), GLFW_CURSOR) - GLFW_CURSOR_NORMAL;
		return static_cast<ECursorMode>(InputMode);
	}

	FKeyData& LInput::GetKeyData(const EKey Key)
	{
		LK_CORE_ASSERT(KeyDataMap.contains(Key), "Key '{}' is not in the KeyDataMap", Enum::ToString(Key));
		return KeyDataMap[Key];
	}

	FKeyData& LInput::UpdateKeyState(const EKey Key, const EKeyState NewState)
	{
		FKeyData& KeyData = KeyDataMap[Key];
		KeyData.Key = Key;
		KeyData.OldState = KeyData.State;
		KeyData.State = NewState;
		//LK_CORE_DEBUG_TAG("Input", "Update key state: {}, {} -> {}", Enum::ToString(Key), Enum::ToString(KeyData.OldState), Enum::ToString(KeyData.State));

		if (NewState == EKeyState::Pressed)
		{
			KeyData.RepeatCount = 0;
		}
		else if (NewState == EKeyState::Released)
		{
			/* Remove any held key data whenever key is released. */
			std::erase_if(KeyHeldMap, [Key](const auto& CurrentKey) { return (Key == CurrentKey.first); });
		}
		/* Insert timestamp once on the initial held event. */
		else if ((NewState == EKeyState::Held) && (KeyData.RepeatCount <= 1))
		{
			using namespace std::chrono;
			KeyHeldMap.insert({ Key, { steady_clock::now(), steady_clock::now() } });
		}

		return KeyData;
	}

	FMouseButtonData& LInput::UpdateButtonState(const EMouseButton Button, const EMouseButtonState NewState)
	{
		FMouseButtonData& ButtonData = MouseDataMap[Button];
		ButtonData.Button = Button;
		ButtonData.OldState = ButtonData.State;
		ButtonData.State = NewState;
		LastMouseButton = Button;

		return ButtonData;
	}

	void LInput::TransitionPressedKeys()
	{
		for (const auto& [Key, KeyData] : KeyDataMap)
		{
			if (KeyData.State == EKeyState::Pressed)
			{
				UpdateKeyState(Key, EKeyState::Held);
			}
		}
	}

	void LInput::TransitionPressedButtons()
	{
		for (const auto& [Button, ButtonData] : MouseDataMap)
		{
			if (ButtonData.State == EMouseButtonState::Pressed)
			{
				UpdateButtonState(Button, EMouseButtonState::Held);
			}
		}
	}

	void LInput::ClearReleased()
	{
		/* Keyboard. */
		for (const auto& [Key, KeyData] : KeyDataMap)
		{
			if (KeyData.State == EKeyState::Released)
			{
				UpdateKeyState(Key, EKeyState::None);
			}
		}

		/* Mouse. */
		for (const auto& [Button, ButtonData] : MouseDataMap)
		{
			if (ButtonData.State == EMouseButtonState::Released)
			{
				UpdateButtonState(Button, EMouseButtonState::None);
			}
		}
	}

	std::size_t LInput::GetPressedKeys(std::vector<EKey>& InKeys)
	{
		InKeys.clear();
		InKeys.reserve(KeyDataMap.size());

		for (const auto& [Key, KeyData] : KeyDataMap)
		{
			if ((KeyData.State == EKeyState::Pressed) || (KeyData.State == EKeyState::Held))
			{
				InKeys.emplace_back(Key);
			}
		}

		InKeys.shrink_to_fit();
		return InKeys.size();
	}

}
