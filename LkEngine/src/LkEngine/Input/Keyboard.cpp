#include "LkEngine/Input/Keyboard.h"
#include "LkEngine/Renderer/GraphicsContext.h"

namespace LkEngine {

	/* Check to see if a key is pressed */
	bool Keyboard::IsKeyPressed(KeyCode key)
	{
		// GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetGLFWwindow());
		GLFWwindow* window = *GraphicsContext::Get()->GlfwWindow;
		int keyState = glfwGetKey(window, static_cast<int32_t>(key));
		return keyState == GLFW_PRESS;
	}

}
