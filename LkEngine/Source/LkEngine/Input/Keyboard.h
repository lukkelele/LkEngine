#pragma once

#include <utility>

#include "LkEngine/Input/Keycodes.h"


namespace LkEngine {

	///
	/// FIXME: UPDATE THIS ENTIRE IMPL
	///
	class LKeyboard
	{
	public:
		static void Initialize();

		static bool IsKeyPressed(const KeyCode key);
	};
}
