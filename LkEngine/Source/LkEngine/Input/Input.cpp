#include "LKpch.h"
#include "Input.h"

#include "LkEngine/Core/Application.h"
#include "LkEngine/Scene/Scene.h"


#include "LkEngine/Editor/EditorLayer.h"

namespace LkEngine {

    LApplication* Application = nullptr; /* REMOVE ME */

	void LInput::Initialize()
	{
        LKeyboard::Initialize();

        LMouse::Initialize();
		MouseDataMap.insert({ EMouseButton::Button0, FMouseButtonData() });
		MouseDataMap.insert({ EMouseButton::Button1, FMouseButtonData() });
		MouseDataMap.insert({ EMouseButton::Button2, FMouseButtonData() });
		MouseDataMap.insert({ EMouseButton::Button3, FMouseButtonData() });
		MouseDataMap.insert({ EMouseButton::Button4, FMouseButtonData() });
		MouseDataMap.insert({ EMouseButton::Button5, FMouseButtonData() });
	}

	void LInput::SetScene(const TObjectPtr<LScene>& InScene)
	{
		if (!InScene || Scene == InScene)
		{
			return;
		}

		Scene = InScene;
	}

	void LInput::Update()
	{
		if (Scene == nullptr)
		{
			return;
		}

		LEditorLayer* Editor = LEditorLayer::Get();
		if (LMouse::IsButtonPressed(EMouseButton::Button0) == false)
		{
			if (LKeyboard::IsKeyPressed(EKey::Escape))
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

	#if 0
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
		    }
		}
		else if (raycastHits > 1)
		{
			LK_CORE_WARN("Hitcast results > 1");
		    for (auto& raycast : raycastResults)
		    {
		    }
		}
		else /* No hits. */
		{
		}

        if (LKeyboard::IsKeyPressed(EKey::Escape))
        {
			LK_CORE_DEBUG("De-selecting (clicked escape)");
			if (Editor && Editor->IsEnabled())
			{
			}
        }
	#endif
	}

	bool LInput::IsMouseButtonPressed(const EMouseButton Button)
	{
		return ((MouseDataMap.find(Button) != MouseDataMap.end()) && (MouseDataMap[Button].State == EMouseButtonState::Pressed));
	}

	bool LInput::IsMouseButtonHeld(const EMouseButton Button)
	{
		return ((MouseDataMap.find(Button) != MouseDataMap.end()) && (MouseDataMap[Button].State == EMouseButtonState::Held));
	}

	bool LInput::IsMouseButtonDown(const EMouseButton Button)
	{
		/* FIXME: Do not call application here */
		const bool bImguiEnabled = LApplication::Get()->GetSpecification().ImGuiEnabled;
		if (bImguiEnabled == false)
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

	bool LInput::IsMouseButtonReleased(const EMouseButton Button)
	{
		return (MouseDataMap.find(Button) != MouseDataMap.end() && MouseDataMap[Button].State == EMouseButtonState::Released);
	}

	bool LInput::IsAnyMouseButtonPressed()
	{
		auto IsMouseButtonPressed = [&](const std::pair<EMouseButton, FMouseButtonData>& InButtonData)
		{
			return (InButtonData.second.State == EMouseButtonState::Pressed);
		};
		auto Iter = std::find_if(MouseDataMap.begin(), MouseDataMap.end(), IsMouseButtonPressed);

		return Iter != MouseDataMap.end();
	}

	bool LInput::IsKeyPressed(const EKey Key)
	{
		return ((KeyDataMap.find(Key) != KeyDataMap.end()) && (KeyDataMap[Key].State == EKeyState::Pressed));
	}

	bool LInput::IsKeyHeld(const EKey Key)
	{
		return ((KeyDataMap.find(Key) != KeyDataMap.end()) && (KeyDataMap[Key].State == EKeyState::Held));
	}

	bool LInput::IsKeyDown(const EKey Key)
	{
		const bool bEnableImGui = LApplication::Get()->GetSpecification().ImGuiEnabled;
		if (!bEnableImGui)
		{
			auto& window = LApplication::Get()->GetWindow();
			auto state = glfwGetKey(static_cast<GLFWwindow*>(window.GetGlfwWindow()), static_cast<int32_t>(Key));
			return state == GLFW_PRESS || state == GLFW_REPEAT;
		}
		
		LWindow& WindowRef = LApplication::Get()->GetWindow();
		GLFWwindow* win = static_cast<GLFWwindow*>(WindowRef.GetGlfwWindow());
		bool pressed = false;
		auto state = glfwGetKey(win, static_cast<int32_t>(Key));
		if (state == GLFW_PRESS || state == GLFW_REPEAT)
		{
			pressed = true;
		}

		//------------------------------------------------------------
		// ImGui  Multi-Viewports
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
			auto state = glfwGetKey(windowHandle, static_cast<int32_t>(Key));
			if (state == GLFW_PRESS || state == GLFW_REPEAT)
			{
				pressed = true;
				break;
			}
		}
#endif

		return pressed;
	}

	bool LInput::IsKeyReleased(const EKey Key)
	{
		return ((KeyDataMap.find(Key) != KeyDataMap.end()) && (KeyDataMap[Key].State == EKeyState::Released));
	}

	void LInput::SetCursorMode(ECursorMode mode)
	{
		LWindow& Window = LApplication::Get()->GetWindow();
		glfwSetInputMode(static_cast<GLFWwindow*>(Window.GetGlfwWindow()), GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode);

		if (LApplication::Get()->GetSpecification().ImGuiEnabled)
		{
			UI::SetInputEnabled(mode == ECursorMode::Normal);
		}
	}

	ECursorMode LInput::GetCursorMode()
	{
		LWindow& Window = LApplication::Get()->GetWindow();
		const int InputMode = glfwGetInputMode(static_cast<GLFWwindow*>(Window.GetGlfwWindow()), GLFW_CURSOR) - GLFW_CURSOR_NORMAL;

		return static_cast<ECursorMode>(InputMode);
	}

	FKeyData& LInput::UpdateKeyState(const EKey Key, const EKeyState NewState)
	{
		FKeyData& KeyData = KeyDataMap[Key];
		KeyData.Key = Key;
		KeyData.OldState = KeyData.State;
		KeyData.State = NewState;

		return KeyData;
	}

	FMouseButtonData& LInput::UpdateButtonState(const EMouseButton Button, EMouseButtonState NewState)
	{
		FMouseButtonData& ButtonData = MouseDataMap[Button];
		ButtonData.Button = Button;
		ButtonData.OldState = ButtonData.State;
		ButtonData.State = NewState;
		LastMouseButton = Button;

		return ButtonData;
	}

	void LInput::TransitionPressedKeys()
	{
		for (const auto& [Key, KeyData] : KeyDataMap)
		{
			if (KeyData.State == EKeyState::Pressed)
			{
				UpdateKeyState(Key, EKeyState::Held);
			}
		}
	}

	void LInput::TransitionPressedButtons()
	{
		for (const auto& [Button, ButtonData] : MouseDataMap)
		{
			if (ButtonData.State == EMouseButtonState::Pressed)
			{
				UpdateButtonState(Button, EMouseButtonState::Held);
			}
		}
	}

}