#pragma once

#include "LkEngine/Scene/Scene.h"

#include "LkEngine/Renderer/UniformBufferSet.h"



namespace LkEngine {

	class Scene;
	class Renderer;
	class Renderer2D;
	class Renderer2DAPI;
	class Pipeline;
	class RenderPass;

	struct SceneRendererCamera
	{
		Ref<LkEngine::Camera> Camera = nullptr;
		glm::mat4 ViewMatrix = glm::mat4(1.0f);
	};

	struct SceneRendererSpecification
	{
		uint32_t ShadowCascades = 4;
	};

    class SceneRenderer : public RefCounted
    {
	public:
		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t Meshes = 0;
			uint32_t Instances = 0;
			float TotalGPUTime = 0.0f;
		};
    public:
		SceneRenderer(Ref<Scene> scene, const SceneRendererSpecification& specification = SceneRendererSpecification());
		~SceneRenderer() = default;

		void Init();
		void Shutdown();

		void BeginScene(const SceneRendererCamera& camera);
		void EndScene();

		Ref<Renderer2DAPI> GetRenderer2D(); 

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
		} CameraDataUB;
	private:
		SceneRendererSpecification m_Specification;
		Ref<Scene> m_Scene = nullptr;

		struct SceneInfo
		{
			SceneRendererCamera SceneCamera;
		} m_SceneData;

		Ref<Pipeline> m_GeometryPipeline;
		Ref<RenderPass> m_GeometryPass;

		Ref<Renderer2DAPI> m_Renderer2D = nullptr;
		Ref<UniformBufferSet> m_UBSCamera;
    };

}