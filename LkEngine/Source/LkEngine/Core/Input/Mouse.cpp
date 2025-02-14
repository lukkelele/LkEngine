#include "LKpch.h"
#include "Mouse.h"

#include "LkEngine/Core/Application.h"
#include "LkEngine/Core/Window.h"

#include "LkEngine/Renderer/RenderContext.h"

#include "LkEngine/Editor/EditorLayer.h"


namespace LkEngine {

	float LMouse::GetMouseX()
	{
		double PosX, PosY;
		glfwGetCursorPos(LWindow::Get().GetGlfwWindow(), &PosX, &PosY);

		return static_cast<float>(PosX);
	}

	float LMouse::GetMouseY()
	{
		double PosX, PosY;
		glfwGetCursorPos(LWindow::Get().GetGlfwWindow(), &PosX, &PosY);

		return static_cast<float>(PosY);
	}

	bool LMouse::IsButtonPressed(const EMouseButton Button)
	{
		const bool bImGuiEnabled = LApplication::Get().GetSpecification().ImGuiEnabled;
		if (!bImGuiEnabled)
		{
			const int MouseButtonState = glfwGetMouseButton(
				LApplication::Get().GetWindow().GetGlfwWindow(), 
				static_cast<int32_t>(Button)
			);

			return MouseButtonState == GLFW_PRESS;
		}

		bool bPressed = false;
		for (ImGuiViewport* Viewport : ImGui::GetCurrentContext()->Viewports)
		{
			if (!Viewport->PlatformUserData)
			{
				continue;
			}

			GLFWwindow* WindowHandle = *(GLFWwindow**)Viewport->PlatformUserData; 
			if (!WindowHandle)
			{
				continue;
			}

			const int MouseButtonState = glfwGetMouseButton(WindowHandle, static_cast<int32_t>(Button));
			if ((MouseButtonState == GLFW_PRESS) || (MouseButtonState == GLFW_REPEAT))
			{
				bPressed = true;
				break;
			}
		}

		return bPressed;
	}

	void LMouse::Enable()
	{
		//if (!UI::IsMouseEnabled())
		if (!UI::IsInputEnabled())
		{
			//LK_CORE_DEBUG_TAG("Mouse", "Enabling");
			LInput::SetCursorMode(ECursorMode::Normal);
			OnCursorModeChanged.Broadcast(ECursorMode::Normal);

			/* The cursor mode call will enable the input. */
			//UI::SetInputEnabled(true);
			//UI::SetMouseEnabled(true);
			//OnMouseEnabled.Broadcast(true);
		}
	}

	void LMouse::Disable()
	{
		//if (UI::IsMouseEnabled())
		if (UI::IsInputEnabled())
		{
			//LK_CORE_DEBUG_TAG("Mouse", "Disabling");
			LInput::SetCursorMode(ECursorMode::Locked);
			OnCursorModeChanged.Broadcast(ECursorMode::Locked);

			/* The cursor mode call will disable the input. */
			//UI::SetInputEnabled(false);
			//UI::SetMouseEnabled(false);
			//OnMouseEnabled.Broadcast(false);
		}
	}

}
