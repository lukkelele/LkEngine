#include "LKpch.h"
#include "Input.h"

#include "LkEngine/Core/Application.h"
#include "LkEngine/Scene/Scene.h"


#include "LkEngine/Editor/EditorLayer.h"

namespace LkEngine {

    LApplication* Application = nullptr; /* REMOVE ME */

	void Input::Initialize()
	{
        LKeyboard::Initialize();
        Mouse::Initialize();
	}

	void Input::SetScene(const TObjectPtr<LScene>& InScene)
	{
		if (!InScene || Scene == InScene)
		{
			return;
		}

		Scene = InScene;
	}

	void Input::Update()
	{
		if (Scene == nullptr)
		{
			return;
		}

		LEditorLayer* Editor = LEditorLayer::Get();
		if (Mouse::IsButtonPressed(EMouseButton::Button0) == false)
		{
			if (LKeyboard::IsKeyPressed(Key::Escape))
			{
				LK_CORE_TRACE("De-selecting (clicked escape)");
				if (Editor && Editor->IsEnabled())
				{
					//LEntity entity = { (entt::entity)0, Scene.Get()};
					LEntity entity = { (entt::entity)0, Scene };
					LK_UNUSED(entity);

					//SELECTION::SelectedEntity = entity;
				}

				return;
			}
		}

        const glm::vec2 mousePos = Mouse::GetPos();
        std::vector<Raycast2DResult> raycastResults = Physics2D::RaycastFromScreen(*Scene);
		for (const Raycast2DResult& RaycastResult : raycastResults)
		{
			LK_CORE_DEBUG_TAG("RaycastResult2D", "{}", RaycastResult.ToString());
		}

		const int raycastHits = raycastResults.size();

		if (raycastHits == 1)
		{
			LK_CORE_DEBUG("Hitcast result == 1");
		    Raycast2DResult raycast = raycastResults.at(0);
		    LEntity entity = raycast.HitEntity;

		    if (Mouse::IsButtonPressed(EMouseButton::Button0))
		    {
				//SELECTION::SelectedEntity = raycast.HitEntity;
		    }
		}
		else if (raycastHits > 1)
		{
			LK_CORE_WARN("Hitcast results > 1");
		    for (auto& raycast : raycastResults)
		    {
				//SelectionContext::ClearEntitySelection();
		    }
		}
		else // NO HITS
		{
		}

        if (LKeyboard::IsKeyPressed(Key::Escape))
        {
			LK_CORE_DEBUG("De-selecting (clicked escape)");
			if (Editor && Editor->IsEnabled())
			{
				//SelectionContext::ClearEntitySelection();
			}
        }
	}

	bool Input::IsMouseButtonPressed(const EMouseButton Button)
	{
		return ((m_MouseData.find(Button) != m_MouseData.end()) && (m_MouseData[Button].State == KeyState::Pressed));
	}

	bool Input::IsMouseButtonHeld(const EMouseButton Button)
	{
		return ((m_MouseData.find(Button) != m_MouseData.end()) && (m_MouseData[Button].State == KeyState::Held));
	}

	bool Input::IsMouseButtonDown(const EMouseButton Button)
	{
		bool imguiEnabled = LApplication::Get()->GetSpecification().ImGuiEnabled;
		if (imguiEnabled == false)
		{
			LWindow & Window = LApplication::Get()->GetWindow();
			const int State = glfwGetMouseButton(static_cast<GLFWwindow*>(Window.GetGlfwWindow()), static_cast<int32_t>(Button));
			return (State == GLFW_PRESS);
		}
	
		LWindow& Window = LApplication::Get()->GetWindow();
		LK_CORE_ASSERT(&Window != nullptr, "Window is nullptr");
		const int State = glfwGetMouseButton(static_cast<GLFWwindow*>(Window.GetGlfwWindow()), static_cast<int32_t>(Button));

		return (State == GLFW_PRESS);

		//------------------------------------------------------------
		// ImGui Multi-Viewports
		// Needs io.Config |= ImGuiConfigFlags_ViewportsEnable
		//------------------------------------------------------------
#if LK_USE_MULTI_VIEWPORTS
		ImGuiContext* context = ImGui::GetCurrentContext();
		bool pressed = false;
		for (ImGuiViewport* viewport : context->Viewports)
		{
			if (viewport->PlatformUserData == nullptr)
			{
				//LK_CORE_ASSERT(false, "viewport->PlatformUserData is nullptr");
				continue;
			}
			LK_CORE_DEBUG("viewport->PlatformUserData != nullptr !!!");

			GLFWwindow* windowHandle = *(GLFWwindow**)viewport->PlatformUserData; // First member is GLFWwindow
			if (!windowHandle)
			{
				LK_CORE_ASSERT(false, "windowHandle (GLFWwindow*) is nullptr");
				continue;
			}

			auto state = glfwGetMouseButton(static_cast<GLFWwindow*>(windowHandle), static_cast<int32_t>(Button));
			if (state == GLFW_PRESS || state == GLFW_REPEAT)
			{
				pressed = true;
				break;
			}
		}
		return pressed;
#endif
	}

