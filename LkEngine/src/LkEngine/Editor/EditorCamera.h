#pragma once

#include "LkEngine/Renderer/Camera.h"

#include "LkEngine/Input/Keyboard.h"
#include "LkEngine/Input/Mouse.h"

#include "LkEngine/Core/Event/KeyEvent.h"
#include "LkEngine/Core/Event/MouseEvent.h"

#include "LkEngine/ImGui/ImGuiLayer.h"


namespace LkEngine {

	class EditorCamera : public Camera
	{
	public:
		EditorCamera(const float degFov, const float width, const float height, const float nearP, const float farP); 

		void Init();
		void OnUpdate(const Timestep ts);
		void UpdateCameraView();
		void OnEvent(Event& e);

		void SetPerspective(float verticalFOV, float nearClip = 0.1f, float farClip = 1000.0f);
		void SetOrthographic(float width, float height, float nearClip = -1.0f, float farClip = 1.0f);
		void SetViewportSize(uint32_t width, uint32_t height);

		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		glm::mat4 GetViewProjectionMatrix() const { return GetProjectionMatrix() * m_ViewMatrix; }

		unsigned int GetGizmoMode() const;

		inline float GetDistance() const { return m_Distance; }
		inline void SetDistance(float distance) { m_Distance = distance; }
		inline const glm::vec3& GetFocalPoint() const { return m_FocalPoint; }

		glm::vec3 GetUpDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetForwardDirection() const;
		const glm::vec3& GetPosition() const { return m_Position; }
		const glm::vec3& GetOrigin() const { return m_Origin; }
		glm::quat GetOrientation() const;
		glm::vec3 CalculatePosition() const;
		float GetCameraSpeed() const;
		const float GetPerspectiveDegFov() const { return m_DegPerspectiveFOV;  }

		float GetPitch() const { return m_Pitch; }
		float GetYaw() const { return m_Yaw; }

		void SetActive(bool active) { m_IsActive = active; }
		void SetPosition(const glm::vec3& position);

	private:
		bool OnKeyPress(KeyPressedEvent& e);
		bool OnMouseScroll(MouseScrolledEvent& e);

		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseZoom(float delta);

		std::pair<float, float> PanSpeed() const;
		float RotationSpeed() const;
		float ZoomSpeed() const;

		inline float GetYawDelta() const { return m_YawDelta; }
		inline float GetPitchDelta() const { return m_PitchDelta; }

	private:
		glm::mat4 m_ViewMatrix;
		glm::vec3 m_Position{}, m_Direction{}, m_FocalPoint{};
		glm::vec3 m_Origin = { 0.0f, 0.0f, 0.0f };

		float m_VerticalFOV, m_AspectRatio, m_NearClip, m_FarClip;

		enum class Mode
		{
			None = 0,
			Flycam,
			Arcball
		} m_CameraMode = Mode::Arcball;

		bool m_IsActive = false;
		bool m_Panning, m_Rotating;
		glm::vec2 m_InitialMousePosition {};
		glm::vec3 m_InitialFocalPoint, m_InitialRotation;

		float m_Distance;
		float m_NormalSpeed{ 0.002f };

		GizmoMode m_GizmoMode = GizmoMode::Translate;

		float m_Pitch = 0.0f;
		float m_Yaw = 0.0f;
		float m_PitchDelta{}, m_YawDelta{};
		glm::vec3 m_PositionDelta{ 0.0f, 0.0f, 0.0f };
		glm::vec3 m_RightDirection{ 0.0f, 0.0f, 0.0f };

		float m_RotationSpeed = 0.28f;

		uint32_t m_ViewportWidth{ 1280 }, m_ViewportHeight{ 720 };

		bool m_EnableCameraDamping = true;

		constexpr static float MIN_SPEED{ 0.0005f }, MAX_SPEED{ 2.0f };

		friend class EditorLayer;
		friend class SceneSerializer;
	};

}
