#include "LKpch.h"
#include "LkOpenGL.h"

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/Renderer2D.h"

#include "OpenGLRenderer.h"
#include "OpenGLRenderer2D.h"
#include "OpenGLShader.h"
#include "OpenGLTexture.h"


namespace LkEngine {

	//static constexpr uint32_t MaxTexturesPerArray = 32;

	/**
	 * LOpenGL::Debug
	 */
    namespace LOpenGL_Debug
    {
		uint32_t TextureColorBufferID;
		uint32_t DepthTextureID;
		uint32_t CubeTexture_;
		uint32_t FloorTexture_;

		uint32_t CubeVAO;
		uint32_t CubeVBO;
		uint32_t PlaneVAO;
		uint32_t PlaneVBO;
		uint32_t QuadVAO;
		uint32_t QuadVBO;

		LRendererID SkyboxVAO;
		uint32_t SkyboxVBO;

		TObjectPtr<LVertexBuffer> CubeVertexBuffer{};
		TObjectPtr<LVertexBuffer> PlaneVertexBuffer{};
		TObjectPtr<LTexture2D> CubeTexture{};
		TObjectPtr<LTexture2D> PlaneTexture{};

		TObjectPtr<LShader> ScreenShader;
		TObjectPtr<LShader> DebugShader;
		TObjectPtr<LShader> CubeDebugShader;

		TObjectPtr<LVertexBuffer> SkyboxVertexBuffer{};
		TObjectPtr<LTextureCube> SkyboxTexture{};
		TObjectPtr<LShader> SkyboxShader{};
		uint32_t CubemapTexture{};

		glm::mat4 ModelMVP = glm::mat4(1.0f);
		glm::mat4 View = glm::mat4(1.0f);
		glm::mat4 Projection = glm::mat4(1.0f);

