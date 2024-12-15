#include "LKpch.h"

#include "LkEngine/Input/Keyboard.h"
#include "LkEngine/Renderer/RenderContext.h"

#include "LkEngine/Core/Application.h"


namespace LkEngine {

	LKeyboard::FOnKeyPressed LKeyboard::OnKeyPressed;
	LKeyboard::FOnKeyReleased LKeyboard::OnKeyReleased;
	LKeyboard::FOnKeyHeld LKeyboard::OnKeyHeld;

	LKeyboard::LKeyboard()
	{
		LOBJECT_REGISTER();
	}

	void LKeyboard::Initialize()
	{
		LObject::Initialize();
	}

	bool LKeyboard::IsKeyPressed(const EKey Key)
	{
		GLFWwindow* GlfwWindow = LApplication::Get()->GetWindow().GetGlfwWindow();
		const int KeyState = glfwGetKey(GlfwWindow, static_cast<int32_t>(Key));

		return (KeyState == GLFW_PRESS);
	}

	LKeyboard& LKeyboard::Get()
	{
		static LKeyboard Keyboard;
		return Keyboard;
	}

}
