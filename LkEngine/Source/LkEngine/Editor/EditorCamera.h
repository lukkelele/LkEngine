#pragma once

#include "LkEngine/Core/Core.h" /* PATCH OUT */
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

	/**
	 * LEditorCamera
	 */
	class LEditorCamera : public LSceneCamera
	{
	public:
		LEditorCamera(const float DegFov, const float Width, const float Height,
					  const float NearP, const float FarP); 

		void Initialize();

		void Tick(const float DeltaTime);
		void UpdateCameraView();

		FORCEINLINE float GetDistance() const { return Distance; }
		FORCEINLINE const glm::vec3& GetFocalPoint() const { return FocalPoint; }

		FORCEINLINE void SetDistance(const float InDistance) 
		{ 
			if (Distance != InDistance)
			{
				Distance = InDistance; 
			}
		}

		FORCEINLINE void SetPerspective(const float InVerticalFovDeg, 
										const float InNearClip = 0.1f, 
										const float InFarClip = 1000.0f)
		{
			if ((ProjectionType != ECameraProjection::Perspective)
				|| (DegPerspectiveFOV != InVerticalFovDeg)
				|| (PerspectiveNear != InNearClip)
				|| (PerspectiveFar != InFarClip))
			{
				ProjectionType = ECameraProjection::Perspective;
				DegPerspectiveFOV = InVerticalFovDeg;
				PerspectiveNear = InNearClip;
				PerspectiveFar = InFarClip;

				OnCameraProjectionChanged.Broadcast(ECameraProjection::Perspective);
			}
		}

		void SetOrthographic(const float InWidth, const float InHeight, const float InNearClip = -1.0f, const float InFarClip = 1.0f)
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

		void SetViewportSize(const uint16_t InWidth, const uint16_t InHeight);

		FORCEINLINE glm::vec3 CalculatePosition() const
		{
			return (FocalPoint - (GetForwardDirection() * Distance + PositionDelta));
		}

		float GetCameraSpeed() const;

		virtual void SetPosition(const glm::vec3& InPosition) override
		{
			Position = InPosition;
			UpdateCameraView();
		}

		void MousePan(const glm::vec2& Delta);
		void MouseRotate(const glm::vec2& Delta);
		void MouseZoom(const float Delta);

		std::pair<float, float> GetPanSpeed() const;
		float GetZoomSpeed() const;

	private:
		bool OnKeyPress(LKeyPressedEvent& e);
		bool OnMouseScroll(MouseScrolledEvent& e);

	private:
		static constexpr float MIN_SPEED = 0.0005f;
		static constexpr float MAX_SPEED = 2.0f;
	private:
		bool bPanning = false;
		bool bRotating = false;

		float Distance = 0.0f;
		float NormalSpeed = 0.0020f;
		float TravelSpeed = 1.0f;

		glm::vec2 InitialMousePosition{};
		glm::vec3 InitialFocalPoint{};
		glm::vec3 InitialRotation{};

		EEditorCameraMode CameraMode = EEditorCameraMode::Arcball;

		bool bPitchLocked = false; /// EVALUATE
		bool bYawLocked = false;   /// EVALUATE
		bool bEnableCameraDamping = true;

		uint32_t ViewportWidth = 1280;
		uint32_t ViewportHeight = 720;

		friend class LEditorLayer;
		friend class LSceneManagerPanel;
		friend class LSceneSerializer; /* Access to populate members when deserializing a scene. */
	};

}
