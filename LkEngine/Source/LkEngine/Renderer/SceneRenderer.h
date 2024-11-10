#pragma once

#include "LkEngine/Scene/Scene.h"

#include "LkEngine/Renderer/UniformBufferSet.h"



namespace LkEngine {

	class LScene;
	class LPipeline;
	class LRenderer;
	class LRenderPass;
	class LRenderer2D;
	class LRenderCommandBuffer;

	struct FSceneRendererCamera
	{
		TObjectPtr<LCamera> Camera{};
		glm::mat4 ViewMatrix = glm::mat4(1.0f);
	};

	struct FSceneRendererSpecification
	{
		TObjectPtr<LScene> StartScene = nullptr;
		uint32_t ShadowCascades = 4;
	};

    class LSceneRenderer : public LObject
    {
    public:
		LSceneRenderer(const FSceneRendererSpecification& InSpecification = FSceneRendererSpecification());
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

		void BeginScene(const FSceneRendererCamera& camera);
		void EndScene();

	private:
		void PreRender();

	public:
		struct UBCamera
		{
			/* Projection with near and far inverted. */
			glm::mat4 ViewProjection{};
			glm::mat4 InverseViewProjection{};
			glm::mat4 Projection{};
			glm::mat4 InverseProjection{};
			glm::mat4 View{};
			glm::mat4 InverseView{};
			glm::vec2 NDCToViewMul{};
			glm::vec2 NDCToViewAdd{};
			glm::vec2 DepthUnpackConsts{};
			glm::vec2 CameraTanHalfFOV{};
		};
		UBCamera CameraDataUB;

	private:
		FSceneRendererSpecification Specification;
		TObjectPtr<LScene> Scene = nullptr;

		TObjectPtr<LRenderCommandBuffer> RenderCommandBuffer{};
		TObjectPtr<LRenderer2D> Renderer2D{};

		struct FSceneData
		{
			FSceneRendererCamera SceneCamera;
		};
		FSceneData SceneData;

		TObjectPtr<LPipeline> GeometryPipeline;
		TObjectPtr<LRenderPass> GeometryPass;
		TObjectPtr<LUniformBufferSet> UBSCamera;


		LCLASS(LSceneRenderer)
    };

}