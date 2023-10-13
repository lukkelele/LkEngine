#include "Platform/Windows/Windows_Window.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Core/Application.h"
#include "LkEngine/UI/UI.h"


namespace LkEngine {

	Windows_Window::Windows_Window(const char* title, uint32_t width, uint32_t height)
	{
		m_Title = title;
		m_Width = width;
		m_Height = height;
	}
	
	Windows_Window::~Windows_Window()
	{
	}
	
	void Windows_Window::Init(const std::string& glslVersion)
	{
		if (GLFW_Initialized)
			return;
	
		int glfwRes = glfwInit();
		// printf("glfwRes: %d", glfwRes);
	
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Set core profile
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OpenGL_Major_Version); 
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OpenGL_Minor_Version);
	
		// m_GlfwWindow = glfwCreateWindow((int)m_Width, (int)m_Height, m_Title.c_str(), nullptr, nullptr);
		m_GlfwWindow = std::make_shared<GLFWwindow*>(glfwCreateWindow((int)m_Width, (int)m_Height, m_Title.c_str(), nullptr, nullptr));
		LK_ASSERT(m_GlfwWindow != nullptr);
		glfwMakeContextCurrent(*m_GlfwWindow);
	
		if (!GLFW_Initialized)
		{
			m_Context = std::make_shared<GraphicsContext>(this);
			m_Context->Init();
			m_Context->SetDarkTheme();
		}
	
		SetVSync(true);
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
		if (glfwRawMouseMotionSupported())
			glfwSetInputMode(*m_GlfwWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		glfwSetWindowAttrib(*m_GlfwWindow, GLFW_FOCUSED, GL_TRUE);
		glfwSetInputMode(*m_GlfwWindow, GLFW_STICKY_KEYS, GLFW_TRUE);
		glfwSetInputMode(*m_GlfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	
		GLFW_Initialized = true;
	}
	
	void Windows_Window::Exit()
	{
		glfwTerminate();
		glfwDestroyWindow(*m_GlfwWindow);
	}
	
	void Windows_Window::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(*m_GlfwWindow);
	}
	
	void Windows_Window::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
		m_VSync = enabled;
	}
	
	bool Windows_Window::IsVSync() const
	{
		return m_VSync;
	}

}