	bool Input::IsMouseButtonReleased(const EMouseButton Button)
	{
		return m_MouseData.find(Button) != m_MouseData.end() && m_MouseData[Button].State == KeyState::Released;
	}

	bool Input::IsAnyMouseButtonPressed()
	{
		auto IsMouseButtonPressed = [&](const std::pair<EMouseButton, FButtonData>& InButtonData)
		{
			return InButtonData.second.State == KeyState::Pressed;
		};
		auto Iter = std::find_if(m_MouseData.begin(), m_MouseData.end(), IsMouseButtonPressed);

		return Iter != m_MouseData.end();
	}

	bool Input::IsKeyPressed(Key key)
	{
		return ((m_KeyData.find(key) != m_KeyData.end()) && (m_KeyData[key].State == KeyState::Pressed));
	}

	bool Input::IsKeyHeld(Key key)
	{
		return ((m_KeyData.find(key) != m_KeyData.end()) && (m_KeyData[key].State == KeyState::Held));
	}

	bool Input::IsKeyDown(Key keycode)
	{
		const bool bEnableImGui = LApplication::Get()->GetSpecification().ImGuiEnabled;
		if (!bEnableImGui)
		{
			auto& window = LApplication::Get()->GetWindow();
			auto state = glfwGetKey(static_cast<GLFWwindow*>(window.GetGlfwWindow()), static_cast<int32_t>(keycode));
			return state == GLFW_PRESS || state == GLFW_REPEAT;
		}
		
		LWindow& WindowRef = LApplication::Get()->GetWindow();
		GLFWwindow* win = static_cast<GLFWwindow*>(WindowRef.GetGlfwWindow());
		bool pressed = false;
		auto state = glfwGetKey(win, static_cast<int32_t>(keycode));
		if (state == GLFW_PRESS || state == GLFW_REPEAT)
		{
			pressed = true;
		}

		//------------------------------------------------------------
		// ImGui Multi-Viewports
		// Needs io.Config |= ImGuiConfigFlags_ViewportsEnable
		//------------------------------------------------------------
#if LK_USE_MULTI_VIEWPORTS
		ImGuiContext* context = ImGui::GetCurrentContext();
		for (ImGuiViewport* viewport : context->Viewports)
		{
			if (!viewport->PlatformUserData)
				continue;

			GLFWwindow* windowHandle = *(GLFWwindow**)viewport->PlatformUserData; // First member is GLFWwindow
			if (!windowHandle)
				continue;
			auto state = glfwGetKey(windowHandle, static_cast<int32_t>(keycode));
			if (state == GLFW_PRESS || state == GLFW_REPEAT)
			{
				pressed = true;
				break;
			}
		}
#endif

		return pressed;
	}

	bool Input::IsKeyReleased(const KeyCode key)
	{
		return m_KeyData.find(key) != m_KeyData.end() && m_KeyData[key].State == KeyState::Released;
	}

	void Input::SetCursorMode(ECursorMode mode)
	{
		LWindow& Window = LApplication::Get()->GetWindow();
		glfwSetInputMode(static_cast<GLFWwindow*>(Window.GetGlfwWindow()), GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode);

		if (LApplication::Get()->GetSpecification().ImGuiEnabled)
		{
			UI::SetInputEnabled(mode == ECursorMode::Normal);
		}
	}

	ECursorMode Input::GetCursorMode()
	{
		LWindow& Window = LApplication::Get()->GetWindow();
		return static_cast<ECursorMode>(glfwGetInputMode(static_cast<GLFWwindow*>(Window.GetGlfwWindow()), GLFW_CURSOR) - GLFW_CURSOR_NORMAL);
	}

	void Input::UpdateKeyState(KeyCode key, KeyState NewState)
	{
		auto& keyData = m_KeyData[key];
		keyData.Key = key;
		keyData.OldState = keyData.State;
		keyData.State = NewState;
	}

	void Input::UpdateButtonState(const EMouseButton Button, KeyState NewState)
	{
		FButtonData& mouseData = m_MouseData[Button];
		mouseData.Button = Button;
		mouseData.OldState = mouseData.State;
		mouseData.State = NewState;

		//LK_CORE_DEBUG_TAG("Input", "UpdateButtonState  Button='{}'  NewState='{}'", 
		//				  Enum::ToString(Button), Enum::ToString(NewState));

		LastMouseButton = Button;
	}

	void Input::TransitionPressedKeys()
	{
		for (const auto& [key, keyData] : m_KeyData)
		{
			if (keyData.State == KeyState::Pressed)
			{
				UpdateKeyState(key, KeyState::Held);
			}
		}
	}

	void Input::TransitionPressedButtons()
	{
		for (const auto& [Button, ButtonData] : m_MouseData)
		{
			if (ButtonData.State == KeyState::Pressed)
			{
				UpdateButtonState(Button, KeyState::Held);
			}
		}
	}

}