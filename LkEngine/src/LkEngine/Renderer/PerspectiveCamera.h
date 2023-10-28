#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "LkEngine/Input/Keyboard.h"
#include "LkEngine/Input/Mouse.h"
#include "LkEngine/Renderer/Camera.h"


namespace LkEngine {

	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera() = default;
		PerspectiveCamera(float FOV, float nearPlane, float farPlane);
		virtual ~PerspectiveCamera() = default;

		void Update(float ts);

		glm::quat GetOrientation() const;
		glm::vec3 GetDirection();
		glm::vec3 CalculatePosition() const;
		glm::mat4& GetInverseViewProjection() { return m_InverseViewProjection; }
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetForwardDirection() const;
		glm::vec3 GetUpDirection() const;

		float GetFOV() const { return m_FOV; }
		float GetNearClip() const { return m_NearPlane; }
		float GetFarClip() const { return m_FarPlane; }
		void SetRotation(float rot) { m_Rotation = rot; }
		void SetFOV(float fov) { m_FOV = fov; UpdateProjection(); }
		void SetProjection(float fov, float _near, float _far);
		//void SetScene(Scene* scene) { m_Scene = scene; }
		//float GetRotation() { return glm::radians(m_Rotation); }

		void UpdateView();
		void UpdateProjection();
		void UpdateMousePosition();

	protected:
		glm::vec3 m_Direction{};
		glm::vec3 m_ForwardDirection = { 0.0f, 0.0f, 1.0f };
		glm::vec3 m_UpDirection = { 0.0f, 1.0f, 0.0f };
		glm::vec3 m_RightDirection{};

		float m_FOV = 50.0f;
		float m_NearPlane = 0.10f, m_FarPlane = 1000.0f;
		float m_Distance = 500.0f;
		float m_Yaw = 0.0f, m_Pitch = 0.0f;
		float m_YawDelta = 0.0f, m_PitchDelta = 0.0f;
	};

}
