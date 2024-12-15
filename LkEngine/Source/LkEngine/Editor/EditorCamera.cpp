#include "LKpch.h"
#include "LkEngine/Editor/EditorCamera.h"

#include "LkEngine/Core/Application.h" /* REMOVE */
#include "LkEngine/UI/UILayer.h"

#include "LkEngine/Core/Window.h"

//FIXME

namespace LkEngine {

	LEditorCamera::LEditorCamera(const float InFovDeg, const float InWidth, const float InHeight, 
								 const float InNearP, const float InFarP)
	{
		m_PerspectiveNear = InNearP;
		m_PerspectiveFar = InFarP;

		static constexpr glm::vec3 BasePosition = { -5, 5, 5 }; 
		m_Position = BasePosition;

		m_Pitch = 0.0f;
		m_Yaw = glm::pi<float>();

		/* Calculate projection matrix. */
		m_ProjectionMatrix = glm::perspectiveFov(glm::radians(InFovDeg), InWidth, InHeight, InNearP, InFarP);
	}

	void LEditorCamera::Initialize()
	{
		m_Distance = glm::distance(m_Position, m_FocalPoint);

		m_Yaw = 3.0f * glm::pi<float>() / 4.0f;
		m_Pitch = glm::pi<float>() / 4.0f;

		m_Position = CalculatePosition();

		const glm::quat Orientation = GetOrientation();
		m_Direction = glm::eulerAngles(Orientation) * (180.0f / glm::pi<float>());
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(Orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);

		LK_CORE_TRACE_TAG("EditorCamera", "m_Position = ({}, {}, {})", m_Position.x, m_Position.y, m_Position.z);

		bIsActive = true;
	}

	void LEditorCamera::OnUpdate(const float DeltaTime)
	{
		const glm::vec2 Mouse{ LMouse::GetMouseX(), LMouse::GetMouseY() };
		const glm::vec2 MouseDelta = (Mouse - m_InitialMousePosition) * 0.002f;

		/* TODO: I don't like this approach of continously checking if active or not
		         and then handling UI input. This should get taken care of somewhere else.
		*/
	#if 0
		if (!bIsActive)
		{
			LK_CORE_DEBUG_TAG("EditorCamera", "Camera is not active, enabling UI input");
			if (!UI::IsInputEnabled())
			{
				UI::SetInputEnabled(true);
			}
			return;
		}
	#endif

		/* Translate Mode. */
		if (LKeyboard::IsKeyPressed(EKey::T))
		{
		}
		/* Rotation Mode. */
		else if (LKeyboard::IsKeyPressed(EKey::R))
		{
		}
		/* Scale Mode. */
		else if (LKeyboard::IsKeyPressed(EKey::S))
		{
		}

		if ((LInput::IsMouseButtonDown(EMouseButton::Button1)) && (!LInput::IsKeyDown(EKey::LeftAlt)))
		{
			CameraMode = EEditorCameraMode::Flycam;
			LMouse::Disable();

			const float YawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
			const float speed = GetCameraSpeed();

			if (LInput::IsKeyDown(EKey::Q))
			{
				m_PositionDelta -= DeltaTime * speed * glm::vec3{ 0.f, YawSign, 0.f };
			}
			if (LInput::IsKeyDown(EKey::E))
			{
				m_PositionDelta += DeltaTime * speed * glm::vec3{ 0.f, YawSign, 0.f };
			}
			if (LInput::IsKeyDown(EKey::S))
			{
				m_PositionDelta -= DeltaTime * speed * m_Direction;
			}
			if (LInput::IsKeyDown(EKey::W))
			{
				m_PositionDelta -= DeltaTime * speed * m_Direction;
			}
			if (LInput::IsKeyDown(EKey::A))
			{
				m_PositionDelta -= DeltaTime * speed * m_RightDirection;
			}
			if (LInput::IsKeyDown(EKey::D))
			{
				m_PositionDelta += DeltaTime * speed * m_RightDirection;
			}

			if (LKeyboard::IsKeyPressed(EKey::H))
			{
				ProjectionType = ECameraProjection::Orthographic;
				m_Position = { 2, 2, 0 };
			}
			if (LKeyboard::IsKeyPressed(EKey::P))
			{
				ProjectionType = ECameraProjection::Perspective;
				m_Position = { -330, -140, -910 };
				m_Yaw = -3.10f;
				m_Pitch = 6.40f;
			}

			static constexpr float MaxRate = 0.120f;
			m_YawDelta += glm::clamp(YawSign * MouseDelta.x * RotationSpeed(), -MaxRate, MaxRate);
			m_PitchDelta += glm::clamp(MouseDelta.y * RotationSpeed(), -MaxRate, MaxRate);

			m_RightDirection = glm::cross(m_Direction, glm::vec3{ 0.f, YawSign, 0.f });

			m_Direction = glm::rotate(glm::normalize(
				glm::cross(glm::angleAxis(-m_PitchDelta, m_RightDirection),
					glm::angleAxis(-m_YawDelta, glm::vec3{ 0.f, YawSign, 0.f })
				)
			), m_Direction);

			const float distance = glm::distance(m_FocalPoint, m_Position);
			m_FocalPoint = m_Position + GetForwardDirection() * distance;
			m_Distance = distance;

			LK_CORE_ASSERT(m_Distance < 10000, "Perspective   Distance is larger than 10k");
		}
		//-----------------------------------------------
		// Arcball  |  Modes: Pan/Rotate/Zoom 
		//-----------------------------------------------
		else if (LInput::IsKeyDown(EKey::LeftAlt))
		{
			CameraMode = EEditorCameraMode::Arcball;

			/* Camera Mode: Pan */
			if (LInput::IsMouseButtonDown(EMouseButton::Button2))
			{
				LMouse::Disable();
				MousePan(MouseDelta);
			}
			/* Camera Mode: Rotate */
			else if (LInput::IsMouseButtonDown(EMouseButton::Button0))
			{
				LMouse::Disable();
				MouseRotate(MouseDelta);
			}
			/* Camera Mode: Zoom */
			else if (LInput::IsMouseButtonDown(EMouseButton::Button1))
			{
				LMouse::Disable();
				MouseZoom((MouseDelta.x + MouseDelta.y) * 0.10f);
			}
			else
			{
				LMouse::Enable();
			}
		}
		else
		{
			LMouse::Enable();
		}

		m_InitialMousePosition = Mouse;
		m_Position += m_PositionDelta;
		m_Yaw += m_YawDelta;
		m_Pitch += m_PitchDelta;

		if (CameraMode == EEditorCameraMode::Arcball)
		{
			m_Position = CalculatePosition();
		}

		UpdateCameraView();
	}

