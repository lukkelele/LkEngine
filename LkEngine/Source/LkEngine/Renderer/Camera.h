#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "CameraBase.h"

#include "LkEngine/Core/Math/MathLibrary.h"
#include "LkEngine/Core/Input/Keyboard.h"
#include "LkEngine/Core/Input/Mouse.h"


namespace LkEngine {

	enum class ECameraAction : uint16_t
	{
		None   = 0,
		Pan    = LK_BIT(0),
		Rotate = LK_BIT(1),
		Zoom   = LK_BIT(2),
	};
	LK_ENUM_CLASS(ECameraAction);
	LK_ENUM_RANGE_FLAGS_BY_FIRST_AND_LAST(ECameraAction, ECameraAction::None, ECameraAction::Zoom);

	enum class ECameraModifier : int32_t
	{
		None = 0,
		MouseEnabled    = LK_BIT(1),
		KeyboardEnabled = LK_BIT(2),
		PitchLocked     = LK_BIT(3),
		YawLocked       = LK_BIT(4),
		Damping         = LK_BIT(5),
	};
	LK_ENUM_CLASS(ECameraModifier);

	/**
	 * LCamera
	 *
	 *  Base camera class.
	 */
	class LCamera : public LObject
	{
		LK_DECLARE_MULTICAST_DELEGATE(FCameraProjectionChanged, const ECameraProjection);
		LK_DECLARE_MULTICAST_DELEGATE(FCameraActivityChanged, LCamera*, const bool);

		/** FCameraInputModified, called on input modifications. */
		LK_DECLARE_MULTICAST_DELEGATE(FCameraInputModified); /* Keep? */

	public:
		LCamera();
		LCamera(const glm::mat4& InProjection);
		LCamera(const float InDegFov, const float InWidth, const float InHeight,
				const float InNearP, const float InFarP);
		virtual ~LCamera() = default;

		virtual ECameraType GetType() const { return ECameraType::None; }
		FORCEINLINE ECameraProjection GetProjectionType() const { return ProjectionType; }

		FORCEINLINE bool IsActive() const { return bIsActive; }

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

		FORCEINLINE void SetProjectionType(ECameraProjection InProjection)
		{
			if (ProjectionType != InProjection)
			{
				ProjectionType = InProjection;
				OnCameraProjectionChanged.Broadcast(ProjectionType);
			}
		}

		FORCEINLINE void SetPitch(const float InPitch) { Pitch = InPitch; }
		FORCEINLINE float GetPitch() const { return Pitch; }
		FORCEINLINE float GetPitchDelta() const { return PitchDelta; }

		FORCEINLINE void SetYaw(const float InYaw) { Yaw = InYaw; }
		FORCEINLINE float GetYaw() const { return Yaw; }
		FORCEINLINE float GetYawDelta() const { return YawDelta; }

		FORCEINLINE void SetActive(const bool InActive) 
		{ 
			if (bIsActive != InActive)
			{
				bIsActive = InActive; 
				OnCameraActivityChanged.Broadcast(this, bIsActive);
			}
		}

		FORCEINLINE const glm::mat4& GetViewMatrix() const { return ViewMatrix; }
		FORCEINLINE const glm::mat4& GetProjectionMatrix() const { return ProjectionMatrix; }
		FORCEINLINE glm::mat4 GetViewProjection() const { return GetProjectionMatrix() * ViewMatrix; }
		FORCEINLINE float GetRotation() { return glm::radians(Rotation); }
		FORCEINLINE float GetRotationSpeed() const { return RotationSpeed; }

		FORCEINLINE const glm::vec3& GetOrigin() const { return Origin; }
		FORCEINLINE const glm::vec3& GetFocalPoint() const { return FocalPoint; }
		FORCEINLINE const glm::vec3& GetDirection() const { return Direction; }

		FORCEINLINE float GetPerspectiveNearClip() const { return PerspectiveNear; }
		FORCEINLINE float GetPerspectiveFarClip() const { return PerspectiveFar; }
		FORCEINLINE void SetPerspectiveNearClip(const float InNearClip) { PerspectiveNear = InNearClip; }
		FORCEINLINE void SetPerspectiveFarClip(const float InFarClip) { PerspectiveFar = InFarClip; }

		FORCEINLINE float GetOrthographicSize() const { return OrthographicSize; }
		FORCEINLINE float GetOrthographicNearClip() const { return OrthographicNear; }
		FORCEINLINE float GetOrthographicFarClip() const { return OrthographicFar; }
		FORCEINLINE void SetOrthographicNearClip(const float InNearClip) { OrthographicNear = InNearClip; }
		FORCEINLINE void SetOrthographicFarClip(const float InFarClip) { OrthographicFar = InFarClip; }

