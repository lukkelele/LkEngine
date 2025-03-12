#pragma once

#include "LkEngine/Core/Core.h"
#include "LkEngine/Core/Time/Timer.h"
#include "LkEngine/Core/Event/KeyEvent.h"
#include "LkEngine/Core/Event/MouseEvent.h"
#include "LkEngine/Core/Time/Timestep.h"
#include "LkEngine/Core/Input/Keyboard.h"
#include "LkEngine/Core/Input/Mouse.h"

#include "LkEngine/Editor/EditorCore.h"

#include "LkEngine/Renderer/Camera.h"

#include "LkEngine/Scene/SceneCamera.h"


namespace LkEngine {

	enum class EEditorCameraMode
	{
		None = 0,
		Flycam,
		Arcball
	};
	LK_ENUM_CLASS(EEditorCameraMode);

	/**
	 * LEditorCamera
	 */
	class LEditorCamera : public LSceneCamera
	{
	public:
		LEditorCamera(const float DegFov, const float Width, const float Height,
					  const float NearP, const float FarP); 
		LEditorCamera() = delete;
		~LEditorCamera() = default;

		LEditorCamera& operator=(const LSceneCamera& Other)
		{
			Origin = Other.GetOrigin();
			FocalPoint = Other.GetFocalPoint();
			Direction = Other.GetDirection();

			Pitch = Other.GetPitch();
			Yaw = Other.GetYaw();

			Distance = 8.0f;
			NormalSpeed = 0.0020f;
			TravelSpeed = 1.0f;

			return *this;
		}

		void Initialize();
		void Tick(const float DeltaTime = 0.0f);
		void UpdateCameraView();

		virtual ECameraType GetType() const override { return ECameraType::Editor; }

		FORCEINLINE void SetPerspective(const float InVerticalFovDeg, const float InNearClip = 0.1f, const float InFarClip = 1000.0f)
		{
			if ((ProjectionType != ECameraProjection::Perspective)
				|| (DegPerspectiveFov != InVerticalFovDeg)
				|| (PerspectiveNear != InNearClip)
				|| (PerspectiveFar != InFarClip))
			{
				ProjectionType = ECameraProjection::Perspective;
				DegPerspectiveFov = InVerticalFovDeg;
				PerspectiveNear = InNearClip;
				PerspectiveFar = InFarClip;

				OnCameraProjectionChanged.Broadcast(ECameraProjection::Perspective);
			}
		}

		FORCEINLINE void SetOrthographic(const float InWidth, const float InHeight, const float InNearClip = -1.0f, const float InFarClip = 1.0f)
		{
			if ((ProjectionType != ECameraProjection::Perspective)
				|| (OrthographicNear != InNearClip)
				|| (OrthographicFar != InFarClip))
			{
				ProjectionType = ECameraProjection::Orthographic;
				OrthographicNear = InNearClip;
				OrthographicFar = InFarClip;

				OnCameraProjectionChanged.Broadcast(ECameraProjection::Orthographic);
			}
		}

		FORCEINLINE void SetPosition(const glm::vec3& InPosition)
		{
			Position = InPosition;
			UpdateCameraView();
		}

		void SetViewportSize(const uint16_t InWidth, const uint16_t InHeight);

		void MousePan(const glm::vec2& Delta);
		void MouseRotate(const glm::vec2& Delta);
		void MouseZoom(const float Delta);

		float GetCameraSpeed() const;
		std::pair<float, float> GetPanSpeed() const;
		float GetZoomSpeed() const;

		void SetPitchLocked(const bool Locked);
		bool IsPitchLocked() const;
		void SetYawLocked(const bool Locked);
		bool IsYawLocked() const;

		FORCEINLINE float GetDistance() const { return Distance; }
		FORCEINLINE float GetNormalSpeed() const { return NormalSpeed; }
		FORCEINLINE float GetTravelSpeed() const { return TravelSpeed; }
		FORCEINLINE void SetDistance(const float InDistance) { Distance = InDistance; }
		FORCEINLINE void SetNormalSpeed(const float InSpeed) { NormalSpeed = InSpeed; }
		FORCEINLINE void SetTravelSpeed(const float InSpeed) { TravelSpeed = InSpeed; }

		FORCEINLINE glm::vec3 CalculatePosition() const
		{
			return (FocalPoint - (GetForwardDirection() * Distance + PositionDelta));
		}

	private:
		bool OnKeyPressed(LKeyPressedEvent& Event);
		bool OnMouseScrolled(LMouseScrolledEvent& Event);

	private:
		static constexpr float MIN_SPEED = 0.00050f;
		static constexpr float MAX_SPEED = 2.0f;
	private:
		EEditorCameraMode CameraMode = EEditorCameraMode::Arcball;
		uint16_t CameraActionFlags = (uint16_t)ECameraAction::None;

		float Distance = 8.0f;
		float NormalSpeed = 0.0020f;
		float TravelSpeed = 1.0f;

		glm::vec3 Position{};
		glm::vec2 InitialMousePosition{};
		glm::vec3 InitialFocalPoint{};
		glm::vec3 InitialRotation{};

		uint32_t ViewportWidth = 1280;
		uint32_t ViewportHeight = 720;

		friend class LEditorLayer;
		friend class LSceneManagerPanel;
		friend class LSceneSerializer;

		LCLASS(LEditorCamera);
	};


	namespace Enum
	{
		FORCEINLINE static constexpr const char* ToString(const EEditorCameraMode CameraMode)
		{
			switch (CameraMode)
			{
				case EEditorCameraMode::None:    return "None";
				case EEditorCameraMode::Flycam:  return "Flycam";
				case EEditorCameraMode::Arcball: return "Arcball";
			}

			LK_CORE_VERIFY(false, "Enum::ToString(const EEditorCameraMode) failed for: {}", static_cast<int>(CameraMode));
			return nullptr;
		}
	}

}
