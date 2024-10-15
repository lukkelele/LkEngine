#include "LKpch.h"
#include "LkEngine/Editor/EditorCamera.h"

#include "LkEngine/Core/Application.h"

#include "LkEngine/Core/Window.h"

//FIXME

namespace LkEngine {

	LEditorCamera::LEditorCamera(const float degFov, 
								 const float width, 
								 const float height, 
								 const float nearP, 
								 const float farP)
		: LCamera(glm::perspectiveFov(glm::radians(degFov), width, height, farP, nearP))
	{
		m_PerspectiveNear = nearP;
		m_PerspectiveFar = farP;
		constexpr glm::vec3 position = { -5, 5, 5 }; 
		m_Position = position;
		Init();
	}

	void LEditorCamera::Init()
	{
		m_Distance = glm::distance(m_Position, m_FocalPoint);

		m_Yaw = 3.0f * glm::pi<float>() / 4.0f;
		m_Pitch = glm::pi<float>() / 4.0f;

		m_Position = CalculatePosition();
		const glm::quat orientation = GetOrientation();
		m_Direction = glm::eulerAngles(orientation) * (180.0f / glm::pi<float>());
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);

		LK_CORE_TRACE_TAG("EditorCamera", "m_Position = ({}, {}, {})", m_Position.x, m_Position.y, m_Position.z);
	}

	void LEditorCamera::OnUpdate(const FTimestep DeltaTime)
	{
		const glm::vec2& mouse{ Mouse::GetMouseX(), Mouse::GetMouseY() };
		const glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.002f;

		if (m_IsActive == false)
		{
			LK_CORE_DEBUG_TAG("EditorCamera", "Camera is not active, enabling UI input");
			if (UI::IsInputEnabled() == false)
				UI::SetInputEnabled(true);
			return;
		}

		// Translate Mode
		if (Keyboard::IsKeyPressed(Key::T))
		{
			LK_CORE_DEBUG_TAG("EditorCamera", "TRANSLATE MODE");
		}
		// Rotation Mode
		if (Keyboard::IsKeyPressed(Key::R))
		{
			LK_CORE_DEBUG_TAG("EditorCamera", "ROTATION MODE");
		}
		// Scale Mode
		if (Keyboard::IsKeyPressed(Key::S))
		{
			LK_CORE_DEBUG_TAG("EditorCamera", "SCALE MODE");
		}

		if ((Input::IsMouseButtonDown(EMouseButton::Button1)) && (!Input::IsKeyDown(KeyCode::LeftAlt)))
		{
			m_CameraMode = Mode::Flycam;
			Mouse::Disable();

			const float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
			const float speed = GetCameraSpeed();

			if (Input::IsKeyDown(KeyCode::Q))
			{
				m_PositionDelta -= DeltaTime * speed * glm::vec3{ 0.f, yawSign, 0.f };
			}
			if (Input::IsKeyDown(KeyCode::E))
			{
				m_PositionDelta += DeltaTime * speed * glm::vec3{ 0.f, yawSign, 0.f };
			}
			if (Input::IsKeyDown(KeyCode::S))
			{
				m_PositionDelta -= DeltaTime * speed * m_Direction;
			}
			if (Input::IsKeyDown(KeyCode::W))
			{
				m_PositionDelta -= DeltaTime * speed * m_Direction;
			}
			if (Input::IsKeyDown(KeyCode::A))
			{
				m_PositionDelta -= DeltaTime * speed * m_RightDirection;
			}
			if (Input::IsKeyDown(KeyCode::D))
			{
				m_PositionDelta += DeltaTime * speed * m_RightDirection;
			}

			if (Keyboard::IsKeyPressed(Key::H))
			{
				m_ProjectionType = ProjectionType::Orthographic;
				m_Position = { 2, 2, 0 };
			}
			if (Keyboard::IsKeyPressed(Key::P))
			{
				m_ProjectionType = ProjectionType::Perspective;
				m_Position = { -330, -140, -910 };
				m_Yaw = -3.10f;
				m_Pitch = 6.40f;
			}

			constexpr float maxRate{ 0.12f };
			m_YawDelta += glm::clamp(yawSign * delta.x * RotationSpeed(), -maxRate, maxRate);
			m_PitchDelta += glm::clamp(delta.y * RotationSpeed(), -maxRate, maxRate);

			m_RightDirection = glm::cross(m_Direction, glm::vec3{ 0.f, yawSign, 0.f });

			m_Direction = glm::rotate(glm::normalize(
				glm::cross(glm::angleAxis(-m_PitchDelta, m_RightDirection),
					glm::angleAxis(-m_YawDelta, glm::vec3{ 0.f, yawSign, 0.f }))), 
					m_Direction);

			const float distance = glm::distance(m_FocalPoint, m_Position);
			m_FocalPoint = m_Position + GetForwardDirection() * distance;
			m_Distance = distance;

			LK_CORE_ASSERT(m_Distance < 10000, "Perspective   Distance is larger than 10k");
		}
		//-----------------------------------------------
		// Arcball  |  Modes: Pan/Rotate/Zoom 
		//-----------------------------------------------
		else if (Input::IsKeyDown(KeyCode::LeftAlt))
		{
			m_CameraMode = Mode::Arcball;

			// Camera Mode: Pan
			// Middle Mouse Button.
			if (Input::IsMouseButtonDown(EMouseButton::Button2))
			{
				Mouse::Disable();
				MousePan(delta);
			}
			// Camera Mode: Rotate
			// Left Mouse Button.
			else if (Input::IsMouseButtonDown(EMouseButton::Button0))
			{
				Mouse::Disable();
				MouseRotate(delta);
			}
			// Camera Mode: Zoom
			// Right Mouse Button.
			else if (Input::IsMouseButtonDown(EMouseButton::Button1))
			{
				Mouse::Disable();
				MouseZoom((delta.x + delta.y) * 0.1f);
			}
			else
			{
				Mouse::Enable();
			}
		}
		else
		{
			Mouse::Enable();
		}

		m_InitialMousePosition = mouse;
		m_Position += m_PositionDelta;
		m_Yaw += m_YawDelta;
		m_Pitch += m_PitchDelta;

		if (m_CameraMode == Mode::Arcball)
			m_Position = CalculatePosition();

		UpdateCameraView();
	}

	void LEditorCamera::SetPerspective(float degVerticalFOV, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Perspective;
		m_DegPerspectiveFOV = degVerticalFOV;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;
	}

	void LEditorCamera::SetOrthographic(float width, float height, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;
	}

	void LEditorCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		switch (m_ProjectionType)
		{
		case ProjectionType::Perspective:
			SetPerspectiveProjectionMatrix(glm::radians(m_DegPerspectiveFOV), 
										   static_cast<float>(width),
										   static_cast<float>(height), 
										   m_PerspectiveNear, 
										   m_PerspectiveFar);
			break;

		case ProjectionType::Orthographic:
			SetOrthoProjectionMatrix(static_cast<float>(width), 
									 static_cast<float>(height), 
									 m_OrthographicNear, 
									 m_OrthographicFar);
			break;
		}
	}

	void LEditorCamera::UpdateCameraView()
	{
		const float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
		// Handle problem with up direction same as normal
		const float cosAngle = glm::dot(GetForwardDirection(), GetUpDirection());
		if (cosAngle * yawSign > 0.99f)
			m_PitchDelta = 0.f;

		const glm::vec3 lookAt = m_Position + GetForwardDirection();
		m_Direction = glm::normalize(lookAt - m_Position);
		m_Distance = glm::distance(m_Position, m_FocalPoint);
		m_ViewMatrix = glm::lookAt(m_Position, lookAt, glm::vec3{ 0.f, yawSign, 0.f });

		m_YawDelta *= 0.6f;
		m_PitchDelta *= 0.6f;
		m_PositionDelta *= 0.8f;
	}

	void LEditorCamera::MousePan(const glm::vec2& delta)
	{
		auto [xSpeed, ySpeed] = PanSpeed();
		m_FocalPoint -= GetRightDirection() * delta.x * xSpeed * m_Distance;
		m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
	}

	void LEditorCamera::MouseRotate(const glm::vec2& delta)
	{
		const float yawSign = GetUpDirection().y < 0.0f ? -1.0f : 1.0f;
		m_YawDelta += yawSign * delta.x * RotationSpeed();
		m_PitchDelta += delta.y * RotationSpeed();
	}

	void LEditorCamera::MouseZoom(float delta)
	{
		m_Distance -= delta * ZoomSpeed();
		const glm::vec3 forwardDir = GetForwardDirection();
		m_Position = m_FocalPoint - forwardDir * m_Distance;
		if (m_Distance < 1.0f)
		{
			m_FocalPoint += forwardDir * m_Distance;
			m_Distance = 1.0f;
		}
		m_PositionDelta += delta * ZoomSpeed() * forwardDir;
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
		if (Input::IsMouseButtonDown(EMouseButton::Button1))
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

	bool LEditorCamera::OnKeyPress(KeyPressedEvent& e)
	{
		const glm::vec2& mouse{ Mouse::GetMouseX(), Mouse::GetMouseY() };
		const glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.002f;

		const float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
		const float speed = GetCameraSpeed();
		const float ts = 10;

		Key pressedKey = e.GetKeyCode();
		if (pressedKey == Key::Q)
		{
			m_PositionDelta -= ts * speed * glm::vec3{ 0.f, yawSign, 0.f };
		}
		if (pressedKey == Key::E)
		{
			m_PositionDelta += ts * speed * glm::vec3{ 0.f, yawSign, 0.f };
		}
		if (pressedKey == KeyCode::W)
		{
			m_PositionDelta += ts * speed * m_Direction;
		}
		if (pressedKey == Key::A)
		{
			m_PositionDelta -= ts * speed * m_RightDirection;
		}
		if (pressedKey == Key::S)
		{
			//m_PositionDelta -= ts * speed * m_Direction;
		}
		if (pressedKey == Key::D)
		{
			m_PositionDelta += ts * speed * m_RightDirection;
		}

		if (pressedKey == Key::T)
		{
			m_GizmoMode = GizmoMode::Translate;
		}
		if (pressedKey == Key::R)
		{
			m_GizmoMode = GizmoMode::Rotate;
		}
		if (pressedKey == Key::S)
		{
			m_GizmoMode = GizmoMode::Scale;
		}

		return true;
	}

	void LEditorCamera::OnEvent(LEvent& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>([this](KeyPressedEvent& e) { return OnKeyPress(e); });

		dispatcher.Dispatch<MouseScrolledEvent>([this](MouseScrolledEvent& Event)
		{ 
			return OnMouseScroll(Event); 
		});
	}

	void LEditorCamera::SetPosition(const glm::vec3& position)
	{
		m_Position = position;
		//m_Position = glm::vec3(0.0f);
		//m_PositionDelta = position;
		UpdateCameraView();
	}

	glm::vec3 LEditorCamera::GetUpDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 LEditorCamera::GetRightDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.f, 0.f, 0.f));
	}

