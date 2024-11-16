#include "LKpch.h"
#include "OpenGLRenderer.h"

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/Renderer2D.h"

#include "LkEngine/Asset/AssetManager.h"

#include "LkEngine/Core/Window.h"


namespace LkEngine {

	namespace {
		constexpr int MaxTexturesArrays = 10;
		constexpr int MaxTexturesPerTextureArray = 32;
	}

	struct FRendererData
	{
		RendererCapabilities m_RendererCapabilities;

		TObjectPtr<LOpenGLTextureArray> TextureArrays[MaxTexturesArrays];
	};

	static FRendererData* RendererData = nullptr;
	static int TextureArrayCount = 0;

	LOpenGLRenderer::LOpenGLRenderer()
	{
		LCLASS_REGISTER();
	}

	void LOpenGLRenderer::Initialize()
	{
		LK_CORE_DEBUG_TAG("OpenGLRenderer", "Initializing");
		RendererData = new FRendererData();

		RenderContext = LWindow::Get().GetRenderContext().As<LOpenGLContext>();

		// Texture Arrays
		static constexpr ETextureArrayDimension TextureArrayDimensions[] = {
			ETextureArrayDimension::Dim_512x512,
			ETextureArrayDimension::Dim_1024x1024,
			ETextureArrayDimension::Dim_2048x2048
		};

		FTextureArraySpecification TextureArraySpec;
		for (int i = 0; i < LK_ARRAYSIZE(TextureArrayDimensions); i++)
		{
			TextureArraySpec.ImageFormat = EImageFormat::RGBA;
			TextureArraySpec.TextureSlot = TextureArrayCount;
			TextureArraySpec.TextureArrayDimension = TextureArrayDimensions[i];
			TextureArraySpec.DebugName = LK_FORMAT_STRING("GLArrayTexture-{}", Enum::ToString(TextureArrayDimensions[i]));
			RendererData->TextureArrays[TextureArrayCount++] = TObjectPtr<LOpenGLTextureArray>::Create(TextureArraySpec);
			LK_CORE_TRACE_TAG("OpenGLRenderer", "Added array texture {} with name \"{}\"", i, TextureArraySpec.DebugName);
		}

		LK_CORE_DEBUG_TAG("OpenGLRenderer", "Adding Textures to Array Textures");
		const auto& Textures2D = LAssetManager::GetTextures2D();
		for (std::size_t i = 0; i < Textures2D.size(); i++)
		{
			const TObjectPtr<LOpenGLImage2D> img = Textures2D.at(i).second->GetImage().As<LOpenGLImage2D>();
			switch (img->GetWidth())
			{
				case 512:
					GetTextureArray(0)->AddTextureToArray(Textures2D[i].second);
					break;

				case 1024:
					GetTextureArray(1)->AddTextureToArray(Textures2D[i].second);
					break;

				case 2048:
					GetTextureArray(2)->AddTextureToArray(Textures2D[i].second);
					break;

				default:
					break;
			}
		}

#if 1
		/* Add Texture array references to OpenGLRenderer2D as well. */
		// for (int i = 0; i < LOpenGLRenderer2D::MaxTextureArrays; i++)
		for (int i = 0; i < LRenderer2D::MaxTextureArrays; i++)
		{
			if (RendererData->TextureArrays[i])
			{
				LK_CORE_DEBUG_TAG("OpenGLRenderer", "TextureArray {}  RendererID={}", i, RendererData->TextureArrays[i]->GetRendererID());
				// Renderer2D->TextureArrays[i] = RendererData->TextureArrays[i];
				BindTextureArray(i);
			}
		}
		LK_CORE_DEBUG_TAG("OpenGLRenderer", "Initializing 2D renderer");

		RenderContext->SetDepthEnabled(true);
		/* Default to triangles */
		SetPrimitiveTopology(ERenderTopology::Triangles);

		/* Create render passes. */
		{
			/* Geometric. */
			RenderPassSpecification RenderPassSpec;
			RenderPassSpec.DebugName = "RenderPass_Geometric";
			m_GeometricPass = TObjectPtr<OpenGLRenderPass>::Create(RenderPassSpec);

			/* 2D. */
			RenderPassSpec.DebugName = "RenderPass_2D";
			m_RenderPass2D = TObjectPtr<OpenGLRenderPass>::Create(RenderPassSpec);
		}
#endif
	}

	void LOpenGLRenderer::Shutdown()
	{
		if (m_GeometricPass)
		{
			m_GeometricPass->Terminate();
			m_GeometricPass.Release();
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
		std::deque<LRendererID> BoundTextureArrays = InPipeline.As<LOpenGLPipeline>()->GetBoundTextureArrays();
		TObjectPtr<LFramebuffer> Framebuffer = LRenderer::GetViewportFramebuffer();

		/* TODO: Dont submit by & here. */
		LRenderer::Submit([&, Framebuffer, Shader, VertexBuffer,
						   Transform, IndexCount, BoundTextureArrays]() mutable
		{
			Framebuffer->Bind();
			Shader->Bind();

			int ArrIndex = 1;
			for (const LRendererID TextureArray : BoundTextureArrays)
			{
				Shader->Set("u_TextureArray" + std::to_string(ArrIndex++), TextureArray);
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

	TObjectPtr<LOpenGLTextureArray> LOpenGLRenderer::GetTextureArray(int idx)
	{
		return RendererData->TextureArrays[idx];
	}

	void LOpenGLRenderer::BindTextureArray(const uint8_t Index)
	{
		LK_VERIFY(RendererData);
		LTextureArray& TextureArray = *RendererData->TextureArrays[Index];
		TextureArray.Bind();
	}

	void LOpenGLRenderer::BindTextureArray(const ETextureArrayDimension& TextureArrayDimension)
	{
		LOpenGLTextureArray& TextureArray = *GetTextureArrayWithDimension(TextureArrayDimension);
		TextureArray.Bind();
	}

	TObjectPtr<LOpenGLTextureArray> LOpenGLRenderer::GetTextureArrayWithDimension(const ETextureArrayDimension TextureArrayDimension)
	{
		for (TObjectPtr<LOpenGLTextureArray>& TextureArray : RendererData->TextureArrays)
		{
			if (TextureArray->Specification.TextureArrayDimension == TextureArrayDimension)
			{
				return TextureArray;
			}
		}

		LK_CORE_ASSERT(false, "Couldn't find TextureArray with dimension {}...", static_cast<int>(TextureArrayDimension));
		return nullptr;
	}

}
