#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "LkEngine/Renderer/PerspectiveCamera.h"
#include "LkEngine/Input/Keyboard.h"
#include "LkEngine/Input/Mouse.h"

//
// ------- NOTE ----------
// Needs to be updated, might remove this entirely
// and just use the CameraComponent.
//

namespace LkEngine {

	class Scene;

	class SceneCamera : public Camera
	{
	public:
		//SceneCamera(float FOV, float nearPlane, float farPlane);
		SceneCamera();
		~SceneCamera() = default;

		void OnUpdate(float ts);
		void UpdateView();
		void UpdateProjection();
	};

}
