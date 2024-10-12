#pragma once

#include "LkEngine/Scene/Scene.h"

#include "LkEngine/Renderer/UniformBufferSet.h"



namespace LkEngine {

	class LScene;
	class LRenderer;
	class LRenderer2D;
	class IRenderer2DAPI;
	class LPipeline;
	class LRenderPass;

	struct SceneRendererCamera
	{
		TObjectPtr<LCamera> Camera{};
		glm::mat4 ViewMatrix = glm::mat4(1.0f);
	};

	struct SceneRendererSpecification
	{
		uint32_t ShadowCascades = 4;
	};

    class LSceneRenderer : public LObject
    {
    public:
		LSceneRenderer(TObjectPtr<LScene> scene, 
					   const SceneRendererSpecification& InSpecification = SceneRendererSpecification());
		~LSceneRenderer() = default;

		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t Meshes = 0;
			uint32_t Instances = 0;
			float TotalGPUTime = 0.0f;
		};

		void Init();
		void Shutdown();

		void BeginScene(const SceneRendererCamera& camera);
		void EndScene();

	public:
		struct UBCamera
		{
			// Projection with near and far inverted
			glm::mat4 ViewProjection;
			glm::mat4 InverseViewProjection;
			glm::mat4 Projection;
			glm::mat4 InverseProjection;
			glm::mat4 View;
			glm::mat4 InverseView;
			glm::vec2 NDCToViewMul;
			glm::vec2 NDCToViewAdd;
			glm::vec2 DepthUnpackConsts;
			glm::vec2 CameraTanHalfFOV;
		};
		UBCamera CameraDataUB;

	private:
		SceneRendererSpecification m_Specification;
		TObjectPtr<LScene> m_Scene = nullptr;

		struct FSceneData
		{
			SceneRendererCamera SceneCamera;
		};
		FSceneData SceneData;

		TObjectPtr<LPipeline> m_GeometryPipeline;
		TObjectPtr<LRenderPass> m_GeometryPass;

		TObjectPtr<LUniformBufferSet> m_UBSCamera;

		LCLASS(LSceneRenderer)
    };

}