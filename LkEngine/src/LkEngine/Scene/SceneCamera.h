#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "LkEngine/Renderer/Camera.h"
#include "LkEngine/Input/Keyboard.h"
#include "LkEngine/Input/Mouse.h"


namespace LkEngine {

	class Scene;

	class SceneCamera : public Camera
	{
	public:
		SceneCamera(float FOV, float nearPlane, float farPlane);
		~SceneCamera() = default;

		void OnUpdate(float ts) override;
		void OnImGuiRender() override;

	public:
		float m_TravelSpeed = 0.10f;
		float m_Distance = 500.0f;
	};

}
