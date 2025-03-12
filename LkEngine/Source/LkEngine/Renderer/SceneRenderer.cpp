#include "LKpch.h"
#include "SceneRenderer.h"

#include "LkEngine/Renderer/Mesh.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/Renderer2D.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Asset/AssetManager.h"

#include "LkEngine/Renderer/Backend/OpenGL/OpenGLRenderer.h"

namespace LkEngine {

	LSceneRenderer::LSceneRenderer(TObjectPtr<LScene> InScene, const FSceneRendererSpecification& InSpecification)
		: Scene(InScene)
		, Specification(InSpecification)
	{
		LOBJECT_REGISTER();

		LK_CORE_TRACE_TAG("SceneRenderer", "Creating command buffer");
		CommandBuffer = LRenderCommandBuffer::Create(0, "SceneRenderer");

		/* TODO: Create 2D renderer here. */
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
		Data.ViewProjectionMatrix = Camera.GetViewProjection();
	}

	void LSceneRenderer::EndScene()
	{
		LK_PROFILE_FUNC();

		FlushDrawList();

		StaticMeshDrawList.clear();
		StaticMeshTransformMap.clear();

		MeshDrawList.clear();
		MeshTransformMap.clear();
	}

	void LSceneRenderer::SetScene(TObjectPtr<LScene> InScene)
	{
		Scene = InScene;
	}

	void LSceneRenderer::SubmitStaticMesh(TObjectPtr<LStaticMesh> StaticMesh,
										  TObjectPtr<LMaterialTable> MaterialTable,
										  const glm::mat4& Transform,
										  TObjectPtr<LMaterialAsset> MaterialOverride)
	{
		LK_PROFILE_FUNC();

		LK_CORE_ASSERT(StaticMesh && StaticMesh->MeshSource);
		const TObjectPtr<LMeshSource>& MeshSource = StaticMesh->MeshSource;
		const std::vector<LSubmesh>& Submeshes = MeshSource->Submeshes; 
		LK_CORE_ASSERT(!StaticMesh->Submeshes.empty(), "Submeshes for StaticMesh '{}' is empty", StaticMesh->Handle);

		for (const uint32_t SubmeshIndex : StaticMesh->GetSubmeshes())
		{
			const glm::mat4 SubmeshTransform = Transform * Submeshes[SubmeshIndex].Transform;
			const LSubmesh& Submesh = Submeshes[SubmeshIndex];
			const bool IsRigged = Submesh.bIsRigged;
			const uint32_t MaterialIndex = Submesh.MaterialIndex;

			FAssetHandle MaterialHandle;
			if (MaterialOverride)
			{
				MaterialHandle = MaterialOverride->Handle;
			}
			else
			{
				MaterialHandle = MaterialTable->HasMaterial(MaterialIndex)
					? MaterialTable->GetMaterial(MaterialIndex)
					: LEditorAssetManager::BaseMaterial->Handle;
			}
			TObjectPtr<LMaterialAsset> Material = LAssetManager::GetAsset<LMaterialAsset>(MaterialHandle);
			LK_CORE_VERIFY(Material);

			/* TODO: Replace rigged-bool with EMeshRigged. */
			FMeshKey MeshKey = { StaticMesh->Handle, MaterialHandle, SubmeshIndex, false /* == IsRigged */ };
			FTransformVertexData& TransformStorage = MeshTransformMap[MeshKey].Transforms.emplace_back();

			TransformStorage.MRow[0] = { SubmeshTransform[0][0], SubmeshTransform[1][0], SubmeshTransform[2][0], SubmeshTransform[3][0] };
			TransformStorage.MRow[1] = { SubmeshTransform[0][1], SubmeshTransform[1][1], SubmeshTransform[2][1], SubmeshTransform[3][1] };
			TransformStorage.MRow[2] = { SubmeshTransform[0][2], SubmeshTransform[1][2], SubmeshTransform[2][2], SubmeshTransform[3][2] };

			{
				/// TODO: Drawlist selection.
				const bool IsTransparent = Material->IsTransparent();

				auto& DestDrawList = StaticMeshDrawList;
				auto& DrawCommand = DestDrawList[MeshKey];

				DrawCommand.Mesh = StaticMesh;
				DrawCommand.SubmeshIndex = SubmeshIndex;
				DrawCommand.MaterialTable = MaterialTable;
				if (MaterialOverride)
				{
					DrawCommand.OverrideMaterial = MaterialOverride->GetMaterial();
				}
				DrawCommand.bIsRigged = IsRigged; 
				DrawCommand.InstanceCount++;
			}
		}
	}

