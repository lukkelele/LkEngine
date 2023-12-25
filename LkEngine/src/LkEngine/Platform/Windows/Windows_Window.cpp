#include "LKpch.h"
#include "LkEngine/Platform/Windows/Windows_Window.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Core/Application.h"
#include "LkEngine/UI/UILayer.h"


namespace LkEngine {

	Windows_Window::Windows_Window(const char* title, uint32_t width, uint32_t height)
	{
		m_Instance = this;
		m_Title = title;
		m_Width = width;
		m_Height = height;
		m_ViewportWidth = width;
		m_ViewportHeight = height;
	}

	Windows_Window::Windows_Window(const ApplicationSpecification& specification)
	{
		m_Instance = this;
		m_Title = specification.Title;
		m_Width = specification.Width;
		m_Height = specification.Height;
		m_ViewportWidth = specification.Width;
		m_ViewportHeight = specification.Height;
	}

	Windows_Window::~Windows_Window()
	{
	}
	
	void Windows_Window::Init(const std::string& glslVersion)
	{
		if (GLFW_Initialized)
		{
			throw std::runtime_error("Windows_Window::Init has already been called once!");
			//return;
		}

		int glfwInitResult = glfwInit();
		LK_ASSERT(glfwInitResult == GLFW_TRUE);
	
		GraphicsContext::SetProfile(GraphicsContext::Profile::Core);
		GraphicsContext::SetVersion(4, 5);
	
		m_GlfwWindow = glfwCreateWindow((int)m_Width, (int)m_Height, m_Title.c_str(), nullptr, nullptr);
		LK_ASSERT(m_GlfwWindow != nullptr);
		glfwMakeContextCurrent(m_GlfwWindow);

		if (!GLFW_Initialized)
		{
			m_GraphicsContext = GraphicsContext::Create(this, glslVersion);
			m_GraphicsContext->Init(SourceBlendFunction::Alpha, DestinationBlendFunction::One_Minus_SourceAlpha);
			m_GraphicsContext->SetDarkTheme();
		}
	
		SetVSync(true);
	
		if (glfwRawMouseMotionSupported())
		{
			glfwSetInputMode(m_GlfwWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		}
		glfwSetWindowAttrib(m_GlfwWindow, GLFW_FOCUSED, GL_TRUE);
		glfwSetInputMode(m_GlfwWindow, GLFW_STICKY_KEYS, GLFW_TRUE);
		glfwSetInputMode(m_GlfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetWindowSizeLimits(m_GlfwWindow, 420, 280, 2560, 1440);
		glfwSetWindowSizeCallback(m_GlfwWindow, WindowResizeCallback);
		//glfwSetMouseButtonCallback(m_GlfwWindow, MouseButtonCallback);

		GLFW_Initialized = true;
	}

	void Windows_Window::SwapBuffers()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_GlfwWindow);
	}
	
	void Windows_Window::Exit()
	{
		glfwTerminate();
		glfwDestroyWindow(m_GlfwWindow);
	}
	
	void Windows_Window::SetVSync(bool enabled)
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

	glm::vec2 Windows_Window::GetPos() const
	{
		return m_Pos; 
	}

	glm::vec2 Windows_Window::GetSize() const
	{
		return glm::vec2(m_Width, m_Height);
	}

	glm::vec2 Windows_Window::GetViewportSize() const
	{
		return { m_ViewportWidth, m_ViewportHeight };
	}

	void Windows_Window::WindowResizeCallback(GLFWwindow* window, int width, int height)
	{
		int size_x, size_y;
		glfwGetWindowSize(window, &size_x, &size_y);
		m_Instance->SetViewportWidth(size_x);
		m_Instance->SetViewportHeight(size_y);
		m_Instance->SetWidth(width);
		m_Instance->SetHeight(height);
		m_Instance->GetContext()->UpdateResolution(width, height);
		LOG_DEBUG("Window Resize: ({}, {})", m_Instance->GetWidth(), m_Instance->GetHeight());
	}

	void Windows_Window::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		//Input::HandleScene(*Scene::GetActiveScene());
#if 0
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !Mouse::LeftMouseButtonProcessed)
		{
			LOG_DEBUG("Processing left mouse button click");

			Input::HandleScene(*Scene::GetActiveScene());
			// Set flag to true to avoid processing again
			Mouse::LeftMouseButtonProcessed = true;
		}
		// Reset the flag when the button is released
		else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) 
		{
			Mouse::LeftMouseButtonProcessed = false;
		}
#endif
	}

}
