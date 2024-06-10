#include "LKpch.h"

#include "LkEngine/Input/Keyboard.h"
#include "LkEngine/Renderer/RenderContext.h"

#include "LkEngine/Core/Application.h"


namespace LkEngine {

	static bool Initialized = false;

	void Keyboard::Init()
	{
		if (Initialized)
		{
			return;
		}

		Initialized = true;
	}

	bool Keyboard::IsKeyPressed(const KeyCode key)
	{
		GLFWwindow* GlfwWindow = LApplication::Get()->GetWindow().GetGlfwWindow();
		const int KeyState = glfwGetKey(GlfwWindow, static_cast<int32_t>(key));
		return KeyState == GLFW_PRESS;
	}

}