	void LEditorCamera::SetViewportSize(const uint16_t InWidth, const uint16_t InHeight)
	{
		switch (ProjectionType)
		{
			case ECameraProjection::Perspective:
			{
				SetPerspectiveProjectionMatrix(glm::radians(m_DegPerspectiveFOV), 
											   static_cast<float>(InWidth),
											   static_cast<float>(InHeight), 
											   m_PerspectiveNear, 
											   m_PerspectiveFar);
				return;
			}

			case ECameraProjection::Orthographic:
			{
				SetOrthoProjectionMatrix(static_cast<float>(InWidth), 
										 static_cast<float>(InHeight), 
										 m_OrthographicNear, 
										 m_OrthographicFar);
				return;
			}

			default: break;
		}

		LK_CORE_ASSERT(false, "Invalid projection type");
	}

	void LEditorCamera::UpdateCameraView()
	{
		const float YawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
		// Handle problem with up direction same as normal
		const float cosAngle = glm::dot(GetForwardDirection(), GetUpDirection());
		if (cosAngle * YawSign > 0.99f)
			m_PitchDelta = 0.f;

		const glm::vec3 lookAt = m_Position + GetForwardDirection();
		m_Direction = glm::normalize(lookAt - m_Position);
		m_Distance = glm::distance(m_Position, m_FocalPoint);
		m_ViewMatrix = glm::lookAt(m_Position, lookAt, glm::vec3{ 0.f, YawSign, 0.f });

		m_YawDelta *= 0.6f;
		m_PitchDelta *= 0.6f;
		m_PositionDelta *= 0.8f;
	}

	void LEditorCamera::MousePan(const glm::vec2& Delta)
	{
		auto [xSpeed, ySpeed] = PanSpeed();
		m_FocalPoint -= GetRightDirection() * Delta.x * xSpeed * m_Distance;
		m_FocalPoint += GetUpDirection() * Delta.y * ySpeed * m_Distance;
	}

