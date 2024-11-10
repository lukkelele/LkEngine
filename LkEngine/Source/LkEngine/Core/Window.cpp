#include "LKpch.h"
#include "Window.h"

#include "LkEngine/Core/Application.h"
#include "LkEngine/Core/Viewport.h"
#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

	static void GLFWErrorCallback(int Error, const char* Description)
	{
		LK_CORE_ERROR_TAG("GLFW", "Error ({0}): {1}", Error, Description);
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
		LCLASS_REGISTER();
		Instance = this;

		/* Window Data. */
		Data.Title = m_Title;
		Data.Width = static_cast<decltype(Data.Width)>(Size.X);
		Data.Height = static_cast<decltype(Data.Height)>(Size.Y);

		LK_CORE_DEBUG_TAG("Window", "Class Name: \"{}\"  IsA<LEntity>()={}  IsA<LObject>()={}", 
						  StaticClassName(), 
						  IsA<LEntity>() ? "TRUE" : "FALSE",
						  IsA<LObject>() ? "TRUE" : "FALSE");
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
		LRenderContext::SetProfile(ERenderProfile::Core);

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
	
		GlfwWindow = glfwCreateWindow(static_cast<int>(Size.X), static_cast<int>(Size.Y), m_Title.c_str(), nullptr, nullptr);
		LK_CORE_ASSERT(GlfwWindow != nullptr);
		glfwMakeContextCurrent(GlfwWindow);

		/* Create render context. */
		RenderContext = LRenderContext::Create(this);
		RenderContext->Initialize(
			ESourceBlendFunction::Alpha, 
			EDestinationBlendFunction::One_Minus_SourceAlpha
		);

		SetVSync(true);

		glfwSetWindowUserPointer(GlfwWindow, &Data);

		/* Input, mouse and Keyboard. */
		if (glfwRawMouseMotionSupported())
		{
			glfwSetInputMode(GlfwWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		}
		glfwSetInputMode(GlfwWindow, GLFW_STICKY_KEYS, GLFW_TRUE);
		glfwSetInputMode(GlfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetWindowSizeLimits(GlfwWindow, 420, 280, 2560, 1440);

		glfwSetWindowSizeCallback(GlfwWindow, WindowResizeCallback);

		glfwSetKeyCallback(GlfwWindow, [](GLFWwindow* Window, int Key, int ScanCode, int Action, int Modifiers)
		{
			FWindowData& WindowDataRef = *((FWindowData*)glfwGetWindowUserPointer(Window));

			switch (Action)
			{
				case GLFW_PRESS:
				{
					LInput::UpdateKeyState(static_cast<EKey>(Key), EKeyState::Pressed);
			#if 0
					KeyPressedEvent Event(static_cast<EKey>(Key), 0);
					WindowDataRef.EventCallback(Event);
					//WindowDataRef.OnWindowEvent.Broadcast(Event);
			#endif
					break;
				}

				case GLFW_RELEASE:
				{
			#if 0
					LInput::UpdateKeyState((EKey)Key, EKeyState::Released);
					KeyReleasedEvent Event((EKey)Key);
					WindowDataRef.EventCallback(Event);
			#endif
					break;
				}

				case GLFW_REPEAT:
				{
					LInput::UpdateKeyState(static_cast<EKey>(Key), EKeyState::Held);
				#if 0
					KeyPressedEvent Event(static_cast<EKey>(Key), 1);
					WindowDataRef.EventCallback(Event);
					WindowDataRef.OnWindowEvent.Broadcast(Event);
				#endif

					break;
				}
			}
		});

		/* Framebuffer resize callback. */
		glfwSetFramebufferSizeCallback(GlfwWindow, [](GLFWwindow* GlfwWindow, int Width, int Height)
		{
			LK_CORE_DEBUG_TAG("GLFW", "Framebuffer Size Callback  ({}, {})", Width, Height);
			/* FIXME: Should be no raw OpenGL calls. */
			glViewport(0, 0, Width, Height);
		});

		/* Character input callback. */
		glfwSetCharCallback(GlfwWindow, [](GLFWwindow* Window, uint32_t Codepoint)
		{
			FWindowData& WindowDataRef = *(static_cast<FWindowData*>(glfwGetWindowUserPointer(Window)));

			KeyTypedEvent Event(static_cast<EKey>(Codepoint));
			WindowDataRef.EventCallback(Event);
		});

		/* Mouse button callbacks. */
		glfwSetMouseButtonCallback(GlfwWindow, [](GLFWwindow* GlfwWindow, int Button, int Action, int Modifiers)
		{
			FWindowData& WindowDataRef = *static_cast<FWindowData*>(glfwGetWindowUserPointer(GlfwWindow));
			switch (Action)
			{
				case GLFW_PRESS:
				{
					double MousePosX, MousePosY;
					glfwGetCursorPos(GlfwWindow, &MousePosX, &MousePosY);

					const FMouseButtonData& ButtonData = LInput::UpdateButtonState(static_cast<EMouseButton>(Button), 
																				   EMouseButtonState::Pressed);
					WindowDataRef.OnMouseButtonPressed.Broadcast(ButtonData);

					break;
				}

				case GLFW_RELEASE:
				{
					double MousePosX, MousePosY;
					glfwGetCursorPos(GlfwWindow, &MousePosX, &MousePosY);

					const FMouseButtonData& ButtonData = LInput::UpdateButtonState(static_cast<EMouseButton>(Button), 
																				   EMouseButtonState::Pressed);
					WindowDataRef.OnMouseButtonReleased.Broadcast(ButtonData);

					break;
				}
			}
		});

		/* Cursor position callback. */
		glfwSetCursorPosCallback(GlfwWindow, [](GLFWwindow* Window, double x, double y)
		{
			FWindowData& WindowDataRef = *(static_cast<FWindowData*>(glfwGetWindowUserPointer(Window)));
		});

		/* Mouse scroll callback. */
		glfwSetScrollCallback(GlfwWindow, [](GLFWwindow* Window, double OffsetX, double OffsetY)
		{
			FWindowData& WindowData = *(static_cast<FWindowData*>(glfwGetWindowUserPointer(Window)));
			if (OffsetY > 0)
			{
				WindowData.OnMouseScrolled.Broadcast(EMouseScroll::Up);
			}
			else if (OffsetY < 0)
			{
				WindowData.OnMouseScrolled.Broadcast(EMouseScroll::Down);
			}

		});

		bGlfwInitialized = true;
	}

	void LWindow::SwapBuffers()
	{
		glfwSwapBuffers(GlfwWindow);
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
		if (GlfwWindow)
		{
			glfwDestroyWindow(GlfwWindow);
			glfwTerminate();

			GlfwWindow = nullptr;
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

	void LWindow::MouseButtonCallback(GLFWwindow* Window, int button, int action, int Modifiers)
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

		LInput::Update();
	}

}
