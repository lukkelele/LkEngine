#pragma once

#include "LkEngine/Renderer/CameraBase.h"
#include "LkEngine/Renderer/Camera.h"


namespace LkEngine {

	class LScene;

	class LSceneCamera : public LCamera
	{
	public:
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

		FORCEINLINE void SetOrthographic(const float InWidth, 
										 const float InHeight, 
										 const float InNearClip = -1.0f, 
										 const float InFarClip = 1.0f)
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

		void SetViewportSize(uint32_t width, uint32_t height);

		void Update(float ts);
		void UpdateView();
		void UpdateProjection();
		void UpdateViewProjection();

		FORCEINLINE float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
		FORCEINLINE float GetPerspectiveFarClip() const { return m_PerspectiveFar; }
		FORCEINLINE float GetDegPerspectiveVerticalFOV() const { return m_DegPerspectiveFOV; }
		FORCEINLINE float GetRadPerspectiveVerticalFOV() const { return glm::radians(m_DegPerspectiveFOV); }

		/** */
		FORCEINLINE void SetDegPerspectiveVerticalFOV(const float InVerticalFovDeg) 
		{ 
			if (m_DegPerspectiveFOV != InVerticalFovDeg)
			{
				m_DegPerspectiveFOV = InVerticalFovDeg; 
				bDirty = true;
			}
		}

		/** */
		FORCEINLINE void SetRadPerspectiveVerticalFOV(const float InVerticalFovRad) 
		{ 
			if (const float InVerticalFovDeg = glm::degrees(InVerticalFovRad); m_DegPerspectiveFOV != InVerticalFovDeg)
			{
				m_DegPerspectiveFOV = InVerticalFovDeg;
				bDirty = true;
			}
		}

		/** */
		FORCEINLINE void SetPerspectiveNearClip(const float InNearClip) 
		{ 
			if (m_PerspectiveNear != InNearClip)
			{
				m_PerspectiveNear = InNearClip; 
				bDirty = true;
			}
		}

		/** */
		FORCEINLINE void SetPerspectiveFarClip(const float InFarClip) 
		{ 
			if (m_PerspectiveFar != InFarClip)
			{
				m_PerspectiveFar = InFarClip;
				bDirty = true;
			}
		}

		FORCEINLINE float GetOrthographicSize() const { return m_OrthographicSize; }
		FORCEINLINE float GetOrthographicNearClip() const { return m_OrthographicNear; }
		FORCEINLINE float GetOrthographicFarClip() const { return m_OrthographicFar; }

		FORCEINLINE void SetOrthographicSize(const float InSize) 
		{ 
			if (m_OrthographicSize != InSize)
			{
				m_OrthographicSize = InSize; 
				bDirty = true;
			}
		}

		FORCEINLINE void SetOrthographicNearClip(const float InNearClip) 
		{ 
			if (m_OrthographicNear != InNearClip)
			{
				m_OrthographicNear = InNearClip; 
				bDirty = true;
			}
		}

		FORCEINLINE void SetOrthographicFarClip(const float InFarClip) 
		{ 
			if (m_OrthographicFar != InFarClip)
			{
				m_OrthographicFar = InFarClip; 
				bDirty = true;
			}
		}

		FORCEINLINE glm::vec3& GetPos() { return m_Pos; }
		FORCEINLINE glm::vec3 GetPos() const { return m_Pos; }

		FORCEINLINE const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		FORCEINLINE const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

	private:
		glm::vec3 m_Pos = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_PosDelta = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Origin = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Offset = { 0.0f, 0.0f, 0.0f };
		glm::vec2 m_MouseDelta = { 0.0f, 0.0f };
		glm::vec2 m_MousePos = { 0.0f, 0.0f };
		glm::vec2 m_InitialMousePos{};

		float m_OrthographicSize = 10.0f;
		float m_OrthographicNear = -1.0f;
		float m_OrthographicFar = 1.0f;

		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewProjectionMatrix = glm::mat4(1.0f); 
	};

}
