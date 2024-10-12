#include "LKpch.h"
#include "LkEngine/Renderer/SceneRenderer.h"

#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

	LSceneRenderer::LSceneRenderer(TObjectPtr<LScene> InScene, 
								   const SceneRendererSpecification& InSpecification)
		: m_Scene(InScene)
		, m_Specification(InSpecification)
	{
	}

	void LSceneRenderer::Init()
	{
	}

	void LSceneRenderer::Shutdown()
	{
	}

	void LSceneRenderer::BeginScene(const SceneRendererCamera& camera)
	{
		SceneData.SceneCamera = camera;

		UBCamera& cameraData = CameraDataUB;

		auto& sceneCamera = SceneData.SceneCamera;
		//const auto viewProjection = sceneCamera.Camera->GetProjectionMatrix() * sceneCamera.ViewMatrix;
		const auto viewProjection = sceneCamera.Camera->GetProjectionMatrix() * sceneCamera.ViewMatrix;
		const glm::mat4 viewInverse = glm::inverse(sceneCamera.ViewMatrix);
		const glm::mat4 projectionInverse = glm::inverse(sceneCamera.Camera->GetProjectionMatrix());
		const glm::vec3 cameraPosition = viewInverse[3];

		cameraData.ViewProjection = viewProjection;
		cameraData.Projection = sceneCamera.Camera->GetProjectionMatrix();
		//cameraData.Projection = sceneCamera.Camera->GetProjectionMatrix();
		cameraData.InverseProjection = projectionInverse;
		cameraData.View = sceneCamera.ViewMatrix;
		cameraData.InverseView = viewInverse;
		cameraData.InverseViewProjection = viewInverse * cameraData.InverseProjection;

		TObjectPtr<LSceneRenderer> SceneRendererInstance = this;
		LRenderer::Submit([&SceneRendererInstance, cameraData]() mutable
		{
			SceneRendererInstance->m_UBSCamera->Get()->SetData(&cameraData, sizeof(cameraData));
		});
	}

	void LSceneRenderer::EndScene()
	{
	}

#if 0
	Ref<Renderer2DAPI> LSceneRenderer::GetRenderer2D()
	{ 
		return m_Renderer2D; 
	}
#endif

}