#pragma once

#include "LkEngine/Renderer/CameraBase.h"
#include "LkEngine/Renderer/Camera.h"


namespace LkEngine {

	class LScene;

	class LSceneCamera : public LCamera
	{
	public:
		LSceneCamera() = default;
		virtual ~LSceneCamera() = default;

		virtual ECameraType GetType() const override { return ECameraType::Scene; }

		void SetViewportSize(const uint32_t InWidth, const uint32_t InHeight);

	protected:
		glm::vec2 MouseDelta = { 0.0f, 0.0f };
		glm::vec2 MousePos = { 0.0f, 0.0f };

	private:
		friend class LEditorLayer;
		friend class LSceneSerializer;
	};

}
