#pragma once

#include "LkEngine/Renderer/CameraBase.h"
#include "LkEngine/Renderer/Camera.h"


namespace LkEngine {

	class LScene;

	class LSceneCamera : public LCamera
	{
	public:
		FORCEINLINE void SetPerspective(const float InVerticalFovDeg, const float InNearClip = 0.1f, const float InFarClip = 1000.0f)
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

		void SetViewportSize(const uint32_t InWidth, const uint32_t InHeight);

		void Update(float ts);
		void UpdateView();
		void UpdateProjection();
		void UpdateViewProjection();

	#if 0
		FORCEINLINE float GetPerspectiveNearClip() const { return PerspectiveNear; }
		FORCEINLINE float GetPerspectiveFarClip() const { return PerspectiveFar; }
		FORCEINLINE float GetDegPerspectiveVerticalFOV() const { return DegPerspectiveFOV; }
		FORCEINLINE float GetRadPerspectiveVerticalFOV() const { return glm::radians(DegPerspectiveFOV); }

		/** */
		FORCEINLINE void SetDegPerspectiveVerticalFOV(const float InVerticalFovDeg) 
		{ 
			if (DegPerspectiveFOV != InVerticalFovDeg)
			{
				DegPerspectiveFOV = InVerticalFovDeg; 
			}
		}

		/** */
		FORCEINLINE void SetRadPerspectiveVerticalFOV(const float InVerticalFovRad) 
		{ 
			if (const float InVerticalFovDeg = glm::degrees(InVerticalFovRad); DegPerspectiveFOV != InVerticalFovDeg)
			{
				DegPerspectiveFOV = InVerticalFovDeg;
			}
		}

		/** */
		FORCEINLINE void SetPerspectiveNearClip(const float InNearClip) 
		{ 
			if (PerspectiveNear != InNearClip)
			{
				PerspectiveNear = InNearClip; 
			}
		}

		/** */
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

		FORCEINLINE void SetOrthographicSize(const float InSize) 
		{ 
			if (OrthographicSize != InSize)
			{
				OrthographicSize = InSize; 
			}
		}

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
	#endif

		FORCEINLINE glm::vec3& GetPosition() { return Position; }
		FORCEINLINE glm::vec3 GetPosition() const { return Position; }

	protected:
		/** @brief Camera offset. */
		glm::vec3 CameraBoom{};

		glm::vec2 MouseDelta = { 0.0f, 0.0f };
		glm::vec2 MousePos = { 0.0f, 0.0f };
		glm::vec2 m_InitialMousePos{};

		float Distance = 0.0f;
		float NormalSpeed = 0.0020f;
		float TravelSpeed = 1.0f;

		float OrthographicSize = 10.0f;
		float OrthographicNear = -1.0f;
		float OrthographicFar = 1.0f;
	};

}
