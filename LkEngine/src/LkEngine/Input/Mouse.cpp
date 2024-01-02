#include "LKpch.h"
#include "LkEngine/Input/Mouse.h"
#include "LkEngine/Renderer/GraphicsContext.h"
#include "LkEngine/UI/DockSpace.h"
#include "LkEngine/Core/Window.h"
#include "LkEngine/Editor/EditorLayer.h"


namespace LkEngine {

	void Mouse::Init()
	{
		if (m_ActiveWindow == nullptr)
			m_ActiveWindow = Window::Get();
	}

	bool Mouse::IsButtonPressed(MouseCode button)
	{
		GLFWwindow* window = GraphicsContext::Get()->GetGlfwWindow();
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	glm::vec2 Mouse::GetPos()
	{
		double xpos, ypos;
		glfwGetCursorPos(GraphicsContext::Get()->GetGlfwWindow(), &xpos, &ypos);
		float x = static_cast<float>(xpos);
		float y = static_cast<float>(ypos);
		Pos.x = x;
		Pos.y = y;

		Mouse::CenterPos.x = (x / m_ActiveWindow->GetWidth()) * 2.0f - 1.0f;
		Mouse::CenterPos.y = ((y / m_ActiveWindow->GetHeight()) * 2.0f - 1.0f) * -1.0f; 

		return Pos;
	}

	glm::vec2 Mouse::GetRawPos()
	{
		double xpos, ypos;
		glfwGetCursorPos(m_ActiveWindow->GetGlfwWindow(), &xpos, &ypos);
		float x = static_cast<float>(xpos);
		float y = static_cast<float>(ypos);
		return { x, y };
	}

	void Mouse::SetScaledPos(float x, float y)
	{
		ScaledPos = { x, y };
	}

	float Mouse::GetMouseX()
	{
		auto pos = GetPos();
		return pos.x;
	}

	float Mouse::GetMouseY()
	{
		auto pos = GetPos();
		return pos.y;
	}

	void Mouse::SetPos(float x, float y)
	{
		Pos.x = x;
		Pos.y = y;
	}

	// The main cursor position is not updated here as the Editor actually adjusts the
	// position when it is being run because of the editor docking windows
	glm::vec2 Mouse::GetCenterPos() 
	{ 
		CenterPos.x = (Pos.x / m_ActiveWindow->GetWidth()) * 2.0f - 1.0f;
		CenterPos.y = ((Pos.y / m_ActiveWindow->GetHeight()) * 2.0f - 1.0f) * 1.0f;
		ScaledCenterPos.x = (CenterPos.x * m_ActiveWindow->GetWidth()) / m_ActiveWindow->GetScalerX();
		ScaledCenterPos.y = (CenterPos.y * m_ActiveWindow->GetHeight()) / m_ActiveWindow->GetScalerY();
		return CenterPos;
	}

	glm::vec2 Mouse::GetScaledPos()
	{
		Mouse::ScaledPos.x = (Pos.x) / m_ActiveWindow->GetScalerX();
		Mouse::ScaledPos.y = (Pos.y) / m_ActiveWindow->GetScalerY();
		//Mouse::ScaledPos.x = GetMouseX() / m_ActiveWindow->GetScalerX();
		//Mouse::ScaledPos.y = GetMouseY() / m_ActiveWindow->GetScalerY();
		return ScaledPos;
	}

	glm::vec2 Mouse::GetScaledCenterPos()
	{
		GetCenterPos(); // GetCenterPos updates ScaledCenterPos
		return ScaledCenterPos;
	}

	glm::vec2 Mouse::GetMousePosNormalized()
	{
		glm::vec2 mouse_pos = GetPos();
		float window_width = DockSpace::CenterWindowSize.x;
		float window_height = DockSpace::CenterWindowSize.y;
		double norm_mouse_x = mouse_pos.x / window_width;
		double norm_mouse_y = 1.0 - (mouse_pos.y / window_height);
		return { norm_mouse_x, norm_mouse_y };
	}

	void Mouse::SetActiveWindow(Window* window)
	{
		m_ActiveWindow = window;
	}

}
