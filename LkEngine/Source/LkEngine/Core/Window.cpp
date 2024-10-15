#include "LKpch.h"
#include "Window.h"

#include "LkEngine/Core/Application.h"
#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

	static void GLFWErrorCallback(int error, const char* description)
	{
		LK_CORE_ERROR_TAG("GLFW", "GLFW Error ({0}): {1}", error, description);
	}

	LWindow::LWindow(const FWindowSpecification& WindowSpecification)
		: Specification(WindowSpecification)
		, m_Title(WindowSpecification.Title)
		, m_Width(WindowSpecification.Width)
		, m_Height(WindowSpecification.Height)
		, m_ViewportWidth(WindowSpecification.Width)
		, m_ViewportHeight(WindowSpecification.Height)
		, m_VSync(WindowSpecification.VSync)
	{
		Instance = this;

		/* Window Data. */
		m_Data.Title = m_Title;
		m_Data.Width = m_Width;
		m_Data.Height = m_Height;

		LCLASS_REGISTER();

		LK_CORE_DEBUG_TAG("Window", "StaticClassName: \"{}\"", StaticClass());
	}

	LWindow::~LWindow()
	{
	}
	
	void LWindow::Init()
	{
		if (!bGlfwInitialized)
		{
			LK_ASSERT(glfwInit() == GLFW_TRUE, "GLFW failed to initialize, glfwInit() != GLFW_TRUE");
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		// Set context profile and the version to use for the Renderer API
		LRenderContext::SetProfile(LRenderContext::EProfile::Core);

		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: 
			{
				LRenderContext::SetVersion(4, 5);
				break;
			}
		}

		m_Data.Title = Specification.Title;
		m_Data.Width = Specification.Width;
		m_Data.Height = Specification.Height;
	
		m_GlfwWindow = glfwCreateWindow((int)m_Width, (int)m_Height, m_Title.c_str(), nullptr, nullptr);
		LK_CORE_ASSERT(m_GlfwWindow != nullptr);
		glfwMakeContextCurrent(m_GlfwWindow);

		/* Create render context. */
		RenderContext = LRenderContext::Create(this);
		RenderContext->Init(
			ESourceBlendFunction::Alpha, 
			EDestinationBlendFunction::One_Minus_SourceAlpha
		);

		RenderContext->SetName("OpenGL-Context");

		SetVSync(true);
		LK_CORE_DEBUG_TAG("Graphics Context", "Name: {}", RenderContext->GetName());

		glfwSetWindowUserPointer(m_GlfwWindow, &m_Data);

		/* Input, mouse and keyboard. */
		if (glfwRawMouseMotionSupported())
		{
			glfwSetInputMode(m_GlfwWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		}
		glfwSetInputMode(m_GlfwWindow, GLFW_STICKY_KEYS, GLFW_TRUE);
		glfwSetInputMode(m_GlfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetWindowSizeLimits(m_GlfwWindow, 420, 280, 2560, 1440);

		// Callback functions.
		glfwSetWindowSizeCallback(m_GlfwWindow, WindowResizeCallback);

		glfwSetKeyCallback(m_GlfwWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			auto& data = *((WindowData*)glfwGetWindowUserPointer(window));

			switch (action)
			{
				case GLFW_PRESS:
				{
					Input::UpdateKeyState((KeyCode)key, KeyState::Pressed);
					KeyPressedEvent event((KeyCode)key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					Input::UpdateKeyState((KeyCode)key, KeyState::Released);
					KeyReleasedEvent event((KeyCode)key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					Input::UpdateKeyState((KeyCode)key, KeyState::Held);
					KeyPressedEvent event((KeyCode)key, 1);
					data.EventCallback(event);
					break;
				}
			}
		});

		// Character callback
		glfwSetCharCallback(m_GlfwWindow, [](GLFWwindow* window, uint32_t codepoint)
		{
			auto& data = *((WindowData*)glfwGetWindowUserPointer(window));
			KeyTypedEvent event((KeyCode)codepoint);
			data.EventCallback(event);
		});

		// Mouse button callback
#if 1
		glfwSetMouseButtonCallback(m_GlfwWindow, [](GLFWwindow* GlfwWindow, int Button, int Action, int Modifiers)
		{
			WindowData& Data = *((WindowData*)glfwGetWindowUserPointer(GlfwWindow));
			switch (Action)
			{
				case GLFW_PRESS:
				{
					Input::UpdateButtonState(static_cast<EMouseButton>(Button), KeyState::Pressed);

					double MousePosX, MousePosY;
					glfwGetCursorPos(GlfwWindow, &MousePosX, &MousePosY);
					LMouseButtonPressedEvent Event(static_cast<EMouseButton>(Button), 
												   { MousePosX, MousePosY });
					Data.EventCallback(Event);
					break;
				}
				case GLFW_RELEASE:
				{
					Input::UpdateButtonState(static_cast<EMouseButton>(Button), KeyState::Released);

					double MousePosX, MousePosY;
					glfwGetCursorPos(GlfwWindow, &MousePosX, &MousePosY);
					LMouseButtonReleasedEvent Event(static_cast<EMouseButton>(Button), 
												   { MousePosX, MousePosY });
					Data.EventCallback(Event);
					break;
				}
			}
		});
#endif

		// Cursor position callback
		glfwSetCursorPosCallback(m_GlfwWindow, [](GLFWwindow* window, double x, double y)
		{
			auto& data = *((WindowData*)glfwGetWindowUserPointer(window));
			MouseMovedEvent event((float)x, (float)y);
			data.EventCallback(event);
		});

		// Mouse scroll callback
		glfwSetScrollCallback(m_GlfwWindow, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			//LK_CORE_TRACE_TAG("Glfw Callback", "Scroll, (x, y)", xOffset, yOffset);
			auto& data = *((WindowData*)glfwGetWindowUserPointer(window));
			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		bGlfwInitialized = true;
	}

	void LWindow::SwapBuffers()
	{
		glfwSwapBuffers(m_GlfwWindow);
		glfwPollEvents();
	}
	
	void LWindow::Shutdown()
	{
		if (RenderContext)
		{
			RenderContext->Destroy();
			//RenderContext->~LRenderContext();
			RenderContext.Release();
			RenderContext = nullptr;
		}
		if (m_GlfwWindow)
		{
			glfwDestroyWindow(m_GlfwWindow);
			glfwTerminate();

			m_GlfwWindow = nullptr;
		}
	}
	
	void LWindow::SetVSync(bool enabled)
	{
		m_VSync = enabled;
		if (m_VSync)
		{
			glfwSwapInterval(1);
		}
		else
		{
			glfwSwapInterval(0);
		}
	}

	void LWindow::SetSize(const glm::vec2& InSize)
	{
		m_Width = static_cast<decltype(m_Width)>(InSize.x);
		m_Height = static_cast<decltype(m_Height)>(InSize.y);
	}

	void LWindow::WindowResizeCallback(GLFWwindow* glfwWindow, int width, int height)
	{
		int size_x, size_y;
		glfwGetWindowSize(glfwWindow, &size_x, &size_y);
		auto& window = LWindow::Get();
		window.SetViewportWidth(size_x);
		window.SetViewportHeight(size_y);
		window.SetWidth(width);
		window.SetHeight(height);
		window.GetRenderContext()->UpdateResolution(width, height);
		LK_CORE_DEBUG("Window Resize: ({}, {})", window.GetWidth(), window.GetHeight());

		if (LEditorLayer* Editor = LEditorLayer::Get(); Editor && Editor->IsEnabled())
		{
			float EditorWindowWidth = Editor->GetEditorWindowSize().x;
			float EditorWindowHeight = Editor->GetEditorWindowSize().y;

			// Scale the resolution for the Editor 'main' window
			EditorWindowWidth /= window.GetScalerX();
			EditorWindowHeight /= window.GetScalerY();
			
			window.SetWidth(static_cast<uint32_t>(EditorWindowWidth));
			window.SetHeight(EditorWindowHeight);
			window.GetRenderContext()->UpdateResolution(EditorWindowWidth, EditorWindowHeight);

			// Set the window size to be that of the Editor window size,
			// this is because of the other docking windows in the Editor that occopy screen space
			Editor->SetUpdateWindowFlag(true);

			LK_CORE_DEBUG("Editor enabled, setting width and height to -> ({}, {})", EditorWindowWidth, EditorWindowHeight);
		}
	}

	void LWindow::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
	}

	float LWindow::GetScalerX() const
	{
		return m_ViewportScalers.x;
	}
	
	float LWindow::GetScalerY() const
	{
		return m_ViewportScalers.y;
	}

	glm::vec2 LWindow::GetScalers() const
	{
		return m_ViewportScalers;
	}

	void LWindow::SetScalerX(float x)
	{
		m_ViewportScalers.x = x;
	}

	void LWindow::SetScalerY(float y)
	{
		m_ViewportScalers.y = y;
	}

	void LWindow::SetScalers(float x, float y)
	{
		m_ViewportScalers.x = x;
		m_ViewportScalers.y = y;
	}

	void LWindow::SetScalers(const glm::vec2& scalers)
	{
		m_ViewportScalers = scalers;
	}

	TObjectPtr<LSwapChain> LWindow::GetSwapChain()
	{
		return m_SwapChain;
	}

	TObjectPtr<LRenderPass> LWindow::GetRenderPass()
	{
		return nullptr;
	}

	void LWindow::ProcessEvents()
	{
		glfwPollEvents();
		Input::Update();
	}

}
