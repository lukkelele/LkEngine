#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "CameraBase.h"

#include "LkEngine/Core/Math/MathLibrary.h"
#include "LkEngine/Core/Input/Keyboard.h"
#include "LkEngine/Core/Input/Mouse.h"


namespace LkEngine {

	/// FIXME: complete this and replace all float args in constructor
	struct FCameraSpecification
	{
		float FovDeg = 90.0f;

		float Width{};
		float Height{};

		float NearP{};
		float FarP{};
	};

	/**
	 * LCamera
	 *
	 *  Base camera class.
	 */
	class LCamera : public LObject
	{
		LK_DECLARE_MULTICAST_DELEGATE(FCameraProjectionChanged, const ECameraProjection);

		/** FCameraInputModified, called on input modifications. */
		LK_DECLARE_MULTICAST_DELEGATE(FCameraInputModified);

	public:
		LCamera();
		LCamera(const glm::mat4& InProjection);
		LCamera(const float InDegFov, const float InWidth, const float InHeight,
				const float InNearP, const float InFarP);
		virtual ~LCamera() = default;

		FORCEINLINE ECameraType GetType() const { return Type; }
		FORCEINLINE ECameraProjection GetProjectionType() const { return ProjectionType; }

		FORCEINLINE glm::vec3 GetPosition() const { return Position; }
		FORCEINLINE const glm::vec3& GetPosition() { return Position; }

		/* TODO: Remove as virtual. */
		virtual void SetPosition(const glm::vec3& InPosition)
		{
			Position = InPosition;
		}

		/* TODO: Remove as virtual. */
		virtual void SetProjection(const glm::mat4& InProjection)
		{
			if (ProjectionMatrix != InProjection)
			{
				ProjectionMatrix = InProjection;
			}
		}

		FORCEINLINE void SetProjectionType(ECameraProjection InProjection)
		{
			if (ProjectionType != InProjection)
			{
				ProjectionType = InProjection;
				OnCameraProjectionChanged.Broadcast(ProjectionType);
			}
		}

		FORCEINLINE float GetPitch() const { return Pitch; }
		FORCEINLINE float GetYaw() const { return Yaw; }
		FORCEINLINE float GetYawDelta() const { return YawDelta; }
		FORCEINLINE float GetPitchDelta() const { return PitchDelta; }

		FORCEINLINE void SetPitch(const float InPitch) 
		{ 
			if (Pitch != InPitch)
			{
				Pitch = InPitch; 
			}
		}

		FORCEINLINE float SetYaw(const float InYaw) 
		{ 
			if (Yaw != InYaw)
			{
				Yaw = InYaw; 
			}
		}

		virtual void SetActive(const bool InActive) 
		{ 
			if (bIsActive != InActive)
			{
				bIsActive = InActive; 
			}
		}

		FORCEINLINE const glm::mat4& GetViewMatrix() const { return ViewMatrix; }
		FORCEINLINE const glm::mat4& GetProjectionMatrix() const { return ProjectionMatrix; }
		FORCEINLINE glm::mat4 GetViewProjection() const { return GetProjectionMatrix() * ViewMatrix; }

		FORCEINLINE float GetRotation() { return glm::radians(Rotation); }

		FORCEINLINE glm::quat GetOrientation() const
		{
			return glm::quat(glm::vec3(-Pitch - PitchDelta, -Yaw - YawDelta, 0.0f));
		}

		FORCEINLINE glm::vec3 GetUpDirection() const
		{
			return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
		}

		FORCEINLINE glm::vec3 GetRightDirection() const
		{
			return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
		}

