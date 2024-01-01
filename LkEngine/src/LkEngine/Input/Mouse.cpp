#include "LKpch.h"
#include "LkEngine/Input/Mouse.h"
#include "LkEngine/Renderer/GraphicsContext.h"
#include "LkEngine/UI/DockSpace.h"
#include "LkEngine/Core/Window.h"


namespace LkEngine {

	void Mouse::Init()
	{
		m_WindowWidth = Window::Get()->GetWidth();
		m_WindowHeight = Window::Get()->GetHeight();
	}

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

		auto* window = Window::Get();

		//(Mouse::Pos.x / EditorWindowSize.x) * 2.0f - 1.0f, 
		//((Mouse::Pos.y / EditorWindowSize.y) * 2.0f - 1.0f) * 1.0f /* was -1.0f */
		LK_ASSERT(m_WindowWidth != 0 && m_WindowHeight != 0);
		//Mouse::CenterPos.x = (Mouse::Pos.x / m_WindowWidth) * 2.0f - 1.0f;
		//Mouse::CenterPos.y = ((Mouse::Pos.y / m_WindowHeight) * 2.0f - 1.0f) * 1.0f; // was -1.0f 

		Mouse::CenterPos.x = (Mouse::Pos.x / window->GetWidth()) * 2.0f - 1.0f;
		Mouse::CenterPos.y = ((Mouse::Pos.y / window->GetWidth()) * 2.0f - 1.0f) * 1.0f; // was -1.0f 

		Mouse::ScaledPos = { 
			(Mouse::Pos.x) / window->GetScalerX(),
			(Mouse::Pos.y) / window->GetScalerY()
		};

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

#if 0
	void Mouse::SetScalerX(float x)
	{
		Scalers.x = x;
	}

	void Mouse::SetScalerY(float y)
	{
		Scalers.y = y;
	}

	void Mouse::SetScalers(float x, float y)
	{
		Scalers = { x, y };
	}
#endif

	void Mouse::SetCenterPos(float x, float y)
	{
		CenterPos.x = x; 
		CenterPos.y = y;

		//ScaledCenterPos.x = (x / Scalers.x);
		//ScaledCenterPos.y = (y / Scalers.y);
		//ScaledCenterPos.x = (x * m_WindowWidth) / Scalers.x;
		//ScaledCenterPos.y = (y * m_WindowHeight) / Scalers.y;
		auto* window = Window::Get();
		ScaledCenterPos.x = (x * m_WindowWidth) / window->GetScalerX();
		ScaledCenterPos.y = (y * m_WindowHeight) / window->GetScalerY();

		// TODO: Set the different variations of the centered pos here as well
		//ImGui::Text("Center (%.2f, %.2f)", Mouse::CenterPos.x * EditorWindowSize.x * 0.50f, Mouse::CenterPos.y * EditorWindowSize.y * 0.50f);
		//ImGui::Text("Center Normalized (%.2f, %.2f)", Mouse::CenterPos.x, Mouse::CenterPos.y);
		//ImGui::Text("Center Scaled (%.2f, %.2f)", (Mouse::CenterPos.x * EditorWindowSize.x * 0.50f) / Mouse::Scalers.x, Mouse::CenterPos.y * EditorWindowSize.y * 0.50f / Mouse::Scalers.y) ;
	}

	glm::vec2 Mouse::GetCenterPos() 
	{ 
		// TODO: Update the centered pos before each get-request

		return CenterPos;
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

	void Mouse::SetWindowWidth(float x)
	{
		m_WindowWidth = x;
	}

	void Mouse::SetWindowHeight(float y)
	{
		m_WindowHeight = y;
	}

	void Mouse::SetWindowResolution(float x, float y)
	{
		m_WindowWidth = x;
		m_WindowHeight = y;
	}

}