	void LSceneRenderer::SubmitMesh(TObjectPtr<LMesh> Mesh, 
									const uint32_t SubmeshIndex, 
									TObjectPtr<LMaterialTable> MaterialTable, 
									const glm::mat4& Transform,
									TObjectPtr<LMaterialAsset> MaterialOverride)
	{
		LK_PROFILE_FUNC();
		LK_CORE_ASSERT(Mesh && Mesh->MeshSource && MaterialTable);

		TObjectPtr<LMeshSource>& MeshSource = Mesh->MeshSource;

		const std::vector<LSubmesh>& Submeshes = MeshSource->Submeshes;
		const LSubmesh& Submesh = Submeshes[SubmeshIndex];
		const bool IsRigged = Submesh.bIsRigged;
		const uint32_t MaterialIndex = Submesh.MaterialIndex;

		FAssetHandle MaterialHandle;
		if (MaterialOverride)
		{
			MaterialHandle = MaterialOverride->Handle;
		}
		else
		{
			MaterialHandle = MaterialTable->HasMaterial(MaterialIndex)
				? MaterialTable->GetMaterial(MaterialIndex)
				: LEditorAssetManager::BaseMaterial->Handle;
		}
		TObjectPtr<LMaterialAsset> Material = LAssetManager::GetAsset<LMaterialAsset>(MaterialHandle);
		LK_CORE_VERIFY(Material);

		/* TODO: Replace rigged-bool with EMeshRigged. */
		FMeshKey MeshKey = { Mesh->Handle, MaterialHandle, SubmeshIndex, false /* == IsRigged */ };
		FTransformVertexData& TransformStorage = MeshTransformMap[MeshKey].Transforms.emplace_back();

		TransformStorage.MRow[0] = { Transform[0][0], Transform[1][0], Transform[2][0], Transform[3][0] };
		TransformStorage.MRow[1] = { Transform[0][1], Transform[1][1], Transform[2][1], Transform[3][1] };
		TransformStorage.MRow[2] = { Transform[0][2], Transform[1][2], Transform[2][2], Transform[3][2] };

		{
			const bool IsTransparent = Material->IsTransparent();
			auto& DestDrawList = MeshDrawList; /// TODO: Drawlist selection.
			auto& DrawCommand = DestDrawList[MeshKey]; 

			DrawCommand.Mesh = Mesh;
			DrawCommand.SubmeshIndex = SubmeshIndex;
			DrawCommand.MaterialTable = MaterialTable;
			if (MaterialOverride)
			{
				DrawCommand.OverrideMaterial = MaterialOverride->GetMaterial();
			}
			DrawCommand.InstanceCount++;
			DrawCommand.bIsRigged = IsRigged; 
		}
	}

	void LSceneRenderer::PreRender()
	{
	}