		template<typename TVector = glm::vec3>
		TVector GetForwardDirection() const
		{
			return TVector(glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f)));
		}

		FORCEINLINE const glm::vec3& GetOrigin() const { return Origin; }

		FORCEINLINE float GetPerspectiveNearClip() const { return PerspectiveNear; }
		FORCEINLINE float GetPerspectiveFarClip() const { return PerspectiveFar; }

		FORCEINLINE void SetPerspectiveNearClip(const float InNearClip)
		{
			if (PerspectiveNear != InNearClip)
			{
				PerspectiveNear = InNearClip;
			}
		}

		FORCEINLINE void SetPerspectiveFarClip(const float InFarClip)
		{
			if (PerspectiveFar != InFarClip)
			{
				PerspectiveFar = InFarClip;
			}
		}

		FORCEINLINE float GetOrthographicSize() const { return OrthographicSize; }
		FORCEINLINE float GetOrthographicNearClip() const { return OrthographicNear; }
		FORCEINLINE float GetOrthographicFarClip() const { return OrthographicFar; }

		FORCEINLINE void SetOrthographicNearClip(const float InNearClip)
		{
			if (OrthographicNear != InNearClip)
			{
				OrthographicNear = InNearClip;
			}
		}

		FORCEINLINE void SetOrthographicFarClip(const float InFarClip)
		{
			if (OrthographicFar != InFarClip)
			{
				OrthographicFar = InFarClip;
			}
		}

		void SetProjectionMatrix(const glm::mat4& InProjection)
		{
			if (ProjectionMatrix != InProjection)
			{
				ProjectionMatrix = InProjection;
			}
		}

		void SetPerspectiveProjectionMatrix(const float InRadFov, const float InWidth, const float InHeight, const float InNearP, const float InFarP)
		{
			LK_VERIFY((InWidth > 0) && (InHeight > 0), "Cannot set projection matrix with invalid arguments");
			ProjectionMatrix = glm::perspectiveFov(InRadFov, InWidth, InHeight, InNearP, InFarP);
		}

		void SetOrthoProjectionMatrix(const float InWidth, const float InHeight, const float InNearP, const float InFarP)
		{
			LK_VERIFY((InWidth > 0) && (InHeight > 0));
			ProjectionMatrix = glm::ortho(
				-(InWidth * 0.50f), (InWidth * 0.50f),
				-(InHeight * 0.50f), (InHeight * 0.50f),
				InNearP, InFarP);
		}

		/// TODO: Rename these awful function names.
		FORCEINLINE float GetDegPerspectiveVerticalFOV() const { return DegPerspectiveFOV; }
		FORCEINLINE float GetRadPerspectiveVerticalFOV() const { return glm::radians(DegPerspectiveFOV); }

		virtual void SetMouseEnabled(const bool InEnabled)
		{
			if (bMouseEnabled != InEnabled)
			{
				bMouseEnabled = InEnabled;
				OnCameraInputModified.Broadcast();
			}
		}

		virtual void SetKeyboardEnabled(const bool InEnabled)
		{
			if (bKeyboardEnabled != InEnabled)
			{
				bKeyboardEnabled = InEnabled;
				OnCameraInputModified.Broadcast();
			}
		}

		FORCEINLINE float GetRotationSpeed() const { return RotationSpeed; }
		FORCEINLINE const float GetPerspectiveDegFov() const { return DegPerspectiveFOV; }

	public:
		FCameraProjectionChanged OnCameraProjectionChanged;
		FCameraInputModified OnCameraInputModified;

	protected:
		bool bIsActive = false;
		ECameraProjection ProjectionType = ECameraProjection::Perspective;
		ECameraType Type = ECameraType::None;

		glm::vec3 Origin = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Position{};
		glm::vec3 Direction{};
		glm::vec3 FocalPoint{};

		glm::vec3 PositionDelta = { 0.0f, 0.0f, 0.0f };
		glm::vec3 RightDirection = { 0.0f, 0.0f, 0.0f };

		float VerticalFOV = 90.0f;
		float AspectRatio = (16.0f / 9.0f);
		float NearClip = 0.0f;
		float FarClip = 2000.0f;

		float Pitch = 0.0f;
		float PitchDelta = 0.0f;
		float Yaw = 0.0f;
		float YawDelta = 0.0f;

		float DegPerspectiveFOV = 45.0f;
		float PerspectiveNear = 0.1f;
		float PerspectiveFar = 1000.0f;
		float OrthographicSize = 10.0f;
		float OrthographicNear = -1.0f;
		float OrthographicFar = 1.0f;

		float Rotation = 0.0f;
		float RotationSpeed = 0.280f;
		float MouseSpeed = 1.0f;
		bool bMouseEnabled = true;
		bool bKeyboardEnabled = true;

		glm::mat4 ViewMatrix = glm::mat4(1.0f);
		glm::mat4 ProjectionMatrix = glm::mat4(1.0f);

	private:
		friend class LEditorLayer;
		friend class LSceneSerializer;

		LCLASS(LCamera);
	};

}
