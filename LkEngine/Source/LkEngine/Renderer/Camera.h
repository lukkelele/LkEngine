#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"
#include "CameraBase.h"

#include "LkEngine/Core/Math/MathLibrary.h"

#include "LkEngine/Input/Keyboard.h"
#include "LkEngine/Input/Mouse.h"


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

		/** FCameraInputModified, delegate for whenever input modifications take place. */
		LK_DECLARE_MULTICAST_DELEGATE(FCameraInputModified);

	public:
		LCamera();
		LCamera(const glm::mat4& projection);
		LCamera(const float InDegFov, const float InWidth, const float InHeight, 
				const float InNearP, const float InFarP);
		virtual ~LCamera() = default;

		/**
		 * @brief Check if camera has changed in configuration and needs to be updated.
		 */
		FORCEINLINE bool IsDirty() const { return bDirty; }

		FORCEINLINE ECameraType GetType() const { return Type; }
		FORCEINLINE ECameraProjection GetProjectionType() const { return ProjectionType; }

		virtual void SetProjection(const glm::mat4& InProjection) 
		{ 
			if (m_ProjectionMatrix != InProjection)
			{
				m_ProjectionMatrix = InProjection;
				bDirty = true;
			}
		}
		FORCEINLINE void SetProjectionType(ECameraProjection InProjection) 
		{ 
			if (ProjectionType != InProjection)
			{
				ProjectionType = InProjection; 
				bDirty = true;
				OnCameraProjectionChanged.Broadcast(ProjectionType);
			}
		}

		FORCEINLINE const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		FORCEINLINE float GetRotation() { return glm::radians(m_Rotation); }
		FORCEINLINE float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
		FORCEINLINE float GetPerspectiveFarClip() const { return m_PerspectiveFar; }

		FORCEINLINE void SetPerspectiveNearClip(const float InNearClip) 
		{ 
			if (m_PerspectiveNear != InNearClip)
			{
				m_PerspectiveNear = InNearClip; 
				bDirty = true;
			}
		}

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

		void SetProjectionMatrix(const glm::mat4& InProjection)
		{
			if (m_ProjectionMatrix != InProjection)
			{
				m_ProjectionMatrix = InProjection;
				bDirty = true;
			}
		}

		void SetPerspectiveProjectionMatrix(const float InRadFov, 
											const float InWidth, 
											const float InHeight, 
											const float InNearP, 
											const float InFarP)
		{
			LK_VERIFY((InWidth > 0) && (InHeight > 0), "Cannot set projection matrix with invalid arguments");
			m_ProjectionMatrix = glm::perspectiveFov(InRadFov, InWidth, InHeight, InNearP, InFarP);
			bDirty = true;
		}

		void SetOrthoProjectionMatrix(const float InWidth, 
									  const float InHeight, 
									  const float InNearP, 
									  const float InFarP)
		{
			LK_VERIFY((InWidth > 0) && (InHeight > 0));
			m_ProjectionMatrix = glm::ortho(
				-(InWidth  * 0.50f), (InWidth  * 0.50f),
				-(InHeight * 0.50f), (InHeight * 0.50f),
				InNearP, InFarP);

			bDirty = true;
		}

		virtual void SetMouseEnabled(const bool InEnabled) 
		{ 
			if (m_MouseEnabled != InEnabled)
			{
				m_MouseEnabled = InEnabled; 
				OnCameraInputModified.Broadcast();
			}
		}

		virtual void SetKeyboardEnabled(const bool InEnabled) 
		{ 
			if (m_KeyboardEnabled != InEnabled)
			{
				m_KeyboardEnabled = InEnabled;
				OnCameraInputModified.Broadcast();
			}
		}

	public:
		FCameraProjectionChanged OnCameraProjectionChanged;
		FCameraInputModified OnCameraInputModified;
	protected:
		ECameraProjection ProjectionType = ECameraProjection::Perspective;
		ECameraType Type = ECameraType::None;

		/** Flag to indicate that the camera is in need of thorough update. */
		bool bDirty = false;

		float m_DegPerspectiveFOV = 45.0f;
		float m_PerspectiveNear = 0.1f;
		float m_PerspectiveFar = 1000.0f;
		float m_OrthographicSize = 10.0f;
		float m_OrthographicNear = -1.0f;
		float m_OrthographicFar = 1.0f;

		float m_Rotation = 0.0f;
		float m_RotationSpeed = 0.0002f;
		float m_TravelSpeed = 1.0f;
		float m_MouseSpeed = 1.0f;
		bool m_MouseEnabled = true;
		bool m_KeyboardEnabled = true;

		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);

		friend class LSceneSerializer;

	private:
		LCLASS(LCamera);
	};

}
