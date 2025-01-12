#include "LKpch.h"
#include "SceneRenderer.h"

#include "LkEngine/Renderer/Mesh.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/Renderer2D.h"

#include "LkEngine/Asset/AssetManager.h"

#include "LkEngine/Renderer/Backend/OpenGL/OpenGLRenderer.h"

#include "LkEngine/Editor/EditorLayer.h" /// REMOVE ME

namespace LkEngine {

	LSceneRenderer::LSceneRenderer(TObjectPtr<LScene> InScene, const FSceneRendererSpecification& InSpecification)
		: Scene(InScene)
		, Specification(InSpecification)
	{
		LOBJECT_REGISTER();

		LK_CORE_DEBUG_TAG("SceneRenderer", "Creating command buffer");
		CommandBuffer = LRenderCommandBuffer::Create(0, "SceneRenderer");

		/* TODO: Create 2D renderer. */
		/* ........... */
	}

	void LSceneRenderer::Initialize()
	{
	}

	void LSceneRenderer::Destroy()
	{
	}

	void LSceneRenderer::BeginScene(const LSceneCamera& Camera)
	{
		LK_PROFILE_FUNC();
	}

	void LSceneRenderer::EndScene()
	{
		LK_PROFILE_FUNC();

		FlushDrawList();

		DrawList.clear();
		MeshTransformMap.clear();
	}

	/* TODO: Evaluate if this should be ran on every tick or not. */
	void LSceneRenderer::SetScene(TObjectPtr<LScene> InScene)
	{
		//LK_CORE_DEBUG_TAG("SceneRenderer", "Set scene to: {}", InScene ? InScene->GetName() : "None");
		Scene = InScene;
	}

	void LSceneRenderer::SubmitMesh(TObjectPtr<LMesh> Mesh, 
									const uint32_t SubmeshIndex, 
									TObjectPtr<LMaterialTable> MaterialTable, 
									const glm::mat4& Transform)
	{
		LK_PROFILE_FUNC();
		//LK_CORE_DEBUG_TAG("SceneRenderer", "Submitting mesh: {}", Mesh->Handle);

		const auto MeshSource = Mesh->GetMeshSource();
		const auto& Submeshes = MeshSource->GetSubmeshes();
		const LSubmesh& Submesh = Submeshes[SubmeshIndex];
		const bool IsRigged = Submesh.bIsRigged;
		const uint32_t MaterialIndex = Submesh.MaterialIndex;

		const LUUID MaterialHandle = MaterialTable->HasMaterial(MaterialIndex) 
			? MaterialTable->GetMaterial(MaterialIndex) 
			: Mesh->GetMaterialTable()->GetMaterial(MaterialIndex);

		TObjectPtr<LMaterialAsset> Material = LAssetManager::GetAsset<LMaterialAsset>(MaterialHandle);

		/* TODO: Replace rigged-bool with EMeshRigged. */
		FMeshKey MeshKey = { Mesh->Handle, MaterialHandle, SubmeshIndex, false /* == IsRigged */ };
		FTransformVertexData& TransformStorage = MeshTransformMap[MeshKey].Transforms.emplace_back();

		TransformStorage.MRow[0] = { Transform[0][0], Transform[1][0], Transform[2][0], Transform[3][0] };
		TransformStorage.MRow[1] = { Transform[0][1], Transform[1][1], Transform[2][1], Transform[3][1] };
		TransformStorage.MRow[2] = { Transform[0][2], Transform[1][2], Transform[2][2], Transform[3][2] };

		{
			bool IsTransparent = Material->IsTransparent();
			/// FIXME
			//auto& DestDrawList = !IsTransparent ? DrawList : DrawList; /* FIXME */
			auto& DestDrawList = DrawList;
			FDrawCommand& DrawCommand = DestDrawList[MeshKey];
			DrawCommand.Mesh = Mesh;
			DrawCommand.SubmeshIndex = SubmeshIndex;
			DrawCommand.MaterialTable = MaterialTable;
			DrawCommand.OverrideMaterial = nullptr; /* FIXME */
			DrawCommand.InstanceCount++;
			DrawCommand.bIsRigged = IsRigged; 
		}

	#if 0
		TObjectPtr<LMaterialTable> MaterialTable = Mesh->GetMaterials();
		if (MaterialTable->HasMaterial(0))
		{
			TObjectPtr<LMaterial> Material = Mesh->GetMaterial(0);
			TObjectPtr<LTexture2D> Texture = Material->GetTexture();
			TObjectPtr<LShader> Shader = Material->GetShader();
			LK_CORE_VERIFY(Material && Texture && Shader, "Failed to submit mesh: {}", Mesh->Handle);
			Shader->Bind();

			glm::mat4 Model = Transform;
			Shader->Set("u_Model", Model);
			Shader->Set("u_ViewProjection", Data.SceneCamera.Camera->GetViewProjection());
			Shader->Set("u_Texture0", 0);
			Texture->Bind(0);
			LK_CORE_ASSERT((Mesh->GetIndexBuffer()->GetCount() > 0), "IndexBuffer count not valid");

			Mesh->GetVertexBuffer()->Bind();
			//LK_CORE_INFO("DRAWING {}", Entity.Name());
			LK_OpenGL_Verify(glDrawElements(GL_TRIANGLES, Mesh->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr));
		}
	#endif
	}