	void LEditorCamera::MouseRotate(const glm::vec2& Delta)
	{
		const float YawSign = GetUpDirection().y < 0.0f ? -1.0f : 1.0f;
		m_YawDelta += YawSign * Delta.x * RotationSpeed();
		m_PitchDelta += Delta.y * RotationSpeed();
	}

	void LEditorCamera::MouseZoom(const float Delta)
	{
		m_Distance -= Delta * ZoomSpeed();
		const glm::vec3 ForwardDir = GetForwardDirection();
		m_Position = m_FocalPoint - ForwardDir * m_Distance;

		if (m_Distance < 1.0f)
		{
			m_FocalPoint += ForwardDir * m_Distance;
			m_Distance = 1.0f;
		}

		m_PositionDelta += Delta * ZoomSpeed() * ForwardDir;
	}

	float LEditorCamera::ZoomSpeed() const
	{
		float distance = m_Distance * 0.2f;
		distance = glm::max(distance, 0.0f);
		float speed = distance * distance;
		speed = glm::min(speed, 120.0f);
		return speed;
	}

	std::pair<float, float> LEditorCamera::PanSpeed() const
	{
		const float x = glm::min(float(m_ViewportWidth) / 1000.0f, 2.4f); // max = 2.4f
		const float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		const float y = glm::min(float(m_ViewportHeight) / 1000.0f, 2.4f); // max = 2.4f
		const float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}

	bool LEditorCamera::OnMouseScroll(MouseScrolledEvent& e)
	{
		// Right Mouse Button.
		if (LInput::IsMouseButtonDown(EMouseButton::Button1))
		{
			m_NormalSpeed += e.GetYOffset() * 0.3f * m_NormalSpeed;
			m_NormalSpeed = std::clamp(m_NormalSpeed, MIN_SPEED, MAX_SPEED);
		}
		else
		{
			MouseZoom(e.GetYOffset() * 0.1f);
			UpdateCameraView();
		}
		return true;
	}

	bool LEditorCamera::OnKeyPress(KeyPressedEvent& Event)
	{
		const glm::vec2 MousePosition = { LMouse::GetMouseX(), LMouse::GetMouseY() };
		const glm::vec2 MouseDelta = (MousePosition - m_InitialMousePosition) * 0.002f;

		const float YawSign = (GetUpDirection().y >= 0) ? 1.0f : -1.0f;
		const float speed = GetCameraSpeed();
		const float ts = 10;

		const EKey PressedKey = Event.GetKey();
		if (PressedKey == EKey::Q)
		{
			m_PositionDelta -= ts * speed * glm::vec3{ 0.f, YawSign, 0.f };
		}
		if (PressedKey == EKey::E)
		{
			m_PositionDelta += ts * speed * glm::vec3{ 0.f, YawSign, 0.f };
		}
		if (PressedKey == EKey::W)
		{
			m_PositionDelta += ts * speed * m_Direction;
		}
		if (PressedKey == EKey::A)
		{
			m_PositionDelta -= ts * speed * m_RightDirection;
		}
		if (PressedKey == EKey::S)
		{
			//m_PositionDelta -= ts * speed * m_Direction;
		}
		if (PressedKey == EKey::D)
		{
			m_PositionDelta += ts * speed * m_RightDirection;
		}

		if (PressedKey == EKey::T)
		{
			GizmoMode = EGizmo::Translate;
		}
		if (PressedKey == EKey::R)
		{
			GizmoMode = EGizmo::Rotate;
		}
		if (PressedKey == EKey::S)
		{
			GizmoMode = EGizmo::Scale;
		}

		return true;
	}

	void LEditorCamera::OnEvent(LEvent& Event)
	{
		EventDispatcher Dispatcher(Event);

		Dispatcher.Dispatch<KeyPressedEvent>([this](KeyPressedEvent& Event) 
		{ 
			return OnKeyPress(Event); 
		});

		Dispatcher.Dispatch<MouseScrolledEvent>([this](MouseScrolledEvent& Event)
		{ 
			return OnMouseScroll(Event); 
		});
	}

	float LEditorCamera::GetCameraSpeed() const
	{
		float Speed = m_NormalSpeed;
		if (LInput::IsKeyDown(EKey::LeftControl))
		{
			Speed /= (2.0f - glm::log(m_NormalSpeed));
		}

		if (LInput::IsKeyDown(EKey::LeftShift))
		{
			Speed *= (2.0f - glm::log(m_NormalSpeed));
		}

		return glm::clamp(Speed, MIN_SPEED, MAX_SPEED);
	}

}
