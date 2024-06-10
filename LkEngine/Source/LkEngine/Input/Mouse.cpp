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
		GLFWwindow* GlfwWindow = LWindow::Get().GetGlfwWindow();
		int MouseButtonState = glfwGetMouseButton(GlfwWindow, static_cast<int32_t>(button));
		return MouseButtonState == GLFW_PRESS;
	}

	glm::vec2 Mouse::GetPos()
	{
		double xpos, ypos;
		//glfwGetCursorPos(RenderContext::Get()->GetGlfwWindow(), &xpos, &ypos);
		glfwGetCursorPos(LWindow::Get().GetGlfwWindow(), &xpos, &ypos);
		float x = static_cast<float>(xpos);
		float y = static_cast<float>(ypos);
		Pos.x = x;
		Pos.y = y;

		Mouse::CenterPos.x = (x / LWindow::Get().GetWidth()) * 2.0f - 1.0f;
		Mouse::CenterPos.y = ((y / LWindow::Get().GetHeight()) * 2.0f - 1.0f) * -1.0f;

		return Pos;
	}

	glm::vec2 Mouse::GetRawPos()
	{
		double xpos, ypos;
		glfwGetCursorPos(LWindow::Get().GetGlfwWindow(), &xpos, &ypos);
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
		CenterPos.x = (Pos.x / LWindow::Get().GetWidth()) * 2.0f - 1.0f;
		CenterPos.y = ((Pos.y / LWindow::Get().GetHeight()) * 2.0f - 1.0f) * 1.0f;
		ScaledCenterPos.x = (CenterPos.x * LWindow::Get().GetWidth()) / LWindow::Get().GetScalerX();
		ScaledCenterPos.y = (CenterPos.y * LWindow::Get().GetHeight()) / LWindow::Get().GetScalerY();
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
		const bool bImGuiEnabled = LApplication::Get()->GetSpecification().ImGuiEnabled;
		if (bImGuiEnabled == false)
		{
			LWindow& Window = LApplication::Get()->GetWindow();
			const int MouseButtonState = glfwGetMouseButton(static_cast<GLFWwindow*>(Window.GetGlfwWindow()), static_cast<int32_t>(button));
			return MouseButtonState == GLFW_PRESS;
		}

		ImGuiContext* context = ImGui::GetCurrentContext();
		bool pressed = false;
		for (ImGuiViewport* Viewport : context->Viewports)
		{
			if (!Viewport->PlatformUserData)
			{
				continue;
			}

			// The first member is GLFWwindow handle
			GLFWwindow* WindowHandle = *(GLFWwindow**)Viewport->PlatformUserData; 
			if (!WindowHandle)
			{
				continue;
			}

			const int State = glfwGetMouseButton(static_cast<GLFWwindow*>(WindowHandle), static_cast<int32_t>(button));
			if (State == GLFW_PRESS || State == GLFW_REPEAT)
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
