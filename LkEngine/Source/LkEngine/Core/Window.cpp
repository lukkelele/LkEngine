#include "LKpch.h"
#include "Window.h"

#include "LkEngine/Core/Application.h"
#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

	static void GLFWErrorCallback(int error, const char* description)
	{
		LK_CORE_ERROR_TAG("GLFW", "GLFW Error ({0}): {1}", error, description);
	}

	static void GLFW_FramebufferSizeCallback(GLFWwindow* GLFWWindow, int Width, int Height)
	{
		/* Adjust the viewport when the framebuffer is resized. */
		glViewport(0, 0, Width, Height);
	}

	LWindow::LWindow(const FWindowSpecification& WindowSpecification)
		: Specification(WindowSpecification)
		, m_Title(WindowSpecification.Title)
		, Size({ WindowSpecification.Width, WindowSpecification.Height })
		, ViewportSize({ WindowSpecification.Width, WindowSpecification.Height })
		, m_VSync(WindowSpecification.VSync)
	{
		Instance = this;

		/* Window Data. */
		Data.Title = m_Title;
		Data.Width = static_cast<decltype(Data.Width)>(Size.X);
		Data.Height = static_cast<decltype(Data.Height)>(Size.Y);

		LCLASS_REGISTER();

		LK_CORE_DEBUG_TAG("Window", "StaticClassName: \"{}\"", StaticClass());
	}

	LWindow::~LWindow()
	{
		/* TODO: */
	}
	
	void LWindow::Initialize()
	{
		LObject::Initialize();

		if (!bGlfwInitialized)
		{
			LK_ASSERT(glfwInit() == GLFW_TRUE, "GLFW failed to initialize, glfwInit() != GLFW_TRUE");
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		/* Set context profile and the version to use for the Renderer API. */
		LRenderContext::SetProfile(LRenderContext::EProfile::Core);

		switch (LRendererAPI::Current())
		{
			case ERendererAPI::OpenGL: 
			{
				LRenderContext::SetVersion(4, 5);
				break;
			}
		}

		Data.Title = Specification.Title;
		Data.Width = Specification.Width;
		Data.Height = Specification.Height;
	
		m_GlfwWindow = glfwCreateWindow(static_cast<int>(Size.X), static_cast<int>(Size.Y), m_Title.c_str(), nullptr, nullptr);
		LK_CORE_ASSERT(m_GlfwWindow != nullptr);
		glfwMakeContextCurrent(m_GlfwWindow);

		/* Create render context. */
		RenderContext = LRenderContext::Create(this);
		RenderContext->Init(
			ESourceBlendFunction::Alpha, 
			EDestinationBlendFunction::One_Minus_SourceAlpha
		);

		RenderContext->SetName(LString::Format("{}-Context", LRendererAPI::GetName().CStr()).CStr());

		SetVSync(true);
		LK_CORE_DEBUG_TAG("Graphics Context", "Name: {}", RenderContext->GetName());

		glfwSetWindowUserPointer(m_GlfwWindow, &Data);

		/* Input, mouse and keyboard. */
		if (glfwRawMouseMotionSupported())
		{
			glfwSetInputMode(m_GlfwWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		}
		glfwSetInputMode(m_GlfwWindow, GLFW_STICKY_KEYS, GLFW_TRUE);
		glfwSetInputMode(m_GlfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetWindowSizeLimits(m_GlfwWindow, 420, 280, 2560, 1440);

		glfwSetWindowSizeCallback(m_GlfwWindow, WindowResizeCallback);

		glfwSetKeyCallback(m_GlfwWindow, [](GLFWwindow* Window, int key, int scancode, int Action, int mods)
		{
			FWindowData& DataRef = *((FWindowData*)glfwGetWindowUserPointer(Window));

			switch (Action)
			{
				case GLFW_PRESS:
				{
					Input::UpdateKeyState((KeyCode)key, KeyState::Pressed);
					KeyPressedEvent event((KeyCode)key, 0);
					DataRef.EventCallback(event);
					break;
				}

				case GLFW_RELEASE:
				{
					Input::UpdateKeyState((KeyCode)key, KeyState::Released);
					KeyReleasedEvent event((KeyCode)key);
					DataRef.EventCallback(event);
					break;
				}

				case GLFW_REPEAT:
				{
					Input::UpdateKeyState((KeyCode)key, KeyState::Held);
					KeyPressedEvent event((KeyCode)key, 1);
					DataRef.EventCallback(event);
					break;
				}
			}
		});

		/* Framebuffer resize callback. */
		glfwSetFramebufferSizeCallback(m_GlfwWindow, [](GLFWwindow* GlfwWindow, int Width, int Height)
		{
			LK_CORE_DEBUG_TAG("GLFW", "Framebuffer Size Callback  ({}, {})", Width, Height);
			glViewport(0, 0, Width, Height);
		});

		/* Character callback. */
		glfwSetCharCallback(m_GlfwWindow, [](GLFWwindow* Window, uint32_t codepoint)
		{
			FWindowData& DataRef = *((FWindowData*)glfwGetWindowUserPointer(Window));

			KeyTypedEvent Event((KeyCode)codepoint);
			DataRef.EventCallback(Event);
		});

		/* Mouse button callback. */
#if 1
		glfwSetMouseButtonCallback(m_GlfwWindow, [](GLFWwindow* GlfwWindow, int Button, int Action, int Modifiers)
		{
			FWindowData& DataRef = *((FWindowData*)glfwGetWindowUserPointer(GlfwWindow));
			switch (Action)
			{
				case GLFW_PRESS:
				{
					Input::UpdateButtonState(static_cast<EMouseButton>(Button), KeyState::Pressed);

					double MousePosX, MousePosY;
					glfwGetCursorPos(GlfwWindow, &MousePosX, &MousePosY);
					LMouseButtonPressedEvent Event(static_cast<EMouseButton>(Button), 
												   { MousePosX, MousePosY });
					DataRef.EventCallback(Event);
					break;
				}

				case GLFW_RELEASE:
				{
					Input::UpdateButtonState(static_cast<EMouseButton>(Button), KeyState::Released);

					double MousePosX, MousePosY;
					glfwGetCursorPos(GlfwWindow, &MousePosX, &MousePosY);
					LMouseButtonReleasedEvent Event(static_cast<EMouseButton>(Button), 
												   { MousePosX, MousePosY });
					DataRef.EventCallback(Event);
					break;
				}
			}
		});
#endif

		/* Cursor position callback. */
		glfwSetCursorPosCallback(m_GlfwWindow, [](GLFWwindow* Window, double x, double y)
		{
			FWindowData& data = *((FWindowData*)glfwGetWindowUserPointer(Window));
			MouseMovedEvent event((float)x, (float)y);
			data.EventCallback(event);
		});

		/* Mouse scroll callback. */
		glfwSetScrollCallback(m_GlfwWindow, [](GLFWwindow* Window, double xOffset, double yOffset)
		{
			FWindowData& data = *((FWindowData*)glfwGetWindowUserPointer(Window));
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
	
	void LWindow::SetVSync(bool InEnabled)
	{
		m_VSync = InEnabled;
		if (m_VSync)
		{
			glfwSwapInterval(1);
		}
		else
		{
			glfwSwapInterval(0);
		}
	}

	void LWindow::WindowResizeCallback(GLFWwindow* glfwWindow, int Width, int Height)
	{
		int SizeX, SizeY;
		glfwGetWindowSize(glfwWindow, &SizeX, &SizeY);

		LWindow& Window = LWindow::Get();
		Window.SetViewportWidth(SizeX);
		Window.SetViewportHeight(SizeY);

		Window.SetSize({ Width, Height });
		LK_CORE_DEBUG_TAG("Window", "Resize: ({}, {})   Viewport ({}, {})", 
						  Window.GetWidth(), Window.GetHeight(), 
						  Window.GetViewportWidth(), Window.GetViewportHeight());

	#if 0
		if (LEditorLayer* Editor = LEditorLayer::Get(); Editor && Editor->IsEnabled())
		{
			float EditorWindowWidth = Editor->GetEditorWindowSize().x;
			float EditorWindowHeight = Editor->GetEditorWindowSize().y;

			/* Scale the resolution for the Editor main window. */
			EditorWindowWidth /= Window.GetScalerX();
			EditorWindowHeight /= Window.GetScalerY();
			
			Window.SetWidth(static_cast<uint32_t>(EditorWindowWidth));
			Window.SetHeight(EditorWindowHeight);
		}
	#endif
	}

	void LWindow::MouseButtonCallback(GLFWwindow* Window, int button, int action, int mods)
	{
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
