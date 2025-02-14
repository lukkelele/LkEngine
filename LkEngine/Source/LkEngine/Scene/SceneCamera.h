#pragma once

#include "LkEngine/Renderer/CameraBase.h"
#include "LkEngine/Renderer/Camera.h"


namespace LkEngine {

	class LScene;

	class LSceneCamera : public LCamera
	{
	public:
		virtual ~LSceneCamera() = default;

		virtual void Tick(const float InDeltaTime = 0.0f);

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

		void UpdateView();
		void UpdateProjection();
		void UpdateViewProjection();

		FORCEINLINE float GetDistance() const { return Distance; }
		FORCEINLINE float GetNormalSpeed() const { return NormalSpeed; }
		FORCEINLINE float GetTravelSpeed() const { return TravelSpeed; }
		FORCEINLINE void SetDistance(const float InDistance) { Distance = InDistance; }
		FORCEINLINE void SetNormalSpeed(const float InSpeed) { NormalSpeed = InSpeed; }
		FORCEINLINE void SetTravelSpeed(const float InSpeed) { TravelSpeed = InSpeed; }

	protected:
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

	private:
		friend class LEditorLayer;
	};

}
