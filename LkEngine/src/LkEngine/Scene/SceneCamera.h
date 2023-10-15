#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "LkEngine/Renderer/PerspectiveCamera.h"
#include "LkEngine/Input/Keyboard.h"
#include "LkEngine/Input/Mouse.h"

// TODO:
// Needs to get updated , big time


namespace LkEngine {

	class Scene;

	class SceneCamera : public Camera
	{
	public:
		SceneCamera();
		~SceneCamera() = default;

		void OnUpdate(float ts);
		void UpdateView();
		void UpdateProjection();
	};

}
