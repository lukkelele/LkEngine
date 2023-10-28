#pragma once
#include "LkEngine/Renderer/PerspectiveCamera.h"
#include "LkEngine/Input/Keyboard.h"
#include "LkEngine/Input/Mouse.h"

namespace LkEngine {

	// Basically just a 3D camera right now
	class EditorCamera : public Camera
	{
	public:
		EditorCamera();
		EditorCamera(float FOV, float nearPlane, float farPlane);
		~EditorCamera() = default;

		void Update(float ts);

		void UpdateView();
		void UpdateProjection();
	};

}
