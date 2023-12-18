#include "LKpch.h"
#include "LkEngine/Input/Mouse.h"
#include "LkEngine/Renderer/GraphicsContext.h"
#include "LkEngine/UI/DockSpace.h"


namespace LkEngine {

	bool Mouse::IsButtonPressed(MouseCode button)
	{
		GLFWwindow* window = GraphicsContext::Get()->GetGlfwWindow();
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	glm::vec2 Mouse::GetMousePos()
	{
		double xpos, ypos;
		glfwGetCursorPos(GraphicsContext::Get()->GetGlfwWindow(), &xpos, &ypos);
		float x = static_cast<float>(xpos);
		float y = static_cast<float>(ypos);
		return { x, y };
	}

	std::pair<float, float> Mouse::GetMousePosition()
	{
		double xpos, ypos;
		glfwGetCursorPos(GraphicsContext::Get()->GetGlfwWindow(), &xpos, &ypos);
		float x = static_cast<float>(xpos);
		float y = static_cast<float>(ypos);
		return std::make_pair(x, y);
	}

	float Mouse::GetMouseX()
	{
		auto pos = GetMousePos();
		return pos.x;
	}

	float Mouse::GetMouseY()
	{
		auto pos = GetMousePos();
		return pos.y;
	}

	glm::vec2 Mouse::GetMousePosNormalized()
	{
		glm::vec2 mouse_pos = GetMousePos();
		float window_width = DockSpace::CenterWindowSize.x;
		float window_height = DockSpace::CenterWindowSize.y;
		double norm_mouse_x = mouse_pos.x / window_width;
		double norm_mouse_y = 1.0 - (mouse_pos.y / window_height);
		return { norm_mouse_x, norm_mouse_y };
	}

	bool Mouse::IsMouseButtonProcessed(const MouseCode& button)
	{
		switch (button)
		{
			case MouseButton::ButtonLeft: return LeftMouseButtonProcessed;
			case MouseButton::ButtonRight: return RightMouseButtonProcessed;
			//default: throw std::runtime_error("Button press registration not implemented");
			default: break;
		}
		throw std::runtime_error("Button press registration not implemented");
	}

}
