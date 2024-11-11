#pragma once

#include "LkEngine/Core/Core.h"
#include "LkEngine/Core/Time/Timer.h"

#include "LkEngine/Renderer/Camera.h"
#include "LkEngine/Core/Time/Timestep.h"

#include "LkEngine/Input/Keyboard.h"
#include "LkEngine/Input/Mouse.h"

#include "LkEngine/Core/Event/KeyEvent.h"
#include "LkEngine/Core/Event/MouseEvent.h"

#include "LkEngine/Editor/EditorCore.h"


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
	class LEditorCamera : public LCamera
	{
	public:
		LEditorCamera(const float DegFov, const float Width, const float Height,
					  const float NearP, const float FarP); 

		void Initialize();

		void OnUpdate(const FTimestep DeltaTime);
		void UpdateCameraView();

		/// @TODO: Patch out
		void OnEvent(LEvent& e);

		FORCEINLINE void SetPerspective(const float InVerticalFovDeg, 
										const float InNearClip = 0.1f, 
										const float InFarClip = 1000.0f)
		{
			if ((ProjectionType != ECameraProjection::Perspective)
				|| (m_DegPerspectiveFOV != InVerticalFovDeg)
				|| (m_PerspectiveNear != InNearClip)
				|| (m_PerspectiveFar != InFarClip))
			{
				ProjectionType = ECameraProjection::Perspective;
				m_DegPerspectiveFOV = InVerticalFovDeg;
				m_PerspectiveNear = InNearClip;
				m_PerspectiveFar = InFarClip;

				bDirty = true;
				OnCameraProjectionChanged.Broadcast(ECameraProjection::Perspective);
			}
		}

		void SetOrthographic(const float InWidth, const float InHeight, 
							 const float InNearClip = -1.0f, const float InFarClip = 1.0f)
		{
			if ((ProjectionType != ECameraProjection::Perspective)
				|| (m_OrthographicNear != InNearClip)
				|| (m_OrthographicFar != InFarClip))
			{
				ProjectionType = ECameraProjection::Orthographic;
				m_OrthographicNear = InNearClip;
				m_OrthographicFar = InFarClip;

				bDirty = true;
				OnCameraProjectionChanged.Broadcast(ECameraProjection::Orthographic);
			}
		}

		void SetViewportSize(const uint16_t InWidth, const uint16_t InHeight);

		FORCEINLINE const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }

		FORCEINLINE glm::mat4 GetViewProjectionMatrix() const 
		{ 
			return GetProjectionMatrix() * m_ViewMatrix; 
		}

		FORCEINLINE float GetDistance() const { return m_Distance; }
		FORCEINLINE const glm::vec3& GetFocalPoint() const { return m_FocalPoint; }

		/// TODO: Patch out
		FORCEINLINE EGizmo GetGizmoMode() const { return GizmoMode; }

		FORCEINLINE void SetDistance(const float InDistance) 
		{ 
			if (m_Distance != InDistance)
			{
				m_Distance = InDistance; 
				bDirty = true;
			}
		}

		FORCEINLINE glm::vec3 GetUpDirection() const
		{
			return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
		}

		FORCEINLINE glm::vec3 GetRightDirection() const
		{
			return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
		}

		template<typename T = glm::vec3>
		T GetForwardDirection() const
		{
			return T(glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f)));
		}

		FORCEINLINE const glm::vec3& GetPosition() const { return m_Position; }
		FORCEINLINE const glm::vec3& GetOrigin() const { return m_Origin; }

		FORCEINLINE glm::quat GetOrientation() const
		{
			return glm::quat(glm::vec3(-m_Pitch - m_PitchDelta, -m_Yaw - m_YawDelta, 0.0f));
		}

		FORCEINLINE glm::vec3 CalculatePosition() const
		{
			return (m_FocalPoint - (GetForwardDirection() * m_Distance + m_PositionDelta));
		}

		float GetCameraSpeed() const;

		FORCEINLINE const float GetPerspectiveDegFov() const 
		{ 
			return m_DegPerspectiveFOV;  
		}

		FORCEINLINE float GetPitch() const { return m_Pitch; }
		FORCEINLINE float GetYaw() const { return m_Yaw; }

		FORCEINLINE void SetPitch(const float InPitch) 
		{ 
			if (m_Pitch != InPitch)
			{
				m_Pitch = InPitch; 
				bDirty = true;
			}
		}

		FORCEINLINE float SetYaw(const float InYaw) 
		{ 
			if (m_Yaw != InYaw)
			{
				m_Yaw = InYaw; 
				bDirty = true;
			}
		}

		FORCEINLINE void SetActive(const bool InActive) 
		{ 
			if (bIsActive != InActive)
			{
				bIsActive = InActive; 
				bDirty = true;
			}
		}

		FORCEINLINE void SetPosition(const glm::vec3& InPosition)
		{
			m_Position = InPosition;
			UpdateCameraView();
		}

		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseZoom(float delta);

		std::pair<float, float> PanSpeed() const;

		FORCEINLINE float RotationSpeed() const { return m_RotationSpeed; }

		float ZoomSpeed() const;

	private:
		bool OnKeyPress(KeyPressedEvent& e);
		bool OnMouseScroll(MouseScrolledEvent& e);

		FORCEINLINE float GetYawDelta() const { return m_YawDelta; }
		FORCEINLINE float GetPitchDelta() const { return m_PitchDelta; }

	private:
		glm::vec3 m_Position{};
		glm::vec3 m_Direction{};
		glm::vec3 m_FocalPoint{};
		glm::vec3 m_Origin = { 0.0f, 0.0f, 0.0f };

		float m_VerticalFOV = 90.0f;
		float m_AspectRatio = (16.0f / 9.0f);
		float m_NearClip = 0.0f;
		float m_FarClip = 2000.0f;

		EEditorCameraMode CameraMode = EEditorCameraMode::Arcball;

		bool bIsActive = false;
		bool bPanning = false;
		bool bRotating = false;

		glm::vec2 m_InitialMousePosition{};
		glm::vec3 m_InitialFocalPoint{};
		glm::vec3 m_InitialRotation{};

		float m_Distance = 0.0f;
		float m_NormalSpeed = 0.0020f;

		EGizmo GizmoMode = EGizmo::Translate; /* FIXME */

		float m_Pitch = 0.0f;
		float m_PitchDelta{};
		float m_Yaw = 0.0f;
		float m_YawDelta{};

		glm::vec3 m_PositionDelta = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_RightDirection = { 0.0f, 0.0f, 0.0f };

		bool m_PitchLocked = false;
		bool m_YawLocked = false;

		float m_RotationSpeed = 0.28f;

		uint32_t m_ViewportWidth = 1280;
		uint32_t m_ViewportHeight = 720;

		bool m_EnableCameraDamping = true;

		static constexpr float MIN_SPEED = 0.0005f;
		static constexpr float MAX_SPEED = 2.0f;

		friend class LEditorLayer;
		friend class LSceneManagerPanel;
		friend class LSceneSerializer; /* Access to populate members when deserializing a scene. */
	};

}
