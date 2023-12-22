#include "LKpch.h"
#include "LkEngine/Renderer/SceneRenderer.h"


namespace LkEngine {

	SceneRenderer::SceneRenderer(s_ptr<Scene> scene, const SceneRendererSpecification& specification)
		: m_Specification(specification)
	{
	}

	void SceneRenderer::BeginScene(s_ptr<SceneCamera> camera)
	{
	}

	void SceneRenderer::EndScene()
	{
	}

}