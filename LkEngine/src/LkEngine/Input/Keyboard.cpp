#include "LKpch.h"
#include "LkEngine/Input/Keyboard.h"
#include "LkEngine/Renderer/GraphicsContext.h"

namespace LkEngine {

	bool Keyboard::IsKeyPressed(KeyCode key)
	{
		GLFWwindow* window = GraphicsContext::Get()->GetGlfwWindow();
		int keyState = glfwGetKey(window, static_cast<int32_t>(key));
		return keyState == GLFW_PRESS;
	}

}
