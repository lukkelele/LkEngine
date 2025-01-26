#include "LKpch.h"
#include "OpenGLRenderer.h"

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/Renderer2D.h"

#include "LkEngine/Asset/AssetManager.h"

#include "LkEngine/Core/Window.h"


namespace LkEngine {

	namespace 
	{
		constexpr int MaxArrayTextures = 10;
		constexpr int MaxTexturesPerArrayTexture = 32;

		std::array<TObjectPtr<LOpenGLArrayTexture>, MaxArrayTextures> ArrayTextures;
		int ArrayTextureCount = 0;

		FRendererCapabilities RendererCapabilities{};

		/**
		 * Map ERenderTopology to its corresponding GL value.
		 */
		const std::unordered_map<ERenderTopology, int> RenderTopologyOpenGLMap = {
			{ ERenderTopology::Lines,     GL_LINES     },
			{ ERenderTopology::Triangles, GL_TRIANGLES },
		};
	}

	LOpenGLRenderer::LOpenGLRenderer()
	{
		LOBJECT_REGISTER();
	}

	void LOpenGLRenderer::Initialize()
	{
		LK_CORE_DEBUG_TAG("OpenGLRenderer", "Initializing");

		RenderContext = LWindow::Get().GetRenderContext().As<LOpenGLContext>();
	}

	void LOpenGLRenderer::Destroy()
	{
		if (GeometricPass)
		{
			GeometricPass->Terminate();
			GeometricPass.Release();
		}
	}

	void LOpenGLRenderer::BeginFrame()
	{
		Clear();
	}

	void LOpenGLRenderer::EndFrame()
	{
	}

