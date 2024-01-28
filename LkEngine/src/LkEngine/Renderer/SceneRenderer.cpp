#include "LKpch.h"
#include "LkEngine/Renderer/SceneRenderer.h"


#include "LkEngine/Renderer/Renderer.h"


namespace LkEngine {

	SceneRenderer::SceneRenderer(Ref<Scene> scene, const SceneRendererSpecification& specification)
		: m_Scene(scene)
		, m_Specification(specification)
	{
	}

	void SceneRenderer::Init()
	{
	}

	void SceneRenderer::Shutdown()
	{
	}

	void SceneRenderer::BeginScene(const SceneRendererCamera& camera)
	{
		m_SceneData.SceneCamera = camera;

		UBCamera& cameraData = CameraDataUB;

		auto& sceneCamera = m_SceneData.SceneCamera;
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


		Ref<SceneRenderer> instance = this;
		Renderer::Submit([instance, cameraData]() mutable
		{
			instance->m_UBSCamera->Get()->SetData(&cameraData, sizeof(cameraData));
		});
	}

	void SceneRenderer::EndScene()
	{
	}

	Ref<Renderer2DAPI> SceneRenderer::GetRenderer2D()
	{ 
		return m_Renderer2D; 
	}

}