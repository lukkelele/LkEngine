#include "LKpch.h"
#include "Windows_Window.h"

#include "LkEngine/Core/Application.h"
#include "LkEngine/Renderer/Renderer.h"


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
	
	void Windows_Window::Init()
	{
		if (GLFW_Initialized)
			throw std::runtime_error("Windows_Window::Init has already been called once! (GLFW already initialized)");

		if (glfwInit() != GLFW_TRUE)
			throw std::runtime_error("Could not initialize GLFW");
	
		// Set context profile and the version to use for the Renderer API
		GraphicsContext::SetProfile(GraphicsContext::Profile::Core);

	#ifdef LK_RENDERER_API_OPENGL
		GraphicsContext::SetVersion(4, 5);

	#elif defined(LK_RENDERER_API_VULKAN)
		GraphicsContext::SetVersion(MAJOR_VERSION, MINOR_VERSION);
	#endif

		glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	
		m_GlfwWindow = glfwCreateWindow((int)m_Width, (int)m_Height, m_Title.c_str(), nullptr, nullptr);
		LK_ASSERT(m_GlfwWindow != nullptr);
		glfwMakeContextCurrent(m_GlfwWindow);

		if (!GLFW_Initialized)
		{
			m_GraphicsContext = GraphicsContext::Create(this, LK_SHADER_VERSION);
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

		Mouse::SetActiveWindow(this);

		GLFW_Initialized = true;
	}

	void Windows_Window::SwapBuffers()
	{
		glfwSwapBuffers(m_GlfwWindow);
		glfwPollEvents();
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
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
	}

	glm::vec2 Windows_Window::GetPos() const
	{
		return m_Pos; 
	}

	glm::vec2 Windows_Window::GetSize() const
	{
		return glm::vec2(m_Width, m_Height);
	}

	void Windows_Window::SetSize(const glm::vec2& size)
	{
		m_Width = size.x;
		m_Height = size.y;
	}

	glm::vec2 Windows_Window::GetViewportSize() const
	{
		return { m_ViewportWidth, m_ViewportHeight };
	}

    void Windows_Window::SetWidth(uint16_t width) 
	{ 
		m_Width = width; 
	}

    void Windows_Window::SetHeight(uint16_t height) 
	{ 
		m_Height = height; 
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

		auto* editor = Editor::Get();
		if (editor && editor->IsEnabled())
		{
			float editorWindowWidth = editor->GetEditorWindowSize().x;
			float editorWindowHeight = editor->GetEditorWindowSize().y;

			// Scale the resolution for the editor 'main' window
			editorWindowWidth /= m_Instance->GetScalerX();
			editorWindowHeight /= m_Instance->GetScalerY();
			
			m_Instance->SetWidth(editorWindowWidth);
			m_Instance->SetHeight(editorWindowHeight);
			m_Instance->GetContext()->UpdateResolution(editorWindowWidth, editorWindowHeight);

			// Set the window size to be that of the editor window size,
			// this is because of the other docking windows in the Editor that occopy screen space
			editor->SetUpdateWindowFlag(true);

			LOG_DEBUG("Editor enabled, setting width and height to -> ({}, {})", editorWindowWidth, editorWindowHeight);
		}
	}

	void Windows_Window::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
	}

	float Windows_Window::GetScalerX() const
	{
		return m_ViewportScalers.x;
	}
	
	float Windows_Window::GetScalerY() const
	{
		return m_ViewportScalers.y;
	}

	glm::vec2 Windows_Window::GetScalers() const
	{
		return m_ViewportScalers;
	}

	void Windows_Window::SetScalerX(float x)
	{
		m_ViewportScalers.x = x;
	}

	void Windows_Window::SetScalerY(float y)
	{
		m_ViewportScalers.y = y;
	}

	void Windows_Window::SetScalers(float x, float y)
	{
		m_ViewportScalers.x = x;
		m_ViewportScalers.y = y;
	}

	void Windows_Window::SetScalers(const glm::vec2& scalers)
	{
		m_ViewportScalers = scalers;
	}

}
