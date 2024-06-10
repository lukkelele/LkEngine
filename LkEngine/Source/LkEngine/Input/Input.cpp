#include "LKpch.h"
#include "Input.h"

#include "LkEngine/Core/Application.h"

#include "LkEngine/Editor/EditorLayer.h"



namespace LkEngine {

    Application* m_App = nullptr;

	void Input::Init()
	{
        Keyboard::Init();
        Mouse::Init();
	}

	void Input::SetScene(const Ref<Scene>& scene)
	{
		if (m_Scene == scene)
			return;
		m_Scene = scene;
	}

	void Input::Update()
	{
		if (m_Scene == nullptr)
			return;

		auto* editor = EditorLayer::Get();
		if (Mouse::IsButtonPressed(MouseButton::Left) == false)
		{
			if (Keyboard::IsKeyPressed(Key::Escape))
			{
				LK_CORE_TRACE("De-selecting (clicked escape)");
				if (editor && editor->IsEnabled())
				{
					Entity entity = { (entt::entity)0, m_Scene.Raw()};
					SELECTION::SelectedEntity = entity;
				}
			}
			return;
		}

        auto mousePos = Mouse::GetPos();
        auto raycastResults = Physics2D::RaycastFromScreen(*m_Scene);
		int raycastHits = raycastResults.size();

		if (raycastHits == 1)
		{
			LK_CORE_DEBUG("Hitcast result == 1");
		    Raycast2DResult raycast = raycastResults.at(0);
		    Entity entity = raycast.HitEntity;
		    //if ((Mouse::IsButtonPressed(MouseButton::ButtonLeft) && Editor::SelectedEntity != raycast.HitEntity))
		    if (Mouse::IsButtonPressed(MouseButton::Left))
		    {
				SELECTION::SelectedEntity = raycast.HitEntity;
		    }
		}
		else if (raycastHits > 1)
		{
			LK_CORE_WARN("Hitcast results > 1");
		    for (auto& raycast : raycastResults)
		    {
				SelectionContext::ClearEntitySelection();
		    }
		}
		else // NO HITS
		{
		}

        if (Keyboard::IsKeyPressed(Key::Escape))
        {
			LK_CORE_DEBUG("De-selecting (clicked escape)");
			if (editor && editor->IsEnabled())
			{
				SelectionContext::ClearEntitySelection();
			}
        }
	}

	bool Input::IsMouseButtonPressed(MouseButton button)
	{
		return m_MouseData.find(button) != m_MouseData.end() && m_MouseData[button].State == KeyState::Pressed;
	}

	bool Input::IsMouseButtonHeld(MouseButton button)
	{
		return m_MouseData.find(button) != m_MouseData.end() && m_MouseData[button].State == KeyState::Held;
	}

	bool Input::IsMouseButtonDown(MouseButton button)
	{
		bool imguiEnabled = Application::Get()->GetSpecification().ImGuiEnabled;
		if (imguiEnabled == false)
		{
			auto& window = Application::Get()->GetWindow();
			auto state = glfwGetMouseButton(static_cast<GLFWwindow*>(window.GetGlfwWindow()), static_cast<int32_t>(button));
			return state == GLFW_PRESS;
		}
	
		Window& window = Application::Get()->GetWindow();
		LK_CORE_ASSERT(&window != nullptr, "Window is nullptr");
		auto state = glfwGetMouseButton(static_cast<GLFWwindow*>(window.GetGlfwWindow()), static_cast<int32_t>(button));
		return state == GLFW_PRESS;

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

			auto state = glfwGetMouseButton(static_cast<GLFWwindow*>(windowHandle), static_cast<int32_t>(button));
			if (state == GLFW_PRESS || state == GLFW_REPEAT)
			{
				pressed = true;
				break;
			}
		}
		return pressed;
#endif
	}

	bool Input::IsMouseButtonReleased(MouseButton button)
	{
		return m_MouseData.find(button) != m_MouseData.end() && m_MouseData[button].State == KeyState::Released;
	}

	bool Input::IsKeyPressed(Key key)
	{
		return m_KeyData.find(key) != m_KeyData.end() && m_KeyData[key].State == KeyState::Pressed;
	}

	bool Input::IsKeyHeld(Key key)
	{
		return m_KeyData.find(key) != m_KeyData.end() && m_KeyData[key].State == KeyState::Held;
	}

	bool Input::IsKeyDown(Key keycode)
	{
		bool enableImGui = Application::Get()->GetSpecification().ImGuiEnabled;
		if (!enableImGui)
		{
			auto& window = Application::Get()->GetWindow();
			auto state = glfwGetKey(static_cast<GLFWwindow*>(window.GetGlfwWindow()), static_cast<int32_t>(keycode));
			return state == GLFW_PRESS || state == GLFW_REPEAT;
		}
		
		auto& window = Application::Get()->GetWindow();
		GLFWwindow* win = static_cast<GLFWwindow*>(window.GetGlfwWindow());
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

	bool Input::IsKeyReleased(KeyCode key)
	{
		return m_KeyData.find(key) != m_KeyData.end() && m_KeyData[key].State == KeyState::Released;
	}

	void Input::SetCursorMode(CursorMode mode)
	{
		auto& window = Application::Get()->GetWindow();
		glfwSetInputMode(static_cast<GLFWwindow*>(window.GetGlfwWindow()), GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode);

		if (Application::Get()->GetSpecification().ImGuiEnabled)
			UI::SetInputEnabled(mode == CursorMode::Normal);
	}

	CursorMode Input::GetCursorMode()
	{
		auto& window = Application::Get()->GetWindow();
		return (CursorMode)(glfwGetInputMode(static_cast<GLFWwindow*>(window.GetGlfwWindow()), GLFW_CURSOR) - GLFW_CURSOR_NORMAL);
	}

	void Input::UpdateKeyState(KeyCode key, KeyState newState)
	{
		auto& keyData = m_KeyData[key];
		keyData.Key = key;
		keyData.OldState = keyData.State;
		keyData.State = newState;
	}

	void Input::UpdateButtonState(MouseButton button, KeyState newState)
	{
		auto& mouseData = m_MouseData[button];
		mouseData.Button = button;
		mouseData.OldState = mouseData.State;
		mouseData.State = newState;
	}

	void Input::TransitionPressedKeys()
	{
		for (const auto& [key, keyData] : m_KeyData)
		{
			if (keyData.State == KeyState::Pressed)
				UpdateKeyState(key, KeyState::Held);
		}
	}

	void Input::TransitionPressedButtons()
	{
		for (const auto& [button, buttonData] : m_MouseData)
		{
			if (buttonData.State == KeyState::Pressed)
				UpdateButtonState(button, KeyState::Held);
		}
	}

}