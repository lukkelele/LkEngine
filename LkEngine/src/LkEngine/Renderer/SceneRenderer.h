#pragma once

#include "LkEngine/Core/Base.h"
#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

	// Forward declaration


    class SceneRenderer
    {
	public:
		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t Meshes = 0;
			uint32_t Instances = 0;
			float TotalGPUTime = 0.0f;
		};

		struct SceneRendererSpecification
		{
			uint32_t ShadowCascades = 4;
		};
    public:
		SceneRenderer(s_ptr<Scene> scene, const SceneRendererSpecification& specification = SceneRendererSpecification());
		~SceneRenderer() = default;

		void BeginScene(s_ptr<SceneCamera> camera);
		void EndScene();
		s_ptr<Renderer2D> GetRenderer2D() { return m_Renderer2D; }

	private:
		SceneRendererSpecification m_Specification;
		s_ptr<Renderer2D> m_Renderer2D = nullptr;

    };

}