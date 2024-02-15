#include "LKpch.h"
#include "Mouse.h"

#include "LkEngine/Core/Application.h"
#include "LkEngine/Core/Window.h"

#include "LkEngine/Renderer/RenderContext.h"

#include "LkEngine/Editor/EditorLayer.h"


namespace LkEngine {

	static bool Initialized = false;

	void Mouse::Init()
	{
		if (Initialized)
			return;

		Initialized = true;
	}

	bool Mouse::IsButtonPressed(MouseButton button)
	{
		GLFWwindow* window = Window::Get().GetGlfwWindow();
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	glm::vec2 Mouse::GetPos()
	{
		double xpos, ypos;
		//glfwGetCursorPos(RenderContext::Get()->GetGlfwWindow(), &xpos, &ypos);
		glfwGetCursorPos(Window::Get().GetGlfwWindow(), &xpos, &ypos);
		float x = static_cast<float>(xpos);
		float y = static_cast<float>(ypos);
		Pos.x = x;
		Pos.y = y;

		Mouse::CenterPos.x = (x / Window::Get().GetWidth()) * 2.0f - 1.0f;
		Mouse::CenterPos.y = ((y / Window::Get().GetHeight()) * 2.0f - 1.0f) * -1.0f;

		return Pos;
	}

	glm::vec2 Mouse::GetRawPos()
	{
		double xpos, ypos;
		glfwGetCursorPos(Window::Get().GetGlfwWindow(), &xpos, &ypos);
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
		CenterPos.x = (Pos.x / Window::Get().GetWidth()) * 2.0f - 1.0f;
		CenterPos.y = ((Pos.y / Window::Get().GetHeight()) * 2.0f - 1.0f) * 1.0f;
		ScaledCenterPos.x = (CenterPos.x * Window::Get().GetWidth()) / Window::Get().GetScalerX();
		ScaledCenterPos.y = (CenterPos.y * Window::Get().GetHeight()) / Window::Get().GetScalerY();
		return CenterPos;
	}

	glm::vec2 Mouse::GetScaledPos()
	{
		return ScaledPos;
	}

	glm::vec2 Mouse::GetScaledCenterPos()
	{
		return ScaledCenterPos;
	}

	//void Mouse::SetActiveWindow(Window* window)
	//{
	//	m_ActiveWindow = window;
	//}

	bool Mouse::IsButtonDown(MouseButton button)
	{
		bool imguiEnabled = Application::Get()->GetSpecification().ImGuiEnabled;
		if (imguiEnabled == false)
		{
			auto& window = Application::Get()->GetWindow();
			auto state = glfwGetMouseButton(static_cast<GLFWwindow*>(window.GetGlfwWindow()), static_cast<int32_t>(button));
			return state == GLFW_PRESS;
		}

		ImGuiContext* context = ImGui::GetCurrentContext();
		bool pressed = false;
		for (ImGuiViewport* viewport : context->Viewports)
		{
			if (!viewport->PlatformUserData)
				continue;

			// The first member is GLFWwindow handle
			GLFWwindow* windowHandle = *(GLFWwindow**)viewport->PlatformUserData; 
			if (!windowHandle)
				continue;

			auto state = glfwGetMouseButton(static_cast<GLFWwindow*>(windowHandle), static_cast<int32_t>(button));
			if (state == GLFW_PRESS || state == GLFW_REPEAT)
			{
				pressed = true;
				break;
			}
		}
		return pressed;
	}

	void Mouse::Enable()
	{
		Input::SetCursorMode(CursorMode::Normal);
		UI::SetInputEnabled(true);
	}

	void Mouse::Disable()
	{
		Input::SetCursorMode(CursorMode::Locked);
		UI::SetInputEnabled(false);
	}

}