	void LSceneRenderer::FlushDrawList()
	{
		LK_PROFILE_FUNC();
		LK_CORE_ASSERT(Scene);

		const glm::mat4 ViewProjection = Data.ViewProjectionMatrix;

		FSceneRendererFlushData FlushData{};
		FlushData.StaticMeshDrawListSize = (int)(StaticMeshDrawList.size());
		FlushData.StaticMeshTransformMapSize = (int)(StaticMeshTransformMap.size());
		FlushData.MeshDrawListSize = (int)(MeshDrawList.size());
		FlushData.MeshTransformMapSize = (int)(MeshTransformMap.size());
		OnDrawListFlush.Broadcast(FlushData);

		/* Static Meshes. */
		for (auto& [MeshKey, DrawCommand] : StaticMeshDrawList)
		{
			const FTransformMapData& TransformData = MeshTransformMap.at(MeshKey);

			TObjectPtr<LStaticMesh> StaticMesh = DrawCommand.Mesh;
			LK_CORE_ASSERT(StaticMesh, "DrawCommand is missing a valid LStaticMesh");
			LK_CORE_ASSERT(StaticMesh->MeshSource, "DrawCommand is missing a valid MeshSource (StaticMesh)");
			TObjectPtr<LMeshSource> MeshSource = StaticMesh->MeshSource;

			LK_CORE_ASSERT(MeshSource->Submeshes.size() > MeshKey.SubmeshIndex);
			const uint32_t MaterialIndex = MeshSource->Submeshes[MeshKey.SubmeshIndex].MaterialIndex;

			TObjectPtr<LMaterial> Material;
			if (DrawCommand.OverrideMaterial)
			{
				Material = DrawCommand.OverrideMaterial;
				//LK_CORE_DEBUG("{}, material override: {}", StaticMesh->Name, Material->GetName());
			}
			else
			{
				if (DrawCommand.MaterialTable->HasMaterial(DrawCommand.SubmeshIndex))
				{
					TObjectPtr<LMaterialAsset> MatAsset = LAssetManager::GetAsset<LMaterialAsset>(DrawCommand.MaterialTable->GetMaterial(DrawCommand.SubmeshIndex));
					Material = MatAsset->GetMaterial();
				}
				else
				{
					Material = LEditorAssetManager::BaseMaterial->GetMaterial();
				}
			}
			LK_CORE_VERIFY(Material);

			LK_CORE_ASSERT(Material->GetShader(), "Invalid shader for material: {}", Material->GetName());
			TObjectPtr<LShader> Shader = Material->GetShader();
			LK_CORE_ASSERT(Material->GetTexture(), "Invalid texture for material: {}", Material->GetName());
			TObjectPtr<LTexture2D> Texture = Material->GetTexture();
			LK_CORE_ASSERT(Shader, "DrawCommand is missing a valid shader");
			LK_CORE_ASSERT(Texture, "DrawCommand is missing a valid texture");

			for (const auto& Transform : TransformData.Transforms)
			{
				const glm::mat4 ModelMatrix = glm::transpose(glm::mat4(
					glm::vec4(Transform.MRow[0][0], Transform.MRow[0][1], Transform.MRow[0][2], Transform.MRow[0][3]),
					glm::vec4(Transform.MRow[1][0], Transform.MRow[1][1], Transform.MRow[1][2], Transform.MRow[1][3]),
					glm::vec4(Transform.MRow[2][0], Transform.MRow[2][1], Transform.MRow[2][2], Transform.MRow[2][3]),
					glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
				));

				LRenderer::Submit([ViewProjection, Shader, Texture, MeshSource, Material, ModelMatrix]() mutable
				{
					LRenderer::GetViewportFramebuffer()->Bind();
					LRenderer::SetDepthFunction(EDepthFunction::LessOrEqual);

					/* Bind shader and set uniforms. */
					Shader->Bind();
					//Shader->Set("u_ViewProjection", SceneCamera.GetViewProjection());
					Shader->Set("u_ViewProjection", ViewProjection);
					Shader->Set("u_Model", ModelMatrix);
					uint32_t Slot = 0;
					Shader->Get("u_Texture", Slot);
					Texture->Bind(Slot);

					MeshSource->GetVertexBuffer()->Bind();

					TObjectPtr<LIndexBuffer> IndexBuffer = MeshSource->GetIndexBuffer();
					IndexBuffer->Bind();

					LK_OpenGL_Verify(glDrawElements(GL_TRIANGLES, IndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr));

					LFramebuffer::TargetSwapChain();
				});
			}
		}

		/* Meshes. */
		for (auto& [MeshKey, DrawCommand] : MeshDrawList)
		{
			const FTransformMapData& TransformData = MeshTransformMap.at(MeshKey);

			TObjectPtr<LMesh> Mesh = DrawCommand.Mesh;
			LK_CORE_ASSERT(Mesh, "DrawCommand is missing a valid mesh");
			LK_CORE_ASSERT(Mesh->GetIndexBuffer(), "Pointer to mesh indexbuffer is not valid");
			LK_CORE_ASSERT((Mesh->GetIndexBuffer()->GetCount() > 0), "Invalid mesh indexbuffer count");
			TObjectPtr<LMeshSource> MeshSource = Mesh->MeshSource;

			const uint32_t MaterialIndex = MeshSource->Submeshes[MeshKey.SubmeshIndex].MaterialIndex;

		#if 0
			TObjectPtr<LMaterial> Material = DrawCommand.OverrideMaterial 
				? DrawCommand.OverrideMaterial 
				: LAssetManager::GetAsset<LMaterialAsset>(DrawCommand.MaterialTable->GetMaterial(DrawCommand.SubmeshIndex))->GetMaterial();
		#else
			TObjectPtr<LMaterial> Material = Mesh->GetMaterial(0);
		#endif
			LK_CORE_ASSERT(Material, "DrawCommand is missing a valid material");

			TObjectPtr<LShader> Shader = Material->GetShader();
			TObjectPtr<LTexture2D> Texture = Material->GetTexture();
			LK_CORE_ASSERT(Shader, "DrawCommand is missing a valid shader");
			LK_CORE_ASSERT(Texture, "DrawCommand is missing a valid texture");

			for (const auto& Transform : TransformData.Transforms)
			{
				const glm::mat4 ModelMatrix = glm::transpose(glm::mat4(
					glm::vec4(Transform.MRow[0][0], Transform.MRow[0][1], Transform.MRow[0][2], Transform.MRow[0][3]),
					glm::vec4(Transform.MRow[1][0], Transform.MRow[1][1], Transform.MRow[1][2], Transform.MRow[1][3]),
					glm::vec4(Transform.MRow[2][0], Transform.MRow[2][1], Transform.MRow[2][2], Transform.MRow[2][3]),
					glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
				));

				LRenderer::Submit([ViewProjection, Shader, Texture, MeshSource, Material, ModelMatrix]() mutable
				{
					LRenderer::GetViewportFramebuffer()->Bind();
					LRenderer::SetDepthFunction(EDepthFunction::LessOrEqual);

					/* Bind shader and set uniforms. */
					Shader->Bind();
					Shader->Set("u_ViewProjection", ViewProjection);
					Shader->Set("u_Model", ModelMatrix);

					uint32_t Slot = 0;
					Shader->Get("u_Texture", Slot);
					Texture->Bind(Slot);

					MeshSource->GetVertexBuffer()->Bind();
					TObjectPtr<LIndexBuffer> IndexBuffer = MeshSource->GetIndexBuffer();
					IndexBuffer->Bind();

					LK_OpenGL_Verify(glDrawElements(GL_TRIANGLES, IndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr));

					LFramebuffer::TargetSwapChain();
				});
			}
		}
	}

}
