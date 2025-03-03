#include "LKpch.h"
#include "Window.h"

#include "LkEngine/Core/Application.h"
#include "LkEngine/Core/Viewport.h"
#include "LkEngine/Core/Input/Keyboard.h"
#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

	LWindow::LWindow(const FWindowSpecification& WindowSpecification)
		: Specification(WindowSpecification)
		, Title(WindowSpecification.Title)
		, Size({ WindowSpecification.Width, WindowSpecification.Height })
		, ViewportSize({ WindowSpecification.Width, WindowSpecification.Height })
	{
		LOBJECT_REGISTER();
		Instance = this;

		Data.Title = Title;
		Data.Width = static_cast<uint32_t>(Size.X);
		Data.Height = static_cast<uint32_t>(Size.Y);
		Data.bVSync = WindowSpecification.bVSync;
	}

	LWindow::~LWindow()
	{
		if (GlfwWindow)
		{
			LK_CORE_TRACE_TAG("Window", "Invoking Destroy");
			Destroy();
		}
	}

	void LWindow::Initialize()
	{
		LObject::Initialize();

		if (!bGlfwInitialized)
		{
			const int GlfwInit = glfwInit();
			LK_VERIFY(GlfwInit == GLFW_TRUE, "GLFW failed to initialize, error ({}): '{}'", GlfwInit, strerror(errno));
			glfwSetErrorCallback([](const int Error, const char* Description)
			{
				LK_CORE_ERROR_TAG("GLFW", "Error ({0}): {1}", Error, Description);
			});
		}

		/* Set context profile and the version to use for the Renderer API. */
		LRenderContext::SetProfile(ERenderProfile::Core);

		switch (LRendererAPI::Get())
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
	
		/* Create GLFW window. */
		GlfwWindow = glfwCreateWindow((int)Size.X, (int)Size.Y, Title.c_str(), nullptr, nullptr);
		LK_CORE_VERIFY(GlfwWindow != nullptr);
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
		glfwSetWindowSizeLimits(GlfwWindow, 340, 200, 5000, 5000);

		glfwSetWindowSizeCallback(GlfwWindow, [](GLFWwindow* InGlfwWindow, int NewWidth, int NewHeight) 
		{
			LWindow& Window = LWindow::Get();
			Window.SetViewportWidth(NewWidth);
			Window.SetViewportHeight(NewHeight);
			Window.OnViewportSizeUpdated.Broadcast(NewWidth, NewHeight);

			Window.SetSize({ NewWidth, NewHeight });
		});

		glfwSetKeyCallback(GlfwWindow, [](GLFWwindow* Window, int Key, int ScanCode, int Action, int Modifiers)
		{
			FWindowData& WindowDataRef = *((FWindowData*)glfwGetWindowUserPointer(Window));

			switch (Action)
			{
				case GLFW_PRESS:
				{
					const FKeyData& KeyData = LInput::UpdateKeyState(static_cast<EKey>(Key), EKeyState::Pressed);
					LKeyboard::OnKeyPressed.Broadcast(KeyData);
					break;
				}

				case GLFW_RELEASE:
				{
					const FKeyData& KeyData = LInput::UpdateKeyState(static_cast<EKey>(Key), EKeyState::Released);
					LKeyboard::OnKeyReleased.Broadcast(KeyData);
					break;
				}

				case GLFW_REPEAT:
				{
					FKeyData& KeyData = LInput::UpdateKeyState(static_cast<EKey>(Key), EKeyState::Held);
					LKeyboard::OnKeyHeld.Broadcast(KeyData);
					break;
				}
			}
		});

		/* Framebuffer resize callback. */
		glfwSetFramebufferSizeCallback(GlfwWindow, [](GLFWwindow* GlfwWindow, int Width, int Height)
		{
			LViewport::SetViewport(0, 0, Width, Height);
		});

	#if 0
		/* Character input callback. */
		glfwSetCharCallback(GlfwWindow, [](GLFWwindow* Window, uint32_t Codepoint)
		{
			FWindowData& WindowDataRef = *(static_cast<FWindowData*>(glfwGetWindowUserPointer(Window)));

			KeyTypedEvent Event(static_cast<EKey>(Codepoint));
			WindowDataRef.EventCallback(Event);
		});
	#endif

		/* Mouse button callbacks. */
		glfwSetMouseButtonCallback(GlfwWindow, [](GLFWwindow* GlfwWindow, int Button, int Action, int Modifiers)
		{
			switch (Action)
			{
				case GLFW_PRESS:
				{
					double MousePosX, MousePosY;
					glfwGetCursorPos(GlfwWindow, &MousePosX, &MousePosY);

					const FMouseButtonData& ButtonData = LInput::UpdateButtonState(
						static_cast<EMouseButton>(Button), 
						EMouseButtonState::Pressed
					);
					LMouse::OnMouseButtonPressed.Broadcast(ButtonData);

					break;
				}

				case GLFW_RELEASE:
				{
					double MousePosX, MousePosY;
					glfwGetCursorPos(GlfwWindow, &MousePosX, &MousePosY);

					const FMouseButtonData& ButtonData = LInput::UpdateButtonState(
						static_cast<EMouseButton>(Button), 
						EMouseButtonState::Pressed
					);
					LMouse::OnMouseButtonReleased.Broadcast(ButtonData);

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
			if (OffsetY > 0)
			{
				LMouse::OnMouseScrolled.Broadcast(EMouseScrollDirection::Up);
			}
			else if (OffsetY < 0)
			{
				LMouse::OnMouseScrolled.Broadcast(EMouseScrollDirection::Down);
			}
		});
		
		/* Maximize callback. */
		glfwSetWindowMaximizeCallback(GlfwWindow, [](GLFWwindow* Window, int Maximized)
		{
			FWindowData& WindowData = *static_cast<FWindowData*>(glfwGetWindowUserPointer(Window));
			if (Maximized == GLFW_TRUE)
			{
				WindowData.bMaximized = true;
			}
			else
			{
				WindowData.bMaximized = false;
			}

			WindowData.OnWindowMaximized.Broadcast(WindowData.bMaximized);
		});

		Data.bFullscreen = (glfwGetWindowMonitor(GlfwWindow) != nullptr);
		
		if (Specification.bStartMaximized)
		{
			Maximize();
		}

		Data.bMaximized = IsMaximized();

		/* Set the viewport width and height. */
		{
			int ReadWidth, ReadHeight;
			glfwGetFramebufferSize(GlfwWindow, &ReadWidth, &ReadHeight);
			ViewportSize.X = ReadWidth;
			ViewportSize.Y = ReadHeight;
		}

		bGlfwInitialized = true;
	}

	void LWindow::Destroy()
	{
		if (GlfwWindow)
		{
			glfwTerminate();
			GlfwWindow = nullptr;
		}
	}

	void LWindow::SwapBuffers()
	{
		glfwSwapBuffers(GlfwWindow);
		glfwPollEvents();
	}
	
	bool LWindow::IsFullscreen() const
	{
		LK_CORE_ASSERT(GlfwWindow);
		return (glfwGetWindowMonitor(GlfwWindow) != nullptr);
	}

	void LWindow::SetFullscreen(const bool Fullscreen)
	{
		LK_CORE_DEBUG_TAG("Window", "Setting fullscreen: {}", Fullscreen);
		GLFWmonitor* Monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* VideoMode = glfwGetVideoMode(Monitor);

		/* TODO: Rework this. */
		Data.bFullscreen = IsFullscreen();
		if (Data.bFullscreen == Fullscreen)
		{
			LK_CORE_INFO_TAG("Window", "The window is already in fullscreen");
			return;
		}

		if (Fullscreen)
		{
			/* Set the window to fullscreen. */
			glfwSetWindowMonitor(GlfwWindow, Monitor, 0, 0, VideoMode->width, VideoMode->height, VideoMode->refreshRate);
			Data.bFullscreen = true;
		}
		else
		{
			static int WindowPosX, WindowPosY;
			static int WindowedWidth, WindowedHeight;
			glfwGetWindowPos(GlfwWindow, &WindowPosX, &WindowPosY);
			glfwGetWindowSize(GlfwWindow, &WindowedWidth, &WindowedHeight);

			glfwSetWindowMonitor(GlfwWindow, Monitor, WindowPosX, WindowPosY, WindowedWidth, WindowedHeight, 0);
			Data.bFullscreen = false;
		}
	}

	void LWindow::SetTitle(const std::string& NewTitle)
	{
		LK_CORE_ASSERT(GlfwWindow);
		Data.Title = NewTitle;
		glfwSetWindowTitle(GlfwWindow, Data.Title.c_str());
	}

	void LWindow::Maximize()
	{
		LK_CORE_ASSERT(GlfwWindow);
		if (glfwGetWindowAttrib(GlfwWindow, GLFW_MAXIMIZED) == GLFW_FALSE)
		{
			glfwMaximizeWindow(GlfwWindow);
			Data.bMaximized = true;
		}
		else
		{
			glfwRestoreWindow(GlfwWindow);
			Data.bMaximized = false;
		}
	}

	bool LWindow::IsMaximized() const
	{
		LK_CORE_ASSERT(GlfwWindow);
		return (glfwGetWindowAttrib(GlfwWindow, GLFW_MAXIMIZED) == GLFW_TRUE);
	}

	void LWindow::Minimize()
	{
		LK_CORE_ASSERT(GlfwWindow);
		glfwIconifyWindow(GlfwWindow);
	}

	bool LWindow::IsMinimized() const
	{
		LK_CORE_ASSERT(GlfwWindow);
		return (glfwGetWindowAttrib(GlfwWindow, GLFW_MAXIMIZED) == GLFW_FALSE);
	}

	void LWindow::SetVSync(bool InEnabled)
	{
		Data.bVSync = InEnabled;
		if (Data.bVSync)
		{
			glfwSwapInterval(1);
		}
		else
		{
			glfwSwapInterval(0);
		}
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
