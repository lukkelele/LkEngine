#include "LKpch.h"
#include "Window.h"

#include "LkEngine/Core/Application.h"
#include "LkEngine/Core/Viewport.h"
#include "LkEngine/Input/Keyboard.h"
#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

	LWindow::LWindow(const FWindowSpecification& WindowSpecification)
		: Specification(WindowSpecification)
		, Title(WindowSpecification.Title)
		, Size({ WindowSpecification.Width, WindowSpecification.Height })
		, ViewportSize({ WindowSpecification.Width, WindowSpecification.Height })
		, bVSync(WindowSpecification.VSync)
	{
		LOBJECT_REGISTER();
		Instance = this;

		/* Window Data. */
		Data.Title = Title;
		Data.Width = static_cast<uint32_t>(Size.X);
		Data.Height = static_cast<uint32_t>(Size.Y);
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
		GlfwWindow = glfwCreateWindow(static_cast<int>(Size.X), static_cast<int>(Size.Y), Title.c_str(), nullptr, nullptr);
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
		glfwSetWindowSizeLimits(GlfwWindow, 340, 200, 5000, 5000);

		glfwSetWindowSizeCallback(GlfwWindow, [](GLFWwindow* InGlfwWindow, int NewWidth, int NewHeight) 
		{
			int SizeX, SizeY;
			glfwGetWindowSize(InGlfwWindow, &SizeX, &SizeY);

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
					KeyData.RepeatCount++;
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
		bVSync = InEnabled;
		if (bVSync)
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
