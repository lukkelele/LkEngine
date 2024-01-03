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


namespace LkEngine {

	class Camera 
	{
	public:
		enum class ProjectionType { Perspective = 0, Orthographic = 1 };
	public:
		Camera() = default;
		virtual ~Camera() = default;

		virtual void Update(float ts) = 0;
		virtual void UpdateView() = 0;
		virtual void UpdateProjection() = 0;
		virtual void UpdateViewProjection() = 0;
		virtual void SetProjection(glm::mat4& proj) { m_ProjectionMatrix = proj; } 
		glm::mat4 GetView() const { return m_ViewMatrix; }
		glm::mat4 GetProjection() const { return m_ProjectionMatrix; }
		glm::mat4 GetViewProjection() const { return m_ViewProjectionMatrix; }
		glm::mat4& GetInverseViewProjection() { return m_InverseViewProjectionMatrix; }
		glm::mat4& GetInverseView() { return m_InverseViewMatrix; }
		glm::mat4& GetInverseProjection() { return m_InverseProjectionMatrix; }
		float GetRotation() { return glm::radians(m_Rotation); }

		void SetProjectionMatrix(const glm::mat4 projection)
		{
			m_ProjectionMatrix = projection;
		}

		void SetPerspectiveProjectionMatrix(const float radFov, const float width, const float height, const float nearP, const float farP)
		{
			m_ProjectionMatrix = glm::perspectiveFov(radFov, width, height, farP, nearP);
		}

		void SetOrthoProjectionMatrix(const float width, const float height, const float nearP, const float farP)
		{
			m_ProjectionMatrix = glm::ortho(-width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f, farP, nearP);
		}

		virtual void SetMouseEnabled(bool enabled) { m_MouseEnabled = enabled; }
		virtual void SetKeyboardEnabled(bool enabled) { m_KeyboardEnabled = enabled; }

	protected:
		float m_NearPlane = 0.10f, m_FarPlane = 1000.0f;
		float m_Zoom = 1.0f;
		float m_Rotation = 0.0f;
		float m_RotationSpeed = 0.0002f;
		float m_TravelSpeed = 1.0f;
		float m_MouseSpeed = 1.0f;
		bool m_MouseEnabled = true;
		bool m_KeyboardEnabled = true;
		bool HasMouseMoved;

		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix; 
		glm::mat4 m_ViewProjectionMatrix; 
		glm::mat4 m_InverseViewMatrix;
		glm::mat4 m_InverseProjectionMatrix;
		glm::mat4 m_InverseViewProjectionMatrix;

		// ----------------- TO BE FIXED ---------------------
		// TODO: Automate assignment
		float m_ViewportWidth;// = 2200;
		float m_ViewportHeight;// = 1240;
		float m_AspectRatio;// = float(16.0f / 9.0f);
	};

}
