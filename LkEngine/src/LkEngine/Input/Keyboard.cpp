#include "LKpch.h"

#include "LkEngine/Input/Keyboard.h"
#include "LkEngine/Renderer/RenderContext.h"

#include "LkEngine/Core/Application.h"


namespace LkEngine {

	static bool Initialized = false;

	void Keyboard::Init()
	{
		if (Initialized)
			return;

		Initialized = true;
	}

	bool Keyboard::IsKeyPressed(KeyCode key)
	{
		GLFWwindow* window = Application::Get()->GetWindow().GetGlfwWindow();
		int keyState = glfwGetKey(window, static_cast<int32_t>(key));
		return keyState == GLFW_PRESS;
	}

}
