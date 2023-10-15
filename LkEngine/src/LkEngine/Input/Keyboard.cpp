#include "LKpch.h"
#include "LkEngine/Input/Keyboard.h"
#include "LkEngine/Renderer/GraphicsContext.h"

namespace LkEngine {

	bool Keyboard::IsKeyPressed(KeyCode key)
	{
		GLFWwindow* window = *GraphicsContext::Get()->GetGlfwWindow();
		int keyState = glfwGetKey(window, static_cast<int32_t>(key));
		//LOG_DEBUG("IsKeyPressed({}) : {}", key, keyState == GLFW_PRESS);
		return keyState == GLFW_PRESS;
	}

}
