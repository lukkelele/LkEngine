#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/LObjectPtr.h"

#include "LkEngine/Input/Keyboard.h"
#include "LkEngine/Input/Mouse.h"

///
/// TODO:
/// * Need to clean this up
///
///

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>


namespace LkEngine {

	class LCamera : public LObject
	{
	public:
		enum class ProjectionType { Perspective = 0, Orthographic = 1 };
		enum class Type { None = 0, Scene = 1, Editor = 2 };
	public:
		LCamera() = default;
		LCamera(const glm::mat4& projection);
		LCamera(const float degFov, 
				const float width, 
				const float height, 
				const float nearP, 
				const float farP);
		virtual ~LCamera() = default;

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }

		virtual void SetProjection(glm::mat4& proj) 
		{ 
			m_ProjectionMatrix = proj;
		}

		float GetRotation() { return glm::radians(m_Rotation); }
		Type GetType() const { return m_Type; }
		std::string GetTypeStr() const;

		ProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType(ProjectionType type) { m_ProjectionType = type; }

		float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
		float GetPerspectiveFarClip() const { return m_PerspectiveFar; }
		void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNear = nearClip; }
		void SetPerspectiveFarClip(float farClip) { m_PerspectiveFar = farClip; }

		float GetOrthographicSize() const { return m_OrthographicSize; }
		float GetOrthographicNearClip() const { return m_OrthographicNear; }
		float GetOrthographicFarClip() const { return m_OrthographicFar; }
		void SetOrthographicNearClip(float nearClip) { m_OrthographicNear = nearClip; }
		void SetOrthographicFarClip(float farClip) { m_OrthographicFar = farClip; }

		void SetProjectionMatrix(const glm::mat4 projection)
		{
			m_ProjectionMatrix = projection;
		}

		void SetPerspectiveProjectionMatrix(const float radFov, const float width, const float height, const float nearP, const float farP)
		{
			m_ProjectionMatrix = glm::perspectiveFov(radFov, width, height, nearP, farP);
		}

		void SetOrthoProjectionMatrix(const float width, const float height, const float nearP, const float farP)
		{
			m_ProjectionMatrix = glm::ortho(
				-(width  * 0.5f), (width  * 0.5f),
				-(height * 0.5f), (height * 0.5f),
				nearP, farP);
		}

		virtual void SetMouseEnabled(bool enabled) { m_MouseEnabled = enabled; }
		virtual void SetKeyboardEnabled(bool enabled) { m_KeyboardEnabled = enabled; }

	protected:
		ProjectionType m_ProjectionType = ProjectionType::Perspective;
		Type m_Type = Type::None;

		float m_DegPerspectiveFOV = 45.0f;
		float m_PerspectiveNear = 0.1f, m_PerspectiveFar = 1000.0f;
		float m_OrthographicSize = 10.0f;
		float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;

		float m_Rotation = 0.0f;
		float m_RotationSpeed = 0.0002f;
		float m_TravelSpeed = 1.0f;
		float m_MouseSpeed = 1.0f;
		bool m_MouseEnabled = true;
		bool m_KeyboardEnabled = true;

		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);

		friend class SceneSerializer;
	};

}
