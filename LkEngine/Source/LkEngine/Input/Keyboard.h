#pragma once

#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "LkEngine/Core/Delegate/Delegate.h"

#include "LkEngine/Input/Keycodes.h"


namespace LkEngine {

	/**
	 * FKeyData
	 */
	struct FKeyData
	{
		EKey Key{};
		EKeyState State = EKeyState::None;
		EKeyState OldState = EKeyState::None;
		int RepeatCount = 0;
	};

	class LKeyboard : public LObject
	{
	public:
		LK_DECLARE_MULTICAST_DELEGATE(FOnKeyPressed, const FKeyData&);
		LK_DECLARE_MULTICAST_DELEGATE(FOnKeyReleased, const FKeyData&);
		LK_DECLARE_MULTICAST_DELEGATE(FOnKeyHeld, const FKeyData&);
	public:
		LKeyboard();

		void Initialize();

		static bool IsKeyPressed(const EKey Key);

		static LKeyboard& Get();

	public:
		static FOnKeyPressed OnKeyPressed;   /* TODO: Might remove this as static. */
		static FOnKeyReleased OnKeyReleased; /* TODO: Might remove this as static. */
		static FOnKeyHeld OnKeyHeld;         /* TODO: Might remove this as static. */
	private:
		LCLASS(LKeyboard);
	};
}