		FORCEINLINE void SetProjectionMatrix(const glm::mat4& InProjection) { ProjectionMatrix = InProjection; }
		FORCEINLINE void SetPerspectiveProjectionMatrix(const float InRadFov, 
														const float InWidth, 
														const float InHeight, 
														const float InNearP, 
														const float InFarP)
		{
			LK_CORE_ASSERT((InWidth > 0) && (InHeight > 0), "Cannot set projection matrix with invalid arguments");
			ProjectionMatrix = glm::perspectiveFov(InRadFov, InWidth, InHeight, InNearP, InFarP);
		}

		FORCEINLINE void SetOrthoProjectionMatrix(const float InWidth, const float InHeight, const float InNearP, const float InFarP)
		{
			LK_CORE_ASSERT((InWidth > 0) && (InHeight > 0));
			ProjectionMatrix = glm::ortho(
				-(InWidth * 0.50f), (InWidth * 0.50f),
				-(InHeight * 0.50f), (InHeight * 0.50f),
				InNearP, InFarP);
		}

		template<enum EAngleUnit = EAngleUnit::Degree> 
		float GetPerspectiveFov() const;

		template<enum EAngleUnit = EAngleUnit::Degree> 	
		float GetPerspectiveVerticalFov() const;

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

		void SetMouseEnabled(const bool InEnabled)
		{
			const bool MouseEnabled = (ModifierFlags & ECameraModifier::MouseEnabled);
			if (MouseEnabled != InEnabled)
			{
				if (MouseEnabled)
				{
					ModifierFlags &= ~ECameraModifier::MouseEnabled;
				}
				else
				{
					ModifierFlags |= ECameraModifier::MouseEnabled;
				}

				OnCameraInputModified.Broadcast();
			}
		}

		void SetKeyboardEnabled(const bool InEnabled)
		{
			const bool KeyboardEnabled = ModifierFlags & ECameraModifier::KeyboardEnabled;
			if (KeyboardEnabled != InEnabled)
			{
				if (KeyboardEnabled)
				{
					ModifierFlags &= ~ECameraModifier::KeyboardEnabled;
				}
				else
				{
					ModifierFlags |= ECameraModifier::KeyboardEnabled;
				}

				OnCameraInputModified.Broadcast();
			}
		}

	public:
		/**
		 * Important to note that delegates have trouble being inherited for multiple
		 * derivations. The inheritence for LEditorCamera looks like this:
		 *  [ LEditorCamera --> LSceneCamera --> LCamera ]
		 * This can be troublesome so it is best to declare camera delegates static.
		 */
		inline static FCameraActivityChanged OnCameraActivityChanged;
		FCameraProjectionChanged OnCameraProjectionChanged;
		FCameraInputModified OnCameraInputModified;
	protected:
		bool bIsActive = false;
		ECameraProjection ProjectionType = ECameraProjection::Perspective;

		int32_t ModifierFlags = 0;
		static_assert(std::is_same_v<decltype(ModifierFlags), std::underlying_type_t<ECameraModifier>>);

		glm::vec3 Origin = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Direction{};
		glm::vec3 FocalPoint{};
		glm::vec3 PositionDelta = { 0.0f, 0.0f, 0.0f };

		float Pitch = 0.0f;
		float PitchDelta = 0.0f;
		float Yaw = 0.0f;
		float YawDelta = 0.0f;

		float AspectRatio = (16.0f / 9.0f);

		float DegPerspectiveFov = 45.0f;
		float PerspectiveNear = 0.1f;
		float PerspectiveFar = 1000.0f;

		float OrthographicSize = 10.0f;
		float OrthographicNear = -1.0f;
		float OrthographicFar = 1.0f;

		float Rotation = 0.0f;
		float RotationSpeed = 0.280f;

		glm::mat4 ViewMatrix = glm::mat4(1.0f);
		glm::mat4 ProjectionMatrix = glm::mat4(1.0f);

	private:
		friend class LEditorLayer;
		friend class LSceneSerializer;

		LCLASS(LCamera);
	};

	template<> 
	inline float LCamera::GetPerspectiveFov<EAngleUnit::Degree>() const 
	{ 
		return DegPerspectiveFov; 
	}

	template<> 
	inline float LCamera::GetPerspectiveFov<EAngleUnit::Radian>() const 
	{ 
		return glm::radians(DegPerspectiveFov); 
	}

	template<> 
	inline float LCamera::GetPerspectiveVerticalFov<EAngleUnit::Degree>() const 
	{ 
		return DegPerspectiveFov; 
	}

	template<> 
	inline float LCamera::GetPerspectiveVerticalFov<EAngleUnit::Radian>() const 
	{ 
		return glm::radians(DegPerspectiveFov); 
	}

}
