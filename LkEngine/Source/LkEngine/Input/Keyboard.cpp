#include "LKpch.h"

#include "LkEngine/Input/Keyboard.h"
#include "LkEngine/Renderer/RenderContext.h"

#include "LkEngine/Core/Application.h"


namespace LkEngine {

	///
	/// FIXME: UPDATE THIS ENTIRE IMPL
	///

	static bool bInitialized = false;

	void LKeyboard::Initialize()
	{
		if (bInitialized)
		{
			return;
		}

		bInitialized = true;
	}

	bool LKeyboard::IsKeyPressed(const EKey Key)
	{
		GLFWwindow* GlfwWindow = LApplication::Get()->GetWindow().GetGlfwWindow();
		const int KeyState = glfwGetKey(GlfwWindow, static_cast<int32_t>(Key));

		return (KeyState == GLFW_PRESS);
	}

}
