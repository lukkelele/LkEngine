#pragma once

//#include "LkEngine/Renderer/PerspectiveCamera.h"
#include "LkEngine/Renderer/Camera.h"
#include "LkEngine/Input/Keyboard.h"
#include "LkEngine/Input/Mouse.h"


namespace LkEngine {

	class EditorCamera : public Camera
	{
	public:
		void SetPerspective(float verticalFOV, float nearClip = 0.1f, float farClip = 1000.0f);
		void SetOrthographic(float width, float height, float nearClip = -1.0f, float farClip = 1.0f);
		void SetViewportSize(uint32_t width, uint32_t height);

		void Update(float ts) override;
		void UpdateView();
		void UpdateProjection();
		void UpdateViewProjection();

		glm::vec3& GetPos() { return m_Pos; }
		glm::vec3 GetPos() const { return m_Pos; }
		glm::vec3 GetPosWithOffset();
	    glm::mat4 GetView() const { return m_ViewMatrix; }
	    glm::mat4 GetProjection() const { return m_ProjectionMatrix; }
	    glm::mat4 GetViewProjection() const { return m_ViewProjectionMatrix; }
		glm::mat4& GetInverseViewProjection() { return m_InverseViewProjectionMatrix; }
		glm::mat4& GetInverseView() { return m_InverseViewMatrix; }
		glm::mat4& GetInverseProjection() { return m_InverseProjectionMatrix; }
		void SetPos(const glm::vec3& pos);

		void SetOffset(const glm::vec2& offset);
		void SetOffset(const glm::vec3& offset);
		glm::vec3 GetOffset() const { return m_Offset; }
		glm::vec3& GetOffset() { return m_Offset; }

	private:
		ProjectionType m_ProjectionType = ProjectionType::Perspective;

		float m_DegPerspectiveFOV = 45.0f;
		float m_PerspectiveNear = 0.1f, m_PerspectiveFar = 1000.0f;
		float m_OrthographicSize = 10.0f;
		float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;

		glm::vec3 m_Pos = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_PosDelta = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Origin = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Offset = { 0.0f, 0.0f, 0.0f };
		glm::vec2 m_MouseDelta = { 0.0f, 0.0f };
		glm::vec2 m_MousePos = { 0.0f, 0.0f };
		glm::vec2 m_InitialMousePos{};
	};

}
