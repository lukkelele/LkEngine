#include "LKpch.h"
#include "LkOpenGL.h"

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/Renderer2D.h"

#include "OpenGLRenderer.h"
#include "OpenGLRenderer2D.h"
#include "OpenGLShader.h"
#include "OpenGLTexture.h"


namespace LkEngine::LOpenGL {

	static constexpr uint32_t MaxTexturesPerArray = 32;

	/**
	 * LOpenGL::Debug
	 */
    namespace Debug
    {
		uint32_t TextureColorBufferID;
		uint32_t DepthTextureID;
		uint32_t CubeTexture_;
		uint32_t FloorTexture_;

		uint32_t CubeVAO, CubeVBO;
		uint32_t PlaneVAO, PlaneVBO;
		uint32_t QuadVAO, QuadVBO;

		LRendererID SkyboxVAO;
		uint32_t SkyboxVBO;

		TObjectPtr<LVertexBuffer> CubeVertexBuffer{};
		TObjectPtr<LVertexBuffer> PlaneVertexBuffer{};
		TObjectPtr<LTexture2D> CubeTexture{};
		TObjectPtr<LTexture2D> PlaneTexture{};

		void RenderMirrorTexture(const glm::mat4& view, const glm::mat4& proj)
		{
			if (!DebugShader || !ScreenShader || !CubeTexture || !PlaneTexture)
			{
				SetupTexturesAndShaders();
				return;
			}

			LRenderer::GetViewportFramebuffer()->Bind();
			LK_OpenGL(glEnable(GL_DEPTH_TEST));

			DebugShader->Bind();
			ModelMVP = glm::mat4(1.0f);
			DebugShader->Set("u_ViewMatrix", view);
			DebugShader->Set("u_ProjectionMatrix", proj);

			// Cubes
			{
				CubeVertexBuffer->Bind();
				CubeTexture->Bind();

				// Cube 1
				ModelMVP = glm::translate(ModelMVP, glm::vec3(-1.0f, 0.0f, -1.0f));
				DebugShader->Set("u_Model", ModelMVP);
				LK_OpenGL(glDrawArrays(GL_TRIANGLES, 0, 36));

				// Cube 2
				ModelMVP = glm::mat4(1.0f);
				ModelMVP = glm::translate(ModelMVP, glm::vec3(2.0f, 0.0f, 0.0f));
				DebugShader->Set("u_Model", ModelMVP);
				LK_OpenGL(glDrawArrays(GL_TRIANGLES, 0, 36));
			}
			// Floor 
			{
				PlaneVertexBuffer->Bind();
				PlaneTexture->Bind();
				//glBindTexture(GL_TEXTURE_2D, FloorTexture);

				DebugShader->Set("u_Model", glm::mat4(1.0f));
				LK_OpenGL(glDrawArrays(GL_TRIANGLES, 0, 6));
			}

			LFramebuffer::TargetSwapChain();
		}

		void RenderScreenTexture(const glm::mat4& view, const glm::mat4& proj)
		{
			if (!DebugShader || !ScreenShader || !CubeTexture || !PlaneTexture)
			{
				SetupTexturesAndShaders();
				return;
			}

			LFramebuffer::TargetSwapChain();
			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//glClearColor(0.1f, 0.2f, 0.1f, 1.0f);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			ModelMVP = glm::mat4(1.0f);
			DebugShader->Bind();
			DebugShader->Set("View", view);
			DebugShader->Set("Projection", proj);

			// Cubes
			{
				// Cube 1
				CubeVertexBuffer->Bind();
				CubeTexture->Bind();
				ModelMVP = glm::translate(ModelMVP, glm::vec3(-1.0f, 0.0f, -1.0f));
				DebugShader->Set("u_Model", ModelMVP);
				LK_OpenGL(glDrawArrays(GL_TRIANGLES, 0, 36));

				// Cube 2
				ModelMVP = glm::mat4(1.0f);
				ModelMVP = glm::translate(ModelMVP, glm::vec3(2.0f, 0.0f, 0.0f));
				DebugShader->Set("u_Model", ModelMVP);
				LK_OpenGL(glDrawArrays(GL_TRIANGLES, 0, 36));
			}

			/* Floor. */
			{
				glBindVertexArray(PlaneVAO);
				PlaneTexture->Bind();
				DebugShader->Set("u_Model", glm::mat4(1.0f));
				LK_OpenGL(glDrawArrays(GL_TRIANGLES, 0, 6));
				LK_OpenGL(glBindVertexArray(0));
			}

			// Draw the mirror quad with screen texture.
			// Disable depth test so screen-space quad isn't discarded due to depth test.
			LK_OpenGL(glDisable(GL_DEPTH_TEST)); 
			ScreenShader->Bind();
			TObjectPtr<LImage2D> colorAttachment0 = LRenderer::GetViewportFramebuffer()->GetImage(0);

			/* Use the color attachment texture as the texture of the quad plane. */
			LK_OpenGL(glBindTexture(GL_TEXTURE_2D, colorAttachment0->GetRendererID()));
			LK_OpenGL(glDrawArrays(GL_TRIANGLES, 0, 6));

			DebugShader->Unbind();
			ScreenShader->Unbind();
		}

