#include "LkEngine/Input/Mouse.h"
#include "LkEngine/Renderer/GraphicsContext.h"

namespace LkEngine {

	// FIXME
	bool Mouse::IsButtonPressed(MouseCode button)
	{
		GLFWwindow* window = *GraphicsContext::Get()->GlfwWindow;
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	std::pair<float, float> Mouse::GetMousePosition()
	{
		double xpos, ypos;
		glfwGetCursorPos(*GraphicsContext::Get()->GlfwWindow, &xpos, &ypos);
		float x = static_cast<float>(xpos);
		float y = static_cast<float>(ypos);
		return std::make_pair(x, y);
	}

	float Mouse::GetMouseX()
	{
		auto [x, y] = GetMousePosition();
		return x;
	}

	float Mouse::GetMouseY()
	{
		auto [x, y] = GetMousePosition();
		return y;
	}

}
