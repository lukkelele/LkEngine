#include "LKpch.h"
#include "OpenGLRenderer.h"

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/Renderer2D.h"

#include "LkEngine/Asset/AssetManager.h"

#include "LkEngine/Core/Window.h"


namespace LkEngine {

	namespace {
		constexpr int MaxTexturesArrays = 10;
		constexpr int MaxTexturesPerArrayTexture = 32;
	}

	struct FRendererData
	{
		RendererCapabilities m_RendererCapabilities;

		TObjectPtr<LOpenGLArrayTexture> ArrayTextures[MaxTexturesArrays];
	};

	static FRendererData* RendererData = nullptr;
	static int ArrayTextureCount = 0;

	LOpenGLRenderer::LOpenGLRenderer()
	{
		LOBJECT_REGISTER();
	}

	void LOpenGLRenderer::Initialize()
	{
		LK_CORE_DEBUG_TAG("OpenGLRenderer", "Initializing renderer");
		RendererData = new FRendererData();

		RenderContext = LWindow::Get().GetRenderContext().As<LOpenGLContext>();

		/* Array texture dimensions. */
		static constexpr EArrayTextureDimension ArrayTextureDimensions[] = {
			EArrayTextureDimension::Dim_512x512,
			EArrayTextureDimension::Dim_1024x1024,
			EArrayTextureDimension::Dim_2048x2048
		};

		FArrayTextureSpecification ArrayTextureSpec;
		for (int i = 0; i < LK_ARRAYSIZE(ArrayTextureDimensions); i++)
		{
			ArrayTextureSpec.ImageFormat = EImageFormat::RGBA;
			ArrayTextureSpec.TextureSlot = ArrayTextureCount;
			ArrayTextureSpec.Dimension = ArrayTextureDimensions[i];
			ArrayTextureSpec.DebugName = LK_FORMAT_STRING("GLArrayTexture-{}", Enum::ToString(ArrayTextureDimensions[i]));
			RendererData->ArrayTextures[ArrayTextureCount++] = TObjectPtr<LOpenGLArrayTexture>::Create(ArrayTextureSpec);
			LK_CORE_TRACE_TAG("OpenGLRenderer", "Added array texture {} with name \"{}\"", i, ArrayTextureSpec.DebugName);
		}

		LK_CORE_TRACE_TAG("OpenGLRenderer", "Binding array textures");
		const auto& Textures2D = LTextureLibrary::Get().GetTextures2D();
		for (const auto& [TextureName, Texture] : Textures2D)
		{
			switch (Texture->GetWidth())
			{
				case 512:
					GetArrayTexture(0)->AddTextureToArray(Texture);
					break;

				case 1024:
					GetArrayTexture(1)->AddTextureToArray(Texture);
					break;

				case 2048:
					GetArrayTexture(2)->AddTextureToArray(Texture);
					break;

				default:
					break;
			}
		}

		/* Bind the array textures. */
		for (int i = 0; i < LRenderer2D::MaxArrayTextures; i++)
		{
			if (RendererData->ArrayTextures[i])
			{
				LK_CORE_TRACE_TAG("OpenGLRenderer", "ArrayTexture {}  RendererID={}", i, RendererData->ArrayTextures[i]->GetRendererID());
				BindArrayTexture(i);
			}
		}

		LK_CORE_TRACE_TAG("OpenGLRenderer", "Creating render passes");
		/* Create render passes. */
		/* Render Pass: Geometric */
		FRenderPassSpecification RP_Geometric;
		RP_Geometric.DebugName = "RenderPass_Geometric";
		GeometricPass = TObjectPtr<LOpenGLRenderPass>::Create(RP_Geometric);

		/* Render Pass: 2D */
		FRenderPassSpecification RP_2D;
		RP_2D.DebugName = "RenderPass_2D";
		RenderPass2D = TObjectPtr<LOpenGLRenderPass>::Create(RP_2D);

		RenderContext->SetDepthEnabled(true);
		SetPrimitiveTopology(ERenderTopology::Triangles);
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
		LK_OpenGL_Verify(glClearColor(LRenderer::ClearColor.r,
							   LRenderer::ClearColor.g,
							   LRenderer::ClearColor.b,
							   LRenderer::ClearColor.a));
		LK_OpenGL_Verify(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}

	void LOpenGLRenderer::SetPrimitiveTopology(const ERenderTopology InRenderTopology)
	{
		/* No need to call RenderContext here as the topology is used for draw calls. */
		switch (InRenderTopology)
		{
			case ERenderTopology::Lines:
			{
				m_Topology = GL_LINES;
				//Renderer2D->m_Topology = GL_LINES;
				break;
			}

			case ERenderTopology::Triangles:
			{
				m_Topology = GL_TRIANGLES;
				//Renderer2D->m_Topology = GL_TRIANGLES;
				break;
			}
		}
	}

	void LOpenGLRenderer::SetDepthFunction(const EDepthFunction InDepthFunction)
	{
		RenderContext->SetDepthFunction(InDepthFunction);
	}

	void LOpenGLRenderer::Draw(LVertexBuffer& VertexBuffer, const LShader& Shader)
	{
		Shader.Bind();
		LK_OpenGL_Verify(glDrawElements(m_Topology, VertexBuffer.GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr));
	}

	void LOpenGLRenderer::Draw(const LVertexBuffer& VertexBuffer, const LIndexBuffer& IndexBuffer, const LShader& Shader)
	{
		LK_OpenGL_Verify(glDrawElements(m_Topology, IndexBuffer.GetCount(), GL_UNSIGNED_INT, nullptr));
	}

	void LOpenGLRenderer::SubmitMesh(TObjectPtr<LMesh>& Mesh, TObjectPtr<LShader>& Shader, const glm::mat4& Transform)
	{
		const uint8_t Topology = m_Topology;
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
		// Renderer2D->DrawImage(image);
	}

	void LOpenGLRenderer::SubmitImage(const TObjectPtr<LImage2D> image)
	{
		// Renderer2D->DrawImage(image);
	}

	void LOpenGLRenderer::SubmitLine(const glm::vec2& p1, const glm::vec2& p2, const glm::vec4& Color, uint64_t EntityID)
	{
		// Renderer2D->DrawLine(p1, p2, Color, EntityID);
	}

	void LOpenGLRenderer::SubmitIndexed(const uint32_t IndexCount)
	{
		LK_OpenGL_Verify(glDrawElements(m_Topology, IndexCount, GL_UNSIGNED_INT, nullptr));
	}

	void LOpenGLRenderer::DrawIndexed(const uint64_t IndexCount)
	{
		LK_OpenGL_Verify(glDrawElements(m_Topology, static_cast<GLsizei>(IndexCount), GL_UNSIGNED_INT, nullptr));
	}

	void LOpenGLRenderer::SubmitQuad(const glm::vec2& Pos, const glm::vec2& Size, const glm::vec4& Color, uint64_t EntityID)
	{
		// Renderer2D->DrawQuad({ Pos.x, Pos.y, 0.0f }, Size, Color, EntityID);
	}

	void LOpenGLRenderer::SubmitQuad(const glm::vec3& Pos, const glm::vec2& Size, const glm::vec4& Color, uint64_t EntityID)
	{
		// Renderer2D->DrawQuad(Pos, Size, Color, EntityID);
	}

	void LOpenGLRenderer::SubmitQuad(const glm::vec2& Pos, const glm::vec2& Size, const glm::vec4& Color,
									 float Rotation, uint64_t EntityID)
	{
		// Renderer2D->DrawQuad({ Pos.x, Pos.y, 0.0f }, Size, Color, Rotation, EntityID);
	}

	void LOpenGLRenderer::SubmitQuad(const glm::vec3& Pos, const glm::vec2& Size,
									 const glm::vec4& Color, float Rotation, uint64_t EntityID)
	{
		// Renderer2D->DrawQuad(Pos, Size, Color, Rotation, EntityID);
	}

	void LOpenGLRenderer::SubmitQuad(const glm::vec2& Pos, const glm::vec2& Size, TObjectPtr<LTexture> Texture,
									 const float Rotation, uint64_t EntityID)
	{
		// Renderer2D->DrawQuad({ Pos.x, Pos.y, 0.0f }, Size, Texture, Rotation, EntityID);
	}

	void LOpenGLRenderer::SubmitQuad(const glm::vec2& Pos, const glm::vec2& Size, TObjectPtr<LTexture> Texture,
									 const glm::vec4& tintColor, const float Rotation, uint64_t EntityID)
	{
		// Renderer2D->DrawQuad({ Pos.x, Pos.y, 0.0f }, Size, Texture, tintColor, Rotation, EntityID);
	}

	void LOpenGLRenderer::SubmitQuad(const glm::vec3& Pos, const glm::vec2& Size, TObjectPtr<LTexture> Texture,
									 const glm::vec4& tintColor, float Rotation, uint64_t EntityID)
	{
		// Renderer2D->DrawQuad(Pos, Size, Texture, tintColor, Rotation, EntityID);
	}

	void LOpenGLRenderer::SubmitQuad(const glm::vec3& Pos, const glm::vec2& Size,
									 TObjectPtr<LTexture> Texture, float Rotation, uint64_t EntityID)
	{
		// Renderer2D->DrawQuad(Pos, Size, Texture, Rotation, EntityID);
	}

	RendererCapabilities& LOpenGLRenderer::GetCapabilities()
	{
		return RendererData->m_RendererCapabilities;
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

	TObjectPtr<LOpenGLArrayTexture> LOpenGLRenderer::GetArrayTexture(const int Index)
	{
		LK_CORE_ASSERT(RendererData && RendererData->ArrayTextures[Index]);
		return RendererData->ArrayTextures[Index];
	}

	void LOpenGLRenderer::BindArrayTexture(const uint8_t Index)
	{
		LK_CORE_ASSERT(RendererData && RendererData->ArrayTextures[Index]);
		LArrayTexture& ArrayTexture = *RendererData->ArrayTextures[Index];
		ArrayTexture.Bind();
	}

	void LOpenGLRenderer::BindArrayTexture(const EArrayTextureDimension Dimension)
	{
		LOpenGLArrayTexture& ArrayTexture = *GetArrayTextureWithDimension(Dimension);
		ArrayTexture.Bind();
	}

	TObjectPtr<LOpenGLArrayTexture> LOpenGLRenderer::GetArrayTextureWithDimension(const EArrayTextureDimension ArrayTextureDimension)
	{
		for (const TObjectPtr<LOpenGLArrayTexture>& ArrayTexture : RendererData->ArrayTextures)
		{
			if (ArrayTexture->Specification.Dimension == ArrayTextureDimension)
			{
				return ArrayTexture;
			}
		}

		LK_CORE_ASSERT(false, "Couldn't find ArrayTexture with dimension {}...", static_cast<int>(ArrayTextureDimension));
		return nullptr;
	}

}