#if 0
	glm::vec3 LEditorCamera::GetForwardDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}
#endif

	glm::vec3 LEditorCamera::CalculatePosition() const
	{
		return m_FocalPoint - GetForwardDirection() * m_Distance + m_PositionDelta;
	}

	glm::quat LEditorCamera::GetOrientation() const
	{
		return glm::quat(glm::vec3(-m_Pitch - m_PitchDelta, -m_Yaw - m_YawDelta, 0.0f));
	}

	float LEditorCamera::RotationSpeed() const
	{
		return m_RotationSpeed;
	}

	float LEditorCamera::GetCameraSpeed() const
	{
		float speed = m_NormalSpeed;
		if (Input::IsKeyDown(Key::LeftControl))
			speed /= 2 - glm::log(m_NormalSpeed);
		if (Input::IsKeyDown(Key::LeftShift))
			speed *= 2 - glm::log(m_NormalSpeed);
		return glm::clamp(speed, MIN_SPEED, MAX_SPEED);
	}

	unsigned int LEditorCamera::GetGizmoMode() const
	{
		return (unsigned int)m_GizmoMode;
	}

	void LEditorCamera::ApplyViewMode(ViewMode mode)
	{
		/// REWORK THIS....
		switch (mode)
		{
			case ViewMode::Classic:
			{
				return;
			}

			// Fake2D is a projection type of 2D
			case ViewMode::Fake2D: 
				m_Pitch = 0.0f;
				m_Yaw = glm::pi<float>();
				return;
		}
		LK_CORE_ASSERT(false, "Camera::ViewMode unknown!");
	}

}