	void LOpenGLRenderer::Clear()
	{
		LK_OpenGL_Verify(glClearColor(
			LRenderer::ClearColor.r, 
			LRenderer::ClearColor.g,
			LRenderer::ClearColor.b,
			LRenderer::ClearColor.a)
		);
		LK_OpenGL_Verify(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}

	void LOpenGLRenderer::SetPrimitiveTopology(const ERenderTopology InRenderTopology)
	{
		LK_CORE_ASSERT(RenderTopologyOpenGLMap.contains(InRenderTopology));
		TopologyGL = RenderTopologyOpenGLMap.at(InRenderTopology);
		LK_CORE_TRACE_TAG("OpenGLRenderer", "Set topology: {}", Enum::ToString(InRenderTopology));
	}

	void LOpenGLRenderer::SetDepthFunction(const EDepthFunction InDepthFunction)
	{
		RenderContext->SetDepthFunction(InDepthFunction);
	}

	void LOpenGLRenderer::SetDepthEnabled(const bool Enabled)
	{
		RenderContext->SetDepthEnabled(Enabled);
	}

	void LOpenGLRenderer::Draw(LVertexBuffer& VertexBuffer, const LShader& Shader)
	{
		Shader.Bind();
		LK_OpenGL_Verify(glDrawElements(TopologyGL, VertexBuffer.GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr));
	}

	void LOpenGLRenderer::Draw(const LVertexBuffer& VertexBuffer, const LIndexBuffer& IndexBuffer, const LShader& Shader)
	{
		LK_OpenGL_Verify(glDrawElements(TopologyGL, IndexBuffer.GetCount(), GL_UNSIGNED_INT, nullptr));
	}

	void LOpenGLRenderer::SubmitMesh(TObjectPtr<LMesh>& Mesh, TObjectPtr<LShader>& Shader, const glm::mat4& Transform)
	{
		const uint8_t Topology = TopologyGL;
		LRenderer::Submit([&Mesh, &Shader, &Transform, Topology]
		{
			LMeshSource& MeshSource = *Mesh->GetMeshSource();
			LVertexBuffer& VertexBuffer = *MeshSource.GetVertexBuffer();
			LIndexBuffer& IndexBuffer = *VertexBuffer.GetIndexBuffer();

			Shader->Bind();

			VertexBuffer.Bind();
			LK_OpenGL_Verify(glDrawElements(Topology, IndexBuffer.GetCount(), GL_UNSIGNED_INT, nullptr));
		});
	}

	void LOpenGLRenderer::SubmitImage(const TObjectPtr<LImage> image)
	{
		//Renderer2D->DrawImage(image);
	}

	void LOpenGLRenderer::SubmitImage(const TObjectPtr<LImage2D> image)
	{
		//Renderer2D->DrawImage(image);
	}

	void LOpenGLRenderer::SubmitLine(const glm::vec2& p1, const glm::vec2& p2, const glm::vec4& Color, uint64_t EntityID)
	{
		//Renderer2D->DrawLine(p1, p2, Color, EntityID);
	}

	void LOpenGLRenderer::SubmitIndexed(const uint32_t IndexCount)
	{
		LK_OpenGL_Verify(glDrawElements(TopologyGL, IndexCount, GL_UNSIGNED_INT, nullptr));
	}

	void LOpenGLRenderer::DrawIndexed(const uint64_t IndexCount)
	{
		LK_OpenGL_Verify(glDrawElements(TopologyGL, static_cast<GLsizei>(IndexCount), GL_UNSIGNED_INT, nullptr));
	}

	void LOpenGLRenderer::SubmitQuad(const glm::vec2& Pos, const glm::vec2& Size, const glm::vec4& Color, uint64_t EntityID)
	{
		//Renderer2D->DrawQuad({ Pos.x, Pos.y, 0.0f }, Size, Color, EntityID);
	}

	void LOpenGLRenderer::SubmitQuad(const glm::vec3& Pos, const glm::vec2& Size, const glm::vec4& Color, uint64_t EntityID)
	{
		//Renderer2D->DrawQuad(Pos, Size, Color, EntityID);
	}

	void LOpenGLRenderer::SubmitQuad(const glm::vec2& Pos, const glm::vec2& Size, const glm::vec4& Color, float Rotation, uint64_t EntityID)
	{
		//Renderer2D->DrawQuad({ Pos.x, Pos.y, 0.0f }, Size, Color, Rotation, EntityID);
	}

	void LOpenGLRenderer::SubmitQuad(const glm::vec3& Pos, const glm::vec2& Size, const glm::vec4& Color, float Rotation, uint64_t EntityID)
	{
		//Renderer2D->DrawQuad(Pos, Size, Color, Rotation, EntityID);
	}

	void LOpenGLRenderer::SubmitQuad(const glm::vec2& Pos, const glm::vec2& Size, TObjectPtr<LTexture> Texture,
									 const float Rotation, uint64_t EntityID)
	{
		//Renderer2D->DrawQuad({ Pos.x, Pos.y, 0.0f }, Size, Texture, Rotation, EntityID);
	}

	void LOpenGLRenderer::SubmitQuad(const glm::vec2& Pos, const glm::vec2& Size, TObjectPtr<LTexture> Texture,
									 const glm::vec4& tintColor, const float Rotation, uint64_t EntityID)
	{
		//Renderer2D->DrawQuad({ Pos.x, Pos.y, 0.0f }, Size, Texture, tintColor, Rotation, EntityID);
	}

	void LOpenGLRenderer::SubmitQuad(const glm::vec3& Pos, const glm::vec2& Size, TObjectPtr<LTexture> Texture,
									 const glm::vec4& tintColor, float Rotation, uint64_t EntityID)
	{
		//Renderer2D->DrawQuad(Pos, Size, Texture, tintColor, Rotation, EntityID);
	}

	void LOpenGLRenderer::SubmitQuad(const glm::vec3& Pos, const glm::vec2& Size,
									 TObjectPtr<LTexture> Texture, float Rotation, uint64_t EntityID)
	{
		// Renderer2D->DrawQuad(Pos, Size, Texture, Rotation, EntityID);
	}

	FRendererCapabilities& LOpenGLRenderer::GetCapabilities()
	{
		return RendererCapabilities;
	}

	void LOpenGLRenderer::RenderGeometry(TObjectPtr<LRenderCommandBuffer> InRenderCommandBuffer,
										 TObjectPtr<LPipeline> InPipeline,
										 TObjectPtr<LVertexBuffer> InVertexBuffer,
										 TObjectPtr<LIndexBuffer> InIndexBuffer,
										 const glm::mat4& InTransform,
										 const uint32_t IndexCount)
	{
	}

	void LOpenGLRenderer::RenderGeometry(TObjectPtr<LRenderCommandBuffer> RenderCommandBuffer,
										 TObjectPtr<LPipeline> Pipeline,
										 TObjectPtr<LMaterial> Material,
										 TObjectPtr<LVertexBuffer> VertexBuffer,
										 TObjectPtr<LIndexBuffer> IndexBuffer,
										 const glm::mat4& Transform,
										 const uint32_t IndexCount /* == 0*/)
	{
		LK_CORE_ASSERT(false, "Not implemented");
	}

	void LOpenGLRenderer::RenderGeometry(TObjectPtr<LRenderCommandBuffer> RenderCommandBuffer,
										 TObjectPtr<LPipeline> InPipeline,
										 TObjectPtr<LShader>& Shader,
										 TObjectPtr<LVertexBuffer>& VertexBuffer,
										 TObjectPtr<LIndexBuffer>& IndexBuffer,
										 const glm::mat4& Transform,
										 const uint32_t IndexCount /* == 0*/)
	{
		LK_CORE_ASSERT(false, "RENDER GEOMETRY DISABLED :)");
		std::deque<LRendererID> BoundArrayTextures = InPipeline.As<LOpenGLPipeline>()->GetBoundArrayTextures();
		TObjectPtr<LFramebuffer> Framebuffer = LRenderer::GetViewportFramebuffer();

		/* TODO: Dont submit by & here. */
		LRenderer::Submit([&, Framebuffer, Shader, VertexBuffer,
						   Transform, IndexCount, BoundArrayTextures]() mutable
		{
			Framebuffer->Bind();
			Shader->Bind();

			int ArrIndex = 1;
			for (const LRendererID ArrayTexture : BoundArrayTextures)
			{
				Shader->Set("u_ArrayTexture" + std::to_string(ArrIndex++), ArrayTexture);
			}

			VertexBuffer->Bind();
			DrawIndexed(IndexCount);

			Framebuffer->Unbind();
		});
	}

	void LOpenGLRenderer::BeginRenderPass(TObjectPtr<LRenderCommandBuffer> RenderCommandBuffer,
										  TObjectPtr<LRenderPass> InRenderPass,
										  const bool ExplicitClear)
	{
	}

	void LOpenGLRenderer::EndRenderPass(TObjectPtr<LRenderCommandBuffer> RenderCommandBuffer)
	{
		LK_UNUSED(RenderCommandBuffer);
	}

	TObjectPtr<LOpenGLArrayTexture> LOpenGLRenderer::GetArrayTexture(const int Idx)
	{
		LK_CORE_ASSERT((Idx >= 0) && (Idx < ArrayTextures.size()) && ArrayTextures.at(Idx));
		return ArrayTextures.at(Idx);
	}

	void LOpenGLRenderer::BindArrayTexture(const uint8_t Idx)
	{
		LK_CORE_ASSERT((Idx < ArrayTextures.size()) && ArrayTextures.at(Idx));
		LArrayTexture& ArrayTexture = *ArrayTextures.at(Idx);
		ArrayTexture.Bind();
	}

	void LOpenGLRenderer::BindArrayTexture(const EArrayTextureDimension Dimension)
	{
		LOpenGLArrayTexture& ArrayTexture = *GetArrayTextureWithDimension(Dimension);
		ArrayTexture.Bind();
	}

	TObjectPtr<LOpenGLArrayTexture> LOpenGLRenderer::GetArrayTextureWithDimension(const EArrayTextureDimension ArrayTextureDimension)
	{
		for (const TObjectPtr<LOpenGLArrayTexture>& ArrayTexture : ArrayTextures)
		{
			if (ArrayTexture->Specification.Dimension == ArrayTextureDimension)
			{
				return ArrayTexture;
			}
		}

		LK_CORE_ASSERT(false, "Failed to find ArrayTexture with dimension: {}", Enum::ToString(ArrayTextureDimension));
		return nullptr;
	}

}
