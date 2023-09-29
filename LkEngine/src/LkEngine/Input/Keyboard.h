#pragma once
#include "LkEngine/Input/Keycodes.h"
#include <utility>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace LkEngine {

	class Keyboard
	{
	public:
		static bool IsKeyPressed(KeyCode key);
	};
}
