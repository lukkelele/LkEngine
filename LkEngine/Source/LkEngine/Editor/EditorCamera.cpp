#include "LKpch.h"
#include "EditorCamera.h"

#include "LkEngine/Core/Window.h"
#include "LkEngine/Core/Input/Input.h"

#include "LkEngine/Renderer/UI/UILayer.h"


namespace LkEngine {

	LEditorCamera::LEditorCamera(const float InFovDeg, const float InWidth, const float InHeight, 
								 const float InNearP, const float InFarP)
	{
		PerspectiveNear = InNearP;
		PerspectiveFar = InFarP;

		static constexpr glm::vec3 BasePosition = { -5.0f, 5.0f, 5.0f }; 
		Position = BasePosition;

		Pitch = 0.0f;
		Yaw = glm::pi<float>();

		ProjectionMatrix = glm::perspectiveFov(glm::radians(InFovDeg), InWidth, InHeight, InNearP, InFarP);
	}

	void LEditorCamera::Initialize()
	{
		LK_CORE_DEBUG_TAG("EditorCamera", "Initializing");
		Distance = glm::distance(Position, FocalPoint);

		Yaw = 3.0f * glm::pi<float>() / 4.0f;
		Pitch = glm::pi<float>() / 4.0f;

		Position = CalculatePosition();

		const glm::quat Orientation = GetOrientation();
		Direction = glm::eulerAngles(Orientation) * (180.0f / glm::pi<float>());
		ViewMatrix = glm::translate(glm::mat4(1.0f), Position) * glm::toMat4(Orientation);
		ViewMatrix = glm::inverse(ViewMatrix);
	}

