#pragma once

#include "LkEngine/Core/CoreMacros.h"
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
	};

	///
	/// FIXME: UPDATE THIS ENTIRE IMPL
	///
	class LKeyboard
	{
	public:
		LK_DECLARE_MULTICAST_DELEGATE(FOnKeyPressed, const FKeyData&);
		LK_DECLARE_MULTICAST_DELEGATE(FOnKeyReleased, const FKeyData&);

	public:
		static void Initialize();

		static bool IsKeyPressed(const EKey Key);
	};
}
