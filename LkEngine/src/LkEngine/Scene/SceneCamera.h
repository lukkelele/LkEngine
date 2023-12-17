#pragma once

//#include "LkEngine/Renderer/PerspectiveCamera.h"
#include "LkEngine/Renderer/Camera.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


// TODO:
// Needs to get updated , big time

namespace LkEngine {

	class Scene;

	class SceneCamera : public Camera
	{
	public:
		SceneCamera();
		~SceneCamera() = default;

		void Update(float ts);
		void UpdateView();
		void UpdateProjection();
	};

}
