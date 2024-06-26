#include "LKpch.h"
#include "Window.h"

#include "LkEngine/Core/Application.h"
#include "LkEngine/Renderer/Renderer.h"

//#include "LkEngine/Platform/Vulkan/VulkanContext.h"
//#include "LkEngine/Platform/Vulkan/VulkanPipeline.h"


namespace LkEngine {

	static bool GLFW_Initialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		LK_CORE_ERROR_TAG("GLFW", "GLFW Error ({0}): {1}", error, description);
	}

	Window::Window(const WindowSpecification& specification)
		: m_Specification(specification)
		, m_Title(specification.Title)
		, m_Width(specification.Width)
		, m_Height(specification.Height)
		, m_ViewportWidth(specification.Width)
		, m_ViewportHeight(specification.Height)
		, m_VSync(specification.VSync)
	{
		m_Instance = this;

		// Window Data
		m_Data.Title = m_Title;
		m_Data.Width = m_Width;
		m_Data.Height = m_Height;
	}

	Window::~Window()
	{
	}
	
	void Window::Init()
	{
		if (GLFW_Initialized == false)
		{
			LK_ASSERT(glfwInit() == GLFW_TRUE, "GLFW failed to initialize, glfwInit() != GLFW_TRUE");
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		// Set context profile and the version to use for the Renderer API
		RenderContext::SetProfile(RenderContext::Profile::Core);

		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL: 
				RenderContext::SetVersion(4, 5);
				break;
		}

		m_Data.Title = m_Specification.Title;
		m_Data.Width = m_Specification.Width;
		m_Data.Height = m_Specification.Height;
	
		m_GlfwWindow = glfwCreateWindow((int)m_Width, (int)m_Height, m_Title.c_str(), nullptr, nullptr);
		LK_CORE_ASSERT(m_GlfwWindow != nullptr);
		glfwMakeContextCurrent(m_GlfwWindow);

		m_RenderContext = RenderContext::Create(this);
		m_RenderContext->Init(SourceBlendFunction::Alpha, DestinationBlendFunction::One_Minus_SourceAlpha);

		m_RenderContext->SetName("OpenGL-Context");

		SetVSync(true);
		LK_CORE_DEBUG_TAG("Graphics Context", "Name: {}", m_RenderContext->GetName());

		glfwSetWindowUserPointer(m_GlfwWindow, &m_Data);
		if (glfwRawMouseMotionSupported())
		{
			glfwSetInputMode(m_GlfwWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		}
		glfwSetInputMode(m_GlfwWindow, GLFW_STICKY_KEYS, GLFW_TRUE);
		glfwSetInputMode(m_GlfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetWindowSizeLimits(m_GlfwWindow, 420, 280, 2560, 1440);
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
#if 0
		glfwSetMouseButtonCallback(m_GlfwWindow, [](GLFWwindow* window, int button, int action, int mods)
		{
			auto& data = *((WindowData*)glfwGetWindowUserPointer(window));
			switch (action)
			{
				case GLFW_PRESS:
				{
					Input::UpdateButtonState((MouseButton)button, KeyState::Pressed);
					MouseButtonPressedEvent event((MouseButton)button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					Input::UpdateButtonState((MouseButton)button, KeyState::Released);
					MouseButtonReleasedEvent event((MouseButton)button);
					data.EventCallback(event);
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

		GLFW_Initialized = true;
	}

	void Window::SwapBuffers()
	{
		glfwSwapBuffers(m_GlfwWindow);
		glfwPollEvents();
	}
	
	void Window::Shutdown()
	{
		if (m_RenderContext)
		{
			m_RenderContext->Destroy();
			m_RenderContext->~RenderContext();
			m_RenderContext = nullptr;
		}
		if (m_GlfwWindow)
		{
			glfwDestroyWindow(m_GlfwWindow);
			glfwTerminate();

			delete m_GlfwWindow;
			m_GlfwWindow = nullptr;
		}
	}
	
	void Window::SetVSync(bool enabled)
	{
		m_VSync = enabled;
		if (m_VSync)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
	}

	void Window::SetSize(const glm::vec2& size)
	{
		m_Width = size.x;
		m_Height = size.y;
	}

    void Window::SetWidth(uint32_t width) 
	{ 
		m_Width = width; 
	}

    void Window::SetHeight(uint32_t height) 
	{ 
		m_Height = height; 
	}

	void Window::WindowResizeCallback(GLFWwindow* glfwWindow, int width, int height)
	{
		int size_x, size_y;
		glfwGetWindowSize(glfwWindow, &size_x, &size_y);
		auto& window = Window::Get();
		window.SetViewportWidth(size_x);
		window.SetViewportHeight(size_y);
		window.SetWidth(width);
		window.SetHeight(height);
		window.GetRenderContext()->UpdateResolution(width, height);
		LK_CORE_DEBUG("Window Resize: ({}, {})", window.GetWidth(), window.GetHeight());

		auto* editor = EditorLayer::Get();
		if (editor && editor->IsEnabled())
		{
			float editorWindowWidth = editor->GetEditorWindowSize().x;
			float editorWindowHeight = editor->GetEditorWindowSize().y;

			// Scale the resolution for the editor 'main' window
			editorWindowWidth /= window.GetScalerX();
			editorWindowHeight /= window.GetScalerY();
			
			window.SetWidth(editorWindowWidth);
			window.SetHeight(editorWindowHeight);
			window.GetRenderContext()->UpdateResolution(editorWindowWidth, editorWindowHeight);

			// Set the window size to be that of the editor window size,
			// this is because of the other docking windows in the Editor that occopy screen space
			editor->SetUpdateWindowFlag(true);

			LK_CORE_DEBUG("Editor enabled, setting width and height to -> ({}, {})", editorWindowWidth, editorWindowHeight);
		}
	}

	void Window::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
	}

	float Window::GetScalerX() const
	{
		return m_ViewportScalers.x;
	}
	
	float Window::GetScalerY() const
	{
		return m_ViewportScalers.y;
	}

	glm::vec2 Window::GetScalers() const
	{
		return m_ViewportScalers;
	}

	void Window::SetScalerX(float x)
	{
		m_ViewportScalers.x = x;
	}

	void Window::SetScalerY(float y)
	{
		m_ViewportScalers.y = y;
	}

	void Window::SetScalers(float x, float y)
	{
		m_ViewportScalers.x = x;
		m_ViewportScalers.y = y;
	}

	void Window::SetScalers(const glm::vec2& scalers)
	{
		m_ViewportScalers = scalers;
	}

	Ref<SwapChain> Window::GetSwapChain()
	{
		return m_SwapChain;
	}

	Ref<RenderPass> Window::GetRenderPass()
	{
		return nullptr;
	}

	void Window::ProcessEvents()
	{
		glfwPollEvents();
		Input::Update();
	}




}