		void RenderCubes(const glm::mat4& view, const glm::mat4& projection)
		{
			if (!CubeTexture)
			{
				SetupTexturesAndShaders();
				return;
			}

			glEnable(GL_DEPTH_TEST);

			LRenderer::GetViewportFramebuffer()->Bind();

			DebugShader->Bind();
			DebugShader->Set("u_ViewMatrix", view);
			DebugShader->Set("u_ProjectionMatrix", projection);
			//DebugShader->Set("u_ViewProjectionMatrix", viewProjection);

			ModelMVP = glm::mat4(1.0f);

			CubeVertexBuffer->Bind();
			CubeTexture->Bind(0);

			// Cube 1
			ModelMVP = glm::translate(ModelMVP, glm::vec3(-1.0f, 0.0f, -1.0f));
			DebugShader->Set("u_Model", ModelMVP);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			// Cube 2
			ModelMVP = glm::mat4(1.0f);
			ModelMVP = glm::translate(ModelMVP, glm::vec3(2.0f, 0.0f, 0.0f));
			DebugShader->Set("u_Model", ModelMVP);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			DebugShader->Unbind();

			LFramebuffer::TargetSwapChain();
		}

		void RenderFloor(const glm::mat4& view, const glm::mat4& projection)
		{
			glEnable(GL_DEPTH_TEST);
			//Renderer2DAPI::Get().As<OpenGLRenderer2D>()->GetFramebuffer()->Bind();
			LRenderer::GetViewportFramebuffer()->Bind();

			ModelMVP = glm::mat4(1.0f);
			DebugShader->Bind();
			DebugShader->Set("u_ViewMatrix", view);
			DebugShader->Set("u_ProjectionMatrix", projection);

			PlaneVertexBuffer->Bind();
			PlaneTexture->Bind();

			DebugShader->Set("u_Model", glm::mat4(1.0f));
			glDrawArrays(GL_TRIANGLES, 0, 6);

			LFramebuffer::TargetSwapChain();
		}

		void GenerateCubeVaoAndVbo(uint32_t& vao, uint32_t& vbo)
		{
			CubeVertexBuffer = LVertexBuffer::Create(Cube_Vertices, sizeof(Cube_Vertices));
			CubeVertexBuffer->SetLayout({
				{ "a_Position",      ShaderDataType::Float3  },
				{ "a_Color",         ShaderDataType::Float4  },
				{ "a_Texcoord",      ShaderDataType::Float2  },
				{ "a_TexIndex",      ShaderDataType::Float,  },
				{ "a_TexArray",      ShaderDataType::Float,  },
				{ "a_TilingFactor",  ShaderDataType::Float,  },
			});

			if (!DebugShader || !ScreenShader || !CubeTexture || !PlaneTexture)
			{
				SetupTexturesAndShaders();
				return;
			}
		}

		void SetupTexturesAndShaders()
		{
			if (!DebugShader)
			{
				DebugShader = LRenderer::GetShaderLibrary()->Get("Renderer2D_Debug");
			}

			if (!ScreenShader)
			{
				ScreenShader = LRenderer::GetShaderLibrary()->Get("Renderer2D_Screen");
			}

			if (!CubeTexture)
			{
				CubeTexture = LTextureLibrary::Get().GetTexture("wood-container");
			}

			if (!PlaneTexture)
			{
				//PlaneTexture = LTextureLibrary::Get()->GetTexture("metal");
			}
		}

		void GeneratePlaneVaoAndVbo(uint32_t& vao, uint32_t& vbo)
		{
			if (!DebugShader)
			{
				DebugShader = LRenderer::GetShaderLibrary()->Get("Renderer2D_Debug");
			}

			if (!ScreenShader)
			{
				ScreenShader = LRenderer::GetShaderLibrary()->Get("Renderer2D_Screen");
			}

			PlaneVertexBuffer = LVertexBuffer::Create(Plane_Vertices, sizeof(Plane_Vertices));
			PlaneVertexBuffer->SetLayout({
				{ "a_Pos",          ShaderDataType::Float3  },
				{ "a_Texcoord",     ShaderDataType::Float2  },
			});
			LK_CORE_TRACE_TAG("LkOpenGL", "Generated CubeVertexBuffer!");
		}

		void GenerateScreenQuadVaoAndVbo(uint32_t& vao, uint32_t& vbo)
		{
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);

			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Quad_Vertices), &Quad_Vertices, GL_STATIC_DRAW);

			// Vertex Indexing
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

			// Texture Indexing
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		}

		TObjectPtr<LShader> GetDebugShader() 
		{ 
			return DebugShader; 
		}

		TObjectPtr<LShader> GetScreenShader() 
		{ 
			return ScreenShader; 
		}

    }

}