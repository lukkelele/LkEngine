#include "LKpch.h"
#include "SceneRenderer.h"

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/Renderer2D.h"


namespace LkEngine {

	LSceneRenderer::LSceneRenderer(const FSceneRendererSpecification& InSpecification)
		: Scene(InSpecification.StartScene)
		, Specification(InSpecification)
	{
	}

	void LSceneRenderer::Init()
	{
	}

	void LSceneRenderer::Shutdown()
	{
	}

	void LSceneRenderer::BeginScene(const FSceneRendererCamera& Camera)
	{
		LK_PROFILE_FUNC();
		SceneData.SceneCamera = Camera;
		UBCamera& CameraData = CameraDataUB;

		auto& SceneCamera = SceneData.SceneCamera;
		const auto ViewProjection = SceneCamera.Camera->GetProjectionMatrix() * SceneCamera.ViewMatrix;
		const glm::mat4 ViewInverse = glm::inverse(SceneCamera.ViewMatrix);
		const glm::mat4 ProjectionInverse = glm::inverse(SceneCamera.Camera->GetProjectionMatrix());
		const glm::vec3 CameraPosition = ViewInverse[3];

		CameraData.ViewProjection = ViewProjection;
		CameraData.Projection = SceneCamera.Camera->GetProjectionMatrix();
		CameraData.InverseProjection = ProjectionInverse;
		CameraData.View = SceneCamera.ViewMatrix;
		CameraData.InverseView = ViewInverse;
		CameraData.InverseViewProjection = ViewInverse * CameraData.InverseProjection;

		TObjectPtr<LSceneRenderer> SceneRendererInstance = this;
		LRenderer::Submit([&SceneRendererInstance, CameraData]() mutable
		{
			SceneRendererInstance->UBSCamera->Get()->SetData(&CameraData, sizeof(CameraData));
		});
	}

	void LSceneRenderer::EndScene()
	{
	}

	void LSceneRenderer::PreRender()
	{
	}

#if 0
	TObjectPtr<LRenderer2D> LSceneRenderer::GetRenderer2D()
	{ 
		return Renderer2D; 
	}
#endif

}