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
		Camera() = default;
		virtual ~Camera() = default;

		virtual void OnUpdate(float ts) = 0;
		virtual void UpdateView() = 0;
		virtual void UpdateProjection() = 0;
		virtual void UpdateMousePosition();
		virtual void SetProjection(glm::mat4& proj) { m_Projection = proj; } // FIXME
		virtual void HandleInput(float ts);

		glm::vec3& GetPos() { return m_Pos; }
	    glm::mat4 GetView() const { return m_View; }
	    glm::mat4 GetProjection() const { return m_Projection; }
	    glm::mat4 GetViewProjection() const { return m_ViewProjection; }
		glm::mat4& GetInverseViewProjection() { return m_InverseViewProjection; }
		glm::mat4& GetInverseView() { return m_InverseView; }
		glm::mat4& GetInverseProjection() { return m_InverseProjection; }
		float GetScreenWidth() const { return m_ViewportWidth; }
		float GetScreenHeight() const { return m_ViewportHeight; }
		void SetPos(glm::vec3& pos) { m_Pos = pos; }

		//float GetFOV() const { return m_FOV; }
		float GetNearClip() const { return m_NearPlane; }
		float GetFarClip() const { return m_FarPlane; }
		void SetNearClip(float nearClip) { m_NearPlane = nearClip; }
		void SetFarClip(float farClip) { m_FarPlane = farClip; }
		float GetRotation() { return glm::radians(m_Rotation); }

		bool IsKeyboardEnabled() { return m_KeyboardEnabled; }
		bool IsMouseEnabled() { return m_MouseEnabled; }

		// Temporary! Should use CameraComponent
		uint64_t ID = 0;

	protected:
		glm::vec3 m_Pos = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_PosDelta = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Origin = { 0.0f, 0.0f, 0.0f };
		glm::vec2 m_MouseDelta = { 0.0f, 0.0f };
		glm::vec2 m_MousePos = { 0.0f, 0.0f };
		glm::vec2 m_InitialMousePos{};

		float m_NearPlane = 0.10f, m_FarPlane = 1000.0f;
		float m_Zoom = 1.0f;
		float m_Rotation = 0.0f;
		float m_RotationSpeed = 0.0002f;
		float m_TravelSpeed = 1.0f; // 0.10f;
		float m_MouseSpeed = 1.0f;
		bool m_KeyboardEnabled = true;
		bool m_MouseEnabled = true;

		glm::mat4 m_View;
		glm::mat4 m_Projection; 
		glm::mat4 m_ViewProjection; 
		glm::mat4 m_InverseView;
		glm::mat4 m_InverseProjection;
		glm::mat4 m_InverseViewProjection;

		bool HasMouseMoved;
		// ----------------- TO BE FIXED ---------------------
		// TODO: Automate assignment
		float m_ViewportWidth;// = 2200;
		float m_ViewportHeight;// = 1240;
		float m_AspectRatio;// = float(16.0f / 9.0f);

	};

}
