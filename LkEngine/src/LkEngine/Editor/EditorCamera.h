#pragma once
#include "LkEngine/Renderer/PerspectiveCamera.h"
#include "LkEngine/Input/Keyboard.h"
#include "LkEngine/Input/Mouse.h"

namespace LkEngine {

	class EditorCamera : public PerspectiveCamera
	{
	public:
		EditorCamera(float FOV, float nearPlane, float farPlane);
		~EditorCamera() = default;

		void OnUpdate(float ts);
	};

}