		void RenderMirrorTexture(const glm::mat4& view, const glm::mat4& proj)
		{
			if (!DebugShader || !ScreenShader || !CubeTexture || !PlaneTexture)
			{
				SetupTexturesAndShaders();
				return;
			}

			LRenderer::GetViewportFramebuffer()->Bind();
			LK_OpenGL_Verify(glEnable(GL_DEPTH_TEST));

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
				LK_OpenGL_Verify(glDrawArrays(GL_TRIANGLES, 0, 36));

				// Cube 2
				ModelMVP = glm::mat4(1.0f);
				ModelMVP = glm::translate(ModelMVP, glm::vec3(2.0f, 0.0f, 0.0f));
				DebugShader->Set("u_Model", ModelMVP);
				LK_OpenGL_Verify(glDrawArrays(GL_TRIANGLES, 0, 36));
			}
			// Floor 
			{
				PlaneVertexBuffer->Bind();
				PlaneTexture->Bind();
				//glBindTexture(GL_TEXTURE_2D, FloorTexture);

				DebugShader->Set("u_Model", glm::mat4(1.0f));
				LK_OpenGL_Verify(glDrawArrays(GL_TRIANGLES, 0, 6));
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
		#if 0
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClearColor(0.1f, 0.2f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		#endif

			ModelMVP = glm::mat4(1.0f);
			DebugShader->Bind();
			//DebugShader->Set("View", view);
			//DebugShader->Set("Projection", proj);
			DebugShader->Set("u_ViewMatrix", view);
			DebugShader->Set("u_ProjectionMatrix", proj);

			// Cubes
			{
				// Cube 1
				CubeVertexBuffer->Bind();
				CubeTexture->Bind();
				ModelMVP = glm::translate(ModelMVP, glm::vec3(-1.0f, 0.0f, -1.0f));
				DebugShader->Set("u_Model", ModelMVP);
				LK_OpenGL_Verify(glDrawArrays(GL_TRIANGLES, 0, 36));

				// Cube 2
				ModelMVP = glm::mat4(1.0f);
				ModelMVP = glm::translate(ModelMVP, glm::vec3(2.0f, 0.0f, 0.0f));
				DebugShader->Set("u_Model", ModelMVP);
				LK_OpenGL_Verify(glDrawArrays(GL_TRIANGLES, 0, 36));
			}

			/* Floor. */
			{
				LK_OpenGL_Verify(glBindVertexArray(PlaneVAO));
				PlaneTexture->Bind();
				DebugShader->Set("u_Model", glm::mat4(1.0f));
				LK_OpenGL_Verify(glDrawArrays(GL_TRIANGLES, 0, 6));
				LK_OpenGL_Verify(glBindVertexArray(0));
			}

			// Draw the mirror quad with screen texture.
			// Disable depth test so screen-space quad isn't discarded due to depth test.
			LK_OpenGL_Verify(glDisable(GL_DEPTH_TEST)); 
			ScreenShader->Bind();
			TObjectPtr<LImage2D> ColorAttachment0 = LRenderer::GetViewportFramebuffer()->GetImage(0);

			/* Use the color attachment texture as the texture of the quad plane. */
			LK_OpenGL_Verify(glBindTexture(GL_TEXTURE_2D, ColorAttachment0->GetRendererID()));
			LK_OpenGL_Verify(glDrawArrays(GL_TRIANGLES, 0, 6));

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

			LRenderer::GetViewportFramebuffer()->Bind();
			LK_OpenGL_Verify(glEnable(GL_DEPTH_TEST));
			//LK_OpenGL(glDepthFunc(GL_LESS));

			DebugShader->Bind();
			DebugShader->Set("u_ViewMatrix", view);
			DebugShader->Set("u_ProjectionMatrix", projection);
			//DebugShader->Set("u_ViewProjectionMatrix", viewProjection);
			/* EXPERIMENTAL. */
			DebugShader->Set("Texture1", 0);
			CubeTexture->Bind(0);

			ModelMVP = glm::mat4(1.0f);
			CubeVertexBuffer->Bind();

			/* Cube 1. */
			ModelMVP = glm::translate(ModelMVP, glm::vec3(-1.0f, 0.0f, -1.0f));
			DebugShader->Set("u_Model", ModelMVP);
			LK_OpenGL_Verify(glDrawArrays(GL_TRIANGLES, 0, 36));

			/* Cube 2. */
			ModelMVP = glm::mat4(1.0f);
			ModelMVP = glm::translate(ModelMVP, glm::vec3(2.0f, 0.0f, 0.0f));
			DebugShader->Set("u_Model", ModelMVP);
			LK_OpenGL_Verify(glDrawArrays(GL_TRIANGLES, 0, 36));

			/* Clean up before exit. */
			DebugShader->Unbind();

			LFramebuffer::TargetSwapChain();
		}

		void RenderFloor(const glm::mat4& view, const glm::mat4& projection)
		{
			LK_OpenGL_Verify(glEnable(GL_DEPTH_TEST));
			LRenderer::GetViewportFramebuffer()->Bind();

			ModelMVP = glm::mat4(1.0f);
			DebugShader->Bind();
			DebugShader->Set("u_ViewMatrix", view);
			DebugShader->Set("u_ProjectionMatrix", projection);

			PlaneVertexBuffer->Bind();
			PlaneTexture->Bind();

			DebugShader->Set("u_Model", glm::mat4(1.0f));
			LK_OpenGL_Verify(glDrawArrays(GL_TRIANGLES, 0, 6));

			LFramebuffer::TargetSwapChain();
		}

		void InitializeEnvironment()
		{
			SetupTexturesAndShaders();

			SetupDebugCube();
			GeneratePlaneVaoAndVbo(PlaneVAO, PlaneVBO);
			GenerateScreenQuadVaoAndVbo(QuadVAO, QuadVBO);

			CubeTexture_ = LoadTexture("Assets/Textures/container.jpg");
			FloorTexture_ = LoadTexture("Assets/Textures/metal.png");

			SetupSkybox();
		}

		// void GenerateCubeVaoAndVbo(uint32_t& vao, uint32_t& vbo)
		/* TODO: Remove passed arguments. */
		void SetupDebugCube()
		{
			LK_CORE_WARN_TAG("LOpenGL", "Generating cube VAO and VBO");
			if (!CubeVertexBuffer)
			{
				CubeVertexBuffer = LVertexBuffer::Create(Cube_Vertices, sizeof(Cube_Vertices));
				CubeVertexBuffer->SetLayout({
					{ "a_Position",      EShaderDataType::Float3 },
					{ "a_Color",         EShaderDataType::Float4 },
					{ "a_TexCoord",      EShaderDataType::Float2 },
					{ "a_TexIndex",      EShaderDataType::Float  },
					{ "a_TexArray",      EShaderDataType::Float  },
					{ "a_TilingFactor",  EShaderDataType::Float  },
				});

				/* REMOVE */
				if (!DebugShader || !ScreenShader || !CubeTexture || !PlaneTexture)
				{
					SetupTexturesAndShaders();
					return;
				}
			}
		}

		void SetupTexturesAndShaders()
		{
			/* REMOVE */
			if (!DebugShader || !ScreenShader || !CubeTexture || !PlaneTexture)
			{
				LK_CORE_INFO_TAG("OpenGL", "Setting up textures and shaders");
			}

			/* Shaders. */
			if (!CubeDebugShader)
			{
				CubeDebugShader = LRenderer::GetShaderLibrary()->Get("Renderer_Debug");
				//const FShaderProgramSource& ShaderSource = CubeDebugShader->GetSource();
				//LK_CORE_WARN_TAG("LOpenGL", "DebugCubeShader:\n{}\n{}", ShaderSource.Vertex, ShaderSource.Fragment);
				CubeDebugShader->Set("u_Texture0", 0);
			}

			if (!DebugShader)
			{
				DebugShader = LRenderer::GetShaderLibrary()->Get("Renderer2D_Debug");
				//const FShaderProgramSource& ShaderSource = DebugShader->GetSource();
				//LK_CORE_WARN_TAG("LOpenGL", "DebugShader -> {}:\n{}\n{}", DebugShader->GetName(), ShaderSource.Vertex, ShaderSource.Fragment);
			}

			if (!ScreenShader)
			{
				ScreenShader = LRenderer::GetShaderLibrary()->Get("Renderer2D_Screen");
			}

			/* Textures. */
			if (!CubeTexture)
			{
				CubeTexture = LTextureLibrary::Get().GetTexture("wood-container");
			}

			if (!PlaneTexture)
			{
				PlaneTexture = LTextureLibrary::Get().GetTexture("MetalGround");
				LK_CORE_ASSERT(PlaneTexture, "Plane Texture failed to setup");
			}
		}

		void SetupSkybox()
		{
			LK_CORE_TRACE("Setting up skybox");
			LK_CORE_ASSERT(SkyboxVertexBuffer == nullptr, "Skybox already exists");

			SkyboxVertexBuffer = LVertexBuffer::Create(Skybox_Vertices, sizeof(Skybox_Vertices));
			SkyboxVertexBuffer->SetLayout({
				{ "a_Position", EShaderDataType::Float3 },
			});
			SkyboxShader = LRenderer::GetShaderLibrary()->Get("Renderer_Skybox");
			LK_CORE_ASSERT(SkyboxShader, "Skybox shader is nullptr");

			FTextureSpecification SkyboxSpec;
			static std::vector<std::filesystem::path> SkyboxFacePaths = {
				"Assets/Textures/Debug/Skybox-WaterIsland/Right.jpg",
				"Assets/Textures/Debug/Skybox-WaterIsland/Left.jpg",
				"Assets/Textures/Debug/Skybox-WaterIsland/Top.jpg",
				"Assets/Textures/Debug/Skybox-WaterIsland/Bottom.jpg",
				"Assets/Textures/Debug/Skybox-WaterIsland/Front.jpg",
				"Assets/Textures/Debug/Skybox-WaterIsland/Back.jpg",
			};
			SkyboxTexture = LTextureCube::Create(SkyboxSpec, SkyboxFacePaths);
			SkyboxTexture->Bind(0);
			SkyboxShader->Bind();
			SkyboxShader->Set("u_Skybox", 0);
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
				{ "a_Pos",       EShaderDataType::Float3 },
				{ "a_TexCoord",  EShaderDataType::Float2 },
			});
			LK_CORE_TRACE_TAG("LkOpenGL", "Generated CubeVertexBuffer!");
		}

		void GenerateScreenQuadVaoAndVbo(uint32_t& VaoRef, uint32_t& VboRef)
		{
			LK_OpenGL_Verify(glGenVertexArrays(1, &VaoRef));
			LK_OpenGL_Verify(glGenBuffers(1, &VboRef));

			LK_OpenGL_Verify(glBindVertexArray(VaoRef));
			LK_OpenGL_Verify(glBindBuffer(GL_ARRAY_BUFFER, VboRef));
			LK_OpenGL_Verify(glBufferData(GL_ARRAY_BUFFER, sizeof(Quad_Vertices), &Quad_Vertices, GL_STATIC_DRAW));

			/* Vertex Indexing. */
			LK_OpenGL_Verify(glEnableVertexAttribArray(0));
			LK_OpenGL_Verify(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0));

			/* Texture Indexing. */
			LK_OpenGL_Verify(glEnableVertexAttribArray(1));
			LK_OpenGL_Verify(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))));
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
