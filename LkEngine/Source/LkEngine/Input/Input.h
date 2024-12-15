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

		static bool IsKeyPressed(EKey Key);
		static bool IsKeyHeld(EKey Key);
		static bool IsKeyDown(EKey Key);
		static bool IsKeyReleased(const EKey Key);

		static bool IsMouseButtonPressed(const EMouseButton Button);
		static bool IsMouseButtonHeld(const EMouseButton Button);
		static bool IsMouseButtonDown(const EMouseButton Button);
		static bool IsMouseButtonReleased(const EMouseButton Button);
		static bool IsAnyMouseButtonPressed();

		static void SetCursorMode(ECursorMode mode);
		static ECursorMode GetCursorMode();

		static FKeyData& GetKeyData(const EKey Key);

		static FKeyData& UpdateKeyState(const EKey Key, EKeyState NewState);

		/**
		 * @brief Update the state of a mouse button.
		 * Return the button data container with the updated state.
		 */
		static FMouseButtonData& UpdateButtonState(const EMouseButton Button, const EMouseButtonState NewState);

		static void TransitionPressedKeys();
		static void TransitionPressedButtons();

		/**
		 * @brief Clear released inputs.
		 */
		static void ClearReleased();

		template<typename TDuration>
		static TDuration GetKeyHeldTime(const EKey Key)
		{
			using namespace std::chrono;
			LK_CORE_ASSERT(KeyHeldMap.contains(Key), "Key '{}' is not held down", Enum::ToString(Key));
			return duration_cast<TDuration>(KeyHeldMap.at(Key).second - KeyHeldMap.at(Key).first);
		}

		FORCEINLINE static FMouseButtonData& GetMouseData(const EMouseButton MouseButton)
		{
			LK_CORE_ASSERT(MouseDataMap.contains(MouseButton));
			return (MouseDataMap.at(MouseButton));
		}

		FORCEINLINE static EMouseButton GetLastMouseButton() { return LastMouseButton; }

		static std::size_t GetPressedKeys(std::vector<EKey>& InKeys);

		FORCEINLINE static float GetMouseX() { return LMouse::GetMouseX(); }
		FORCEINLINE static float GetMouseY() { return LMouse::GetMouseY(); }

    private:
		inline static EMouseButton LastMouseButton = EMouseButton::None;

		inline static std::map<EKey, FKeyData> KeyDataMap{};
		inline static std::map<EMouseButton, FMouseButtonData> MouseDataMap{};

		using FKeyHeldData = std::pair<std::chrono::steady_clock::time_point, std::chrono::steady_clock::time_point>;
		//inline static std::map<EKey, std::chrono::steady_clock::time_point> KeyHeldMap{};
		inline static std::map<EKey, FKeyHeldData> KeyHeldMap{};
    };

}