	void LSceneRenderer::PreRender()
	{
	}

	void LSceneRenderer::FlushDrawList()
	{
		LK_PROFILE_FUNC();
		LK_CORE_ASSERT(Scene);
		/* FIXME: Use 'GetMainCamera' here. */
		//TObjectPtr<LSceneCamera> SceneCamera = Scene->GetMainCamera();
		TObjectPtr<LSceneCamera> SceneCamera = LEditorLayer::Get()->GetEditorCamera();
		LK_CORE_ASSERT(SceneCamera, "Scene camera is not valid");

		for (auto& [MeshKey, DrawCommand] : DrawList)
		{
			const FTransformMapData& TransformData = MeshTransformMap.at(MeshKey);

			TObjectPtr<LMesh> Mesh = DrawCommand.Mesh;
			LK_CORE_VERIFY(Mesh, "DrawCommand is missing a valid mesh");
			LK_CORE_ASSERT((Mesh->GetIndexBuffer()->GetCount() > 0), "Invalid mesh indexbuffer count");

		#if 0
			TObjectPtr<LMaterial> Material = DrawCommand.OverrideMaterial 
				? DrawCommand.OverrideMaterial 
				: LAssetManager::GetAsset<LMaterial>(DrawCommand.MaterialTable->GetMaterial(DrawCommand.SubmeshIndex));
		#else
			TObjectPtr<LMaterial> Material = Mesh->GetMaterial(0);
		#endif
											  
			LK_CORE_VERIFY(Material, "DrawCommand is missing a valid material.");

			TObjectPtr<LShader> Shader = Material->GetShader();
			TObjectPtr<LTexture2D> Texture = Material->GetTexture();
			LK_CORE_VERIFY(Shader, "DrawCommand is missing a valid shader");
			LK_CORE_VERIFY(Texture, "DrawCommand is missing a valid texture");

			for (const auto& Transform : TransformData.Transforms)
			{
				const glm::mat4 ModelMatrix = glm::transpose(glm::mat4(
					glm::vec4(Transform.MRow[0][0], Transform.MRow[0][1], Transform.MRow[0][2], Transform.MRow[0][3]),
					glm::vec4(Transform.MRow[1][0], Transform.MRow[1][1], Transform.MRow[1][2], Transform.MRow[1][3]),
					glm::vec4(Transform.MRow[2][0], Transform.MRow[2][1], Transform.MRow[2][2], Transform.MRow[2][3]),
					glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
				));

				LRenderer::Submit([SceneCamera, Shader, Texture, Mesh, Material, ModelMatrix]() mutable
				{
					LRenderer::GetViewportFramebuffer()->Bind();
					LRenderer::SetDepthFunction(EDepthFunction::LessOrEqual);

					/* Bind shader and set uniforms. */
					Shader->Bind();
					Shader->Set("u_ViewProjection", SceneCamera->GetViewProjection());
					Shader->Set("u_Model", ModelMatrix);
					Shader->Set("u_Texture0", 0);
					Texture->Bind(0);

					Mesh->GetVertexBuffer()->Bind();
					Mesh->GetIndexBuffer()->Bind();
					//LK_CORE_WARN_TAG("SceneRenderer", "Drawing: {}", Mesh->Handle);
					LRenderer::OnMeshSubmission.Broadcast(Mesh, Shader, Material);
					LK_OpenGL_Verify(glDrawElements(GL_TRIANGLES, Mesh->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr));

					LFramebuffer::TargetSwapChain();
				});
			}
		}
	}

}
