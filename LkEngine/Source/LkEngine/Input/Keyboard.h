#pragma once

#include <utility>

#include "LkEngine/Input/Keycodes.h"


namespace LkEngine {

	class Keyboard
	{
	public:
		static void Init();

		static bool IsKeyPressed(KeyCode key);
	};
}