	void LEditorCamera::Tick(const float DeltaTime)
	{
		if (!bIsActive)
		{
			if (!UI::IsInputEnabled())
			{
				UI::SetInputEnabled(true);
			}

			return;
		}

		const glm::vec2 MousePos(LMouse::GetMouseX(), LMouse::GetMouseY());
		const glm::vec2 MouseDelta = (MousePos - InitialMousePosition) * 0.0020f;

		glm::vec3 RightDirection = GetRightDirection();

		if (LInput::IsMouseButtonDown(EMouseButton::Right) && !LInput::IsKeyDown(EKey::LeftAlt))
		{
			CameraMode = EEditorCameraMode::Flycam;
			LMouse::Disable();

			const float YawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
			const float CameraSpeed = GetCameraSpeed();

			if (LInput::IsKeyDown(EKey::Q))
			{
				PositionDelta -= DeltaTime * CameraSpeed * glm::vec3{ 0.0f, YawSign, 0.0f };
			}
			if (LInput::IsKeyDown(EKey::E))
			{
				PositionDelta += DeltaTime * CameraSpeed * glm::vec3{ 0.0f, YawSign, 0.0f };
			}
			if (LInput::IsKeyDown(EKey::S))
			{
				PositionDelta -= DeltaTime * CameraSpeed * Direction;
			}
			if (LInput::IsKeyDown(EKey::W))
			{
				PositionDelta -= DeltaTime * CameraSpeed * Direction;
			}
			if (LInput::IsKeyDown(EKey::A))
			{
				PositionDelta -= DeltaTime * CameraSpeed * RightDirection;
			}
			if (LInput::IsKeyDown(EKey::D))
			{
				PositionDelta += DeltaTime * CameraSpeed * RightDirection;
			}

			static constexpr float MaxRate = 0.120f;
			YawDelta += glm::clamp(YawSign * MouseDelta.x * GetRotationSpeed(), -MaxRate, MaxRate);
			PitchDelta += glm::clamp(MouseDelta.y * GetRotationSpeed(), -MaxRate, MaxRate);

			RightDirection = glm::cross(Direction, glm::vec3(0.0f, YawSign, 0.0f));

			Direction = glm::rotate(
				glm::normalize( 
					glm::cross(glm::angleAxis(-PitchDelta, RightDirection),
					glm::angleAxis(-YawDelta, glm::vec3(0.0f, YawSign, 0.0f))
				)
			), Direction);

			const float ScalarDistance = glm::distance(FocalPoint, Position);
			FocalPoint = Position + GetForwardDirection() * ScalarDistance;
			Distance = ScalarDistance;

			LK_CORE_ASSERT(Distance < 10000, "Distance is larger than 10k");
		}
		//-----------------------------------------------
		// Arcball  |  Modes: Pan/Rotate/Zoom 
		//-----------------------------------------------
		else if (LInput::IsKeyDown(EKey::LeftAlt))
		{
			CameraMode = EEditorCameraMode::Arcball;

			/* Camera Mode: Pan */
			if (LInput::IsMouseButtonDown(EMouseButton::Middle))
			{
				LMouse::Disable();
				CameraActionFlags = (uint16_t)ECameraAction::Pan;
				MousePan(MouseDelta);
			}
			/* Camera Mode: Rotate */
			else if (LInput::IsMouseButtonDown(EMouseButton::Left))
			{
				LMouse::Disable();
				CameraActionFlags = (uint16_t)ECameraAction::Rotate;
				MouseRotate(MouseDelta);
			}
			/* Camera Mode: Zoom */
			else if (LInput::IsMouseButtonDown(EMouseButton::Right))
			{
				LMouse::Disable();
				CameraActionFlags = (uint16_t)ECameraAction::Zoom;
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
			CameraActionFlags = (uint16_t)ECameraAction::None;
		}

		/**
		 * TODO: Cannot use IsKeyPressed here since the input callbacks from LWindow take
		 *       priority over the rest of the application when running since the delegates
		 *       invoke directly and do not queue any events.
		 */
		/* Toggle projection type. */
		//if (LInput::IsKeyPressed(EKey::H) && !LInput::IsKeyHeld(EKey::H))
		if (LInput::IsKeyDown(EKey::H) && !LInput::IsKeyHeld(EKey::H))
		{
			if (ProjectionType == ECameraProjection::Perspective)
			{
				ProjectionType = ECameraProjection::Orthographic;
			}
			else
			{
				ProjectionType = ECameraProjection::Perspective;
			}
			LK_CORE_INFO_TAG("EditorCamera", "Projection type: {}", Enum::ToString(ProjectionType));
		}

		InitialMousePosition = MousePos;
		Position += PositionDelta;
		Yaw += YawDelta;
		Pitch += PitchDelta;

		if (CameraMode == EEditorCameraMode::Arcball)
		{
			Position = CalculatePosition();
		}

		UpdateCameraView();
	}

	void LEditorCamera::SetViewportSize(const uint16_t InWidth, const uint16_t InHeight)
	{
		switch (ProjectionType)
		{
			case ECameraProjection::Perspective:
			{
				SetPerspectiveProjectionMatrix(glm::radians(DegPerspectiveFov), 
											   static_cast<float>(InWidth),
											   static_cast<float>(InHeight), 
											   PerspectiveNear, 
											   PerspectiveFar);
				return;
			}

			case ECameraProjection::Orthographic:
			{
				SetOrthoProjectionMatrix(static_cast<float>(InWidth), 
										 static_cast<float>(InHeight), 
										 OrthographicNear, 
										 OrthographicFar);
				return;
			}
		}

		LK_CORE_VERIFY(false);
	}

	void LEditorCamera::UpdateCameraView()
	{
		const float YawSign = ((GetUpDirection().y < 0) ? -1.0f : 1.0f);

		/* In case direction is the same as the normal. */
		const float CosAngle = glm::dot(GetForwardDirection(), GetUpDirection());
		if (CosAngle * YawSign > 0.99f)
		{
			PitchDelta = 0.0f;
		}

		const glm::vec3 LookAt = Position + GetForwardDirection();
		Direction = glm::normalize(LookAt - Position);
		Distance = glm::distance(Position, FocalPoint);
		ViewMatrix = glm::lookAt(Position, LookAt, glm::vec3(0.0f, YawSign, 0.0f));

		YawDelta *= 0.60f;
		PitchDelta *= 0.60f;
		PositionDelta *= 0.80f;
	}

	void LEditorCamera::MousePan(const glm::vec2& Delta)
	{
		auto [xSpeed, ySpeed] = GetPanSpeed();
		FocalPoint -= GetRightDirection() * Delta.x * xSpeed * Distance;
		FocalPoint += GetUpDirection() * Delta.y * ySpeed * Distance;
	}

	void LEditorCamera::MouseRotate(const glm::vec2& Delta)
	{
		const float YawSign = GetUpDirection().y < 0.0f ? -1.0f : 1.0f;
		YawDelta += YawSign * Delta.x * GetRotationSpeed();
		PitchDelta += Delta.y * GetRotationSpeed();
	}

	void LEditorCamera::MouseZoom(const float Delta)
	{
		Distance -= Delta * GetZoomSpeed();
		const glm::vec3 ForwardDir = GetForwardDirection();
		Position = FocalPoint - ForwardDir * Distance;

		if (Distance < 1.0f)
		{
			FocalPoint += ForwardDir * Distance;
			Distance = 1.0f;
		}

		PositionDelta += Delta * GetZoomSpeed() * ForwardDir;
	}

	float LEditorCamera::GetZoomSpeed() const
	{
		float ZoomDistance = Distance * 0.20f;
		ZoomDistance = glm::max(ZoomDistance, 0.0f);

		float ZoomSpeed = ZoomDistance * ZoomDistance;
		ZoomSpeed = glm::min(ZoomSpeed, 120.0f);

		return ZoomSpeed;
	}

	void LEditorCamera::SetPitchLocked(const bool Locked)
	{
		if (Locked)
		{
			ModifierFlags |= ECameraModifier::PitchLocked;
		}
		else
		{
			ModifierFlags &= ~ECameraModifier::PitchLocked;
		}
	}

	bool LEditorCamera::IsPitchLocked() const
	{
		return ModifierFlags & ECameraModifier::PitchLocked;
	}

	void LEditorCamera::SetYawLocked(const bool Locked)
	{
		if (Locked)
		{
			ModifierFlags |= ECameraModifier::YawLocked;
		}
		else
		{
			ModifierFlags &= ~ECameraModifier::YawLocked;
		}
	}

	bool LEditorCamera::IsYawLocked() const
	{
		return ModifierFlags & ECameraModifier::YawLocked;
	}

	std::pair<float, float> LEditorCamera::GetPanSpeed() const
	{
		/* Maximum: 2.40f */
		const float x = glm::min(float(ViewportWidth) / 1000.0f, 2.40f);
		const float FactorX = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		/* Maximum: 2.40f */
		const float y = glm::min(float(ViewportHeight) / 1000.0f, 2.40f);
		const float FactorY = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { FactorX, FactorY };
	}

	bool LEditorCamera::OnMouseScrolled(LMouseScrolledEvent& Event)
	{
		if (LInput::IsMouseButtonDown(EMouseButton::Right))
		{
			NormalSpeed += Event.GetYOffset() * 0.30f * NormalSpeed;
			NormalSpeed = std::clamp(NormalSpeed, MIN_SPEED, MAX_SPEED);
		}
		else
		{
			MouseZoom(Event.GetYOffset() * 0.10f);
			UpdateCameraView();
		}

		return true;
	}

	bool LEditorCamera::OnKeyPressed(LKeyPressedEvent& Event)
	{
		const glm::vec2 MousePosition = { LMouse::GetMouseX(), LMouse::GetMouseY() };
		const glm::vec2 MouseDelta = (MousePosition - InitialMousePosition) * 0.0020f;
		const glm::vec3 RightDirection = GetRightDirection();

		const float YawSign = (GetUpDirection().y >= 0) ? 1.0f : -1.0f;
		const float CameraSpeed = GetCameraSpeed();
		const float TimeStep = 10.0f;

		const EKey PressedKey = Event.GetKey();
		if (PressedKey == EKey::Q)
		{
			PositionDelta -= TimeStep * CameraSpeed * glm::vec3(0.0f, YawSign, 0.0f);
		}
		if (PressedKey == EKey::E)
		{
			PositionDelta += TimeStep * CameraSpeed * glm::vec3(0.0f, YawSign, 0.0f);
		}
		if (PressedKey == EKey::W)
		{
			PositionDelta += TimeStep * CameraSpeed * Direction;
		}
		if (PressedKey == EKey::A)
		{
			PositionDelta -= TimeStep * CameraSpeed * RightDirection;
		}
		if (PressedKey == EKey::S)
		{
			//PositionDelta -= TimeStep * CameraSpeed * Direction;
		}
		if (PressedKey == EKey::D)
		{
			PositionDelta += TimeStep * CameraSpeed * RightDirection;
		}

		return true;
	}

	float LEditorCamera::GetCameraSpeed() const
	{
		static constexpr float BaseMultiplier = 2.0f;

		float Speed = NormalSpeed;
		if (LInput::IsKeyDown(EKey::LeftControl))
		{
			Speed /= (BaseMultiplier - glm::log(NormalSpeed));
		}

		if (LInput::IsKeyDown(EKey::LeftShift))
		{
			Speed *= (BaseMultiplier - glm::log(NormalSpeed));
		}

		return glm::clamp(Speed, MIN_SPEED, MAX_SPEED);
	}

}
