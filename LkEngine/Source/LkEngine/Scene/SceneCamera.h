#pragma once

#include "LkEngine/Renderer/Camera.h"


namespace LkEngine {

	class Scene;

	class SceneCamera : public Camera
	{
	public:
		void SetPerspective(float verticalFOV, float nearClip = 0.1f, float farClip = 1000.0f);
		void SetOrthographic(float width, float height, float nearClip = -1.0f, float farClip = 1.0f);
		void SetViewportSize(uint32_t width, uint32_t height);

		void Update(float ts);
		void UpdateView();
		void UpdateProjection();
		void UpdateViewProjection();

		// Perspective
		void SetDegPerspectiveVerticalFOV(const float degVerticalFov) { m_DegPerspectiveFOV = degVerticalFov; }
		void SetRadPerspectiveVerticalFOV(const float degVerticalFov) { m_DegPerspectiveFOV = glm::degrees(degVerticalFov); }
		float GetDegPerspectiveVerticalFOV() const { return m_DegPerspectiveFOV; }
		float GetRadPerspectiveVerticalFOV() const { return glm::radians(m_DegPerspectiveFOV); }
		void SetPerspectiveNearClip(const float nearClip) { m_PerspectiveNear = nearClip; }
		float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
		void SetPerspectiveFarClip(const float farClip) { m_PerspectiveFar = farClip; }
		float GetPerspectiveFarClip() const { return m_PerspectiveFar; }

		// Orthographic
		void SetOrthographicSize(const float size) { m_OrthographicSize = size; }
		float GetOrthographicSize() const { return m_OrthographicSize; }
		void SetOrthographicNearClip(const float nearClip) { m_OrthographicNear = nearClip; }
		float GetOrthographicNearClip() const { return m_OrthographicNear; }
		void SetOrthographicFarClip(const float farClip) { m_OrthographicFar = farClip; }
		float GetOrthographicFarClip() const { return m_OrthographicFar; }

		glm::vec3& GetPos() { return m_Pos; }
		glm::vec3 GetPos() const { return m_Pos; }

		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

	private:
		glm::vec3 m_Pos = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_PosDelta = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Origin = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Offset = { 0.0f, 0.0f, 0.0f };
		glm::vec2 m_MouseDelta = { 0.0f, 0.0f };
		glm::vec2 m_MousePos = { 0.0f, 0.0f };
		glm::vec2 m_InitialMousePos{};

		float m_OrthographicSize = 10.0f;
		float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;

		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewProjectionMatrix = glm::mat4(1.0f); 
	};

}
