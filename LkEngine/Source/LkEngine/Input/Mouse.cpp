#include "LKpch.h"
#include "Mouse.h"

#include "LkEngine/Core/Application.h"
#include "LkEngine/Core/Window.h"

#include "LkEngine/Renderer/RenderContext.h"

#include "LkEngine/Editor/EditorLayer.h"


namespace LkEngine {


	void LMouse::Initialize()
	{
		static bool bInitialized = false;
		if (bInitialized)
		{
			LK_ASSERT(false);
			return;
		}

		bInitialized = true;
	}

	glm::vec2 LMouse::GetPos()
	{
		double xpos, ypos;
		glfwGetCursorPos(LWindow::Get().GetGlfwWindow(), &xpos, &ypos);

		float x = static_cast<float>(xpos);
		float y = static_cast<float>(ypos);
		Pos.x = x;
		Pos.y = y;

		LMouse::CenterPos.x = (x / LWindow::Get().GetWidth()) * 2.0f - 1.0f;
		LMouse::CenterPos.y = ((y / LWindow::Get().GetHeight()) * 2.0f - 1.0f) * -1.0f;

		return Pos;
	}

	LVector2 LMouse::GetPosition()
	{
		double xpos, ypos;
		glfwGetCursorPos(LWindow::Get().GetGlfwWindow(), &xpos, &ypos);

		float x = static_cast<float>(xpos);
		float y = static_cast<float>(ypos);
		Pos.x = x;
		Pos.y = y;

		LMouse::CenterPos.x = (x / LWindow::Get().GetWidth()) * 2.0f - 1.0f;
		LMouse::CenterPos.y = ((y / LWindow::Get().GetHeight()) * 2.0f - 1.0f) * -1.0f;

		return { x, y };
	}

	glm::vec2 LMouse::GetRawPos()
	{
		double xpos, ypos;
		glfwGetCursorPos(LWindow::Get().GetGlfwWindow(), &xpos, &ypos);

		return { static_cast<float>(xpos), static_cast<float>(ypos) };
	}

	void LMouse::SetScaledPos(float x, float y)
	{
		ScaledPos = { x, y };
	}

	float LMouse::GetMouseX()
	{
		auto pos = GetPos();
		return pos.x;
	}

	float LMouse::GetMouseY()
	{
		auto pos = GetPos();
		return pos.y;
	}

	void LMouse::SetPos(float x, float y)
	{
		Pos.x = x;
		Pos.y = y;
	}

	// The main cursor position is not updated here as the Editor actually adjusts the
	// position when it is being run because of the editor docking windows
	glm::vec2 LMouse::GetCenterPos() 
	{ 
		CenterPos.x = (Pos.x / LWindow::Get().GetWidth()) * 2.0f - 1.0f;
		CenterPos.y = ((Pos.y / LWindow::Get().GetHeight()) * 2.0f - 1.0f) * 1.0f;
		ScaledCenterPos.x = (CenterPos.x * LWindow::Get().GetWidth()) / LWindow::Get().GetScalerX();
		ScaledCenterPos.y = (CenterPos.y * LWindow::Get().GetHeight()) / LWindow::Get().GetScalerY();
		return CenterPos;
	}

	glm::vec2 LMouse::GetScaledPos()
	{
		return ScaledPos;
	}

	glm::vec2 LMouse::GetScaledCenterPos()
	{
		return ScaledCenterPos;
	}

	bool LMouse::IsButtonPressed(const EMouseButton Button)
	{
		const bool bImGuiEnabled = LApplication::Get()->GetSpecification().ImGuiEnabled;
		if (bImGuiEnabled == false)
		{
			LWindow& Window = LApplication::Get()->GetWindow();
			const int MouseButtonState = glfwGetMouseButton(static_cast<GLFWwindow*>(Window.GetGlfwWindow()), 
															static_cast<int32_t>(Button));

			return MouseButtonState == GLFW_PRESS;
		}

		bool bPressed = false;
		for (ImGuiViewport* Viewport : ImGui::GetCurrentContext()->Viewports)
		{
			if (!Viewport->PlatformUserData)
			{
				continue;
			}

			// The first member is the GLFWwindow handle.
			GLFWwindow* WindowHandle = *(GLFWwindow**)Viewport->PlatformUserData; 
			if (!WindowHandle)
			{
				continue;
			}

			const int State = glfwGetMouseButton(static_cast<GLFWwindow*>(WindowHandle), static_cast<int32_t>(Button));
			if ((State == GLFW_PRESS) || (State == GLFW_REPEAT))
			{
				bPressed = true;
				break;
			}
		}

		return bPressed;
	}

	void LMouse::Enable()
	{
		LInput::SetCursorMode(ECursorMode::Normal);
		UI::SetInputEnabled(true);
	}

	void LMouse::Disable()
	{
		LInput::SetCursorMode(ECursorMode::Locked);
		UI::SetInputEnabled(false);
	}

}
