#include "LKpch.h"
#include "LkOpenGL.h"

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/Renderer2D.h"

#include "OpenGLRenderer.h"
#include "OpenGLRenderer2D.h"
#include "OpenGLShader.h"
#include "OpenGLTexture.h"


namespace LkEngine {

	static constexpr unsigned int MaxTexturesPerArray = 32;

	namespace GLUtils {

		int GetOpenGLSourceBlendFunction(const ESourceBlendFunction& InSourceBlendFunction)
		{
			switch (InSourceBlendFunction)
			{
				case ESourceBlendFunction::Zero:  return GL_ZERO;
				case ESourceBlendFunction::One:   return GL_ONE;
				case ESourceBlendFunction::Alpha: return GL_ALPHA;
				case ESourceBlendFunction::Color: return GL_COLOR;
				case ESourceBlendFunction::One_Minus_DestinationAlpha: return GL_ONE_MINUS_DST_ALPHA;
			}

			LK_CORE_ASSERT(false);
			return -1;
		}
	
		int GetOpenGLDestinationBlendFunction(const EDestinationBlendFunction& InDestinationBlendFunction)
		{
			switch (InDestinationBlendFunction)
			{
				case EDestinationBlendFunction::Zero:  return GL_ZERO;
				case EDestinationBlendFunction::One:   return GL_ONE;
				case EDestinationBlendFunction::Alpha: return GL_ALPHA;
				case EDestinationBlendFunction::Color: return GL_COLOR;
				case EDestinationBlendFunction::One_Minus_SourceAlpha: return GL_ONE_MINUS_SRC_ALPHA;
				default: throw std::runtime_error("Destination blend function could not be retrieved correctly");
			}
		}

		const GLubyte* GetExtensions() { return glGetString(GL_EXTENSIONS); }

		void PrintOpenGLExtensions()
		{
			int Extensions{};
			glGetIntegerv(GL_NUM_EXTENSIONS, &Extensions);
			for (int Index = 0; Index < Extensions; Index++)
			{
				const GLubyte* Extension = glGetStringi(GL_EXTENSIONS, Index);
				LK_CORE_DEBUG_TAG("OpenGL Extension", "{}", std::string(reinterpret_cast<const char*>(Extension)));
			}
		}

		GLenum OpenGLImageInternalFormat(EImageFormat Format)
		{
			switch (Format)
			{
				case EImageFormat::RGB:             return GL_RGB8;
				case EImageFormat::RGBA:            return GL_RGBA8;
				case EImageFormat::RGBA8:           return GL_RGBA8;
				case EImageFormat::RGBA16F:         return GL_RGBA16F;
				case EImageFormat::RGBA32F:         return GL_RGBA32F;
				case EImageFormat::DEPTH24STENCIL8: return GL_DEPTH24_STENCIL8;
				case EImageFormat::DEPTH32F:        return GL_DEPTH_COMPONENT32F;
			}

			LK_CORE_ASSERT(false);
			return GL_INVALID_ENUM;
		}

		GLenum OpenGLFormatDataType(EImageFormat ImageFormat)
		{
			switch (ImageFormat)
			{
				case EImageFormat::RGB:
				case EImageFormat::RGBA:    
				case EImageFormat::RGBA8:   return GL_UNSIGNED_BYTE;
				case EImageFormat::RGBA16F:
				case EImageFormat::RGBA32F: return GL_FLOAT;
			}

			LK_CORE_ASSERT(false, "Unknown OpenGLFormatDataType: {}", static_cast<int>(ImageFormat));
			return GL_INVALID_VALUE;
		}

		GLenum OpenGLSamplerWrap(ETextureWrap TextureWrap)
		{
			switch (TextureWrap)
			{
				case ETextureWrap::Clamp:   return GL_CLAMP_TO_EDGE;
				case ETextureWrap::Repeat:  return GL_REPEAT;
			}

			LK_CORE_ASSERT(false, "Unknown OpenGLSamplerWrap: {}", static_cast<int>(TextureWrap));
			return GL_INVALID_VALUE;
		}

		GLenum OpenGLSamplerFilter(const ETextureFilter TextureFilter, const bool bUseMipmap)
		{
			switch (TextureFilter)
			{
				case ETextureFilter::Linear:  return bUseMipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
				case ETextureFilter::Nearest: return bUseMipmap ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
			}

			LK_CORE_ASSERT(false, "Unknown OpenGLSamplerFilter: {}, mipmap: {}", static_cast<int>(TextureFilter), bUseMipmap);
			return GL_INVALID_VALUE;
		}

		GLenum ImageFormatToGLDataFormat(EImageFormat ImageFormat)
		{
			switch (ImageFormat)
			{
				case EImageFormat::RGBA:   
				case EImageFormat::RGBA8:   
				case EImageFormat::RGBA16F:   
				case EImageFormat::RGBA32F:  return GL_RGBA;

				case EImageFormat::RG8:     
				case EImageFormat::RG16F:     
				case EImageFormat::RG32F:    return GL_RG;

				case EImageFormat::RGB:  
				case EImageFormat::RGB8:     return GL_RGB;

				case EImageFormat::RED8UI:
				case EImageFormat::RED16UI:
				case EImageFormat::RED32UI:  return GL_RED_INTEGER;
				case EImageFormat::RED32F:   return GL_RED_INTEGER;

			}

			LK_CORE_ASSERT(false, "Invalid ImageFormat: {}", static_cast<int>(ImageFormat));
			return GL_INVALID_VALUE;
		}

		GLenum ImageFormatToGLInternalFormat(EImageFormat ImageFormat)
		{
			switch (ImageFormat)
			{
				case EImageFormat::RGB:      return GL_RGB32F;
				case EImageFormat::RGB8:     return GL_RGB8;

				case EImageFormat::RGBA:     return GL_RGBA32F;
				case EImageFormat::RGBA8:    return GL_RGBA8;
				case EImageFormat::RGBA16F:  return GL_RGBA16F;
				case EImageFormat::RGBA32F:  return GL_RGBA32F;

				case EImageFormat::RG16F:    return GL_RG16F;
				case EImageFormat::RG32F:    return GL_RG32F;

				case EImageFormat::RED8UI:   return GL_R8UI;
				case EImageFormat::RED16UI:  return GL_R16UI;
				case EImageFormat::RED32UI:  return GL_R32UI;
				case EImageFormat::RED32F:   return GL_R32F;
			}

			LK_CORE_ASSERT(false, "Invalid internal ImageFormat: {}", static_cast<int>(ImageFormat));
			return GL_INVALID_VALUE;
		}

		GLenum TextureTarget(const bool bMultisampled)
		{
			return (bMultisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D);
		}

		void CreateTextures(const bool bMultisampled, uint32_t* OutTextureID, const uint32_t Count)
		{
			glCreateTextures(TextureTarget(bMultisampled), Count, OutTextureID);
		}

		void BindTexture(bool bMultisampled, uint32_t id)
		{
			glBindTexture(TextureTarget(bMultisampled), id);
		}

		void AttachColorTexture(const uint32_t id, 
								const int Samples, 
								const GLenum InternalFormat, 
								const GLenum Format, 
								const uint32_t Width, uint32_t Height, int Index)
		{
			bool bMultisampled = Samples > 1;
			if (bMultisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Samples, InternalFormat, Width, Height, GL_FALSE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, Width, Height, 0, Format, GL_UNSIGNED_BYTE, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + Index, TextureTarget(bMultisampled), id, 0);
		}

		void AttachDepthTexture(uint32_t id, int Samples, GLenum Format, GLenum attachmentType, uint32_t Width, uint32_t Height)
		{
			bool bMultisampled = Samples > 1;
			if (bMultisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Samples, Format, Width, Height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, Format, Width, Height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(bMultisampled), id, 0);
		}

		GLenum FramebufferTextureFormatToGL(EImageFormat ImageFormat)
		{
			switch (ImageFormat)
			{
				case EImageFormat::RGBA8:            return GL_RGBA8;
				case EImageFormat::RGBA16F:          return GL_RGBA16F;
				case EImageFormat::RGBA32F:          return GL_RGBA32F;
				case EImageFormat::RED8UI:      
				case EImageFormat::RED8UN:      
				case EImageFormat::RED16UI:       
				case EImageFormat::RED32UI:          return GL_RED_INTEGER;
				case EImageFormat::DEPTH24STENCIL8:  return GL_DEPTH24_STENCIL8;
			}

			LK_CORE_ASSERT(false, "FramebufferTextureFormatToGL  ImageFormat not recognized");
			return GL_INVALID_VALUE;
		}

        std::string FramebufferTextureFormatToString(EFramebufferTextureFormat FramebufferTextureFormat)
		{
			switch (FramebufferTextureFormat)
			{
				case EFramebufferTextureFormat::RGBA8:           return "RGBA8";
				case EFramebufferTextureFormat::RED_INTEGER:     return "RED_INTEGER";
				case EFramebufferTextureFormat::DEPTH24STENCIL8: return "DEPTH24STENCIL8";
				case EFramebufferTextureFormat::None:            return "None";
			}

			LK_CORE_ASSERT(false, "Invalid FramebufferTextureFormat");
			return "";
		};
	}

    unsigned int TextureColorBufferID, DepthTextureID;
    unsigned int CubeTexture_, FloorTexture_;

    unsigned int CubeVAO, CubeVBO;
    unsigned int PlaneVAO, PlaneVBO;
    unsigned int QuadVAO, QuadVBO;

	LRendererID SkyboxVAO;
	unsigned int SkyboxVBO;

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

        //Renderer2DAPI::Get().As<OpenGLRenderer2D>()->GetFramebuffer()->Bind();
		LRenderer::GetViewportFramebuffer()->Bind();
		glEnable(GL_DEPTH_TEST);

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
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // Cube 2
            ModelMVP = glm::mat4(1.0f);
            ModelMVP = glm::translate(ModelMVP, glm::vec3(2.0f, 0.0f, 0.0f));
            DebugShader->Set("u_Model", ModelMVP);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        // Floor 
        {
			PlaneVertexBuffer->Bind();
			PlaneTexture->Bind();
            //glBindTexture(GL_TEXTURE_2D, FloorTexture);

            DebugShader->Set("u_Model", glm::mat4(1.0f));
            glDrawArrays(GL_TRIANGLES, 0, 6);
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
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // Cube 2
            ModelMVP = glm::mat4(1.0f);
            ModelMVP = glm::translate(ModelMVP, glm::vec3(2.0f, 0.0f, 0.0f));
            DebugShader->Set("u_Model", ModelMVP);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        // Floor
        {
            glBindVertexArray(PlaneVAO);
			PlaneTexture->Bind();
            DebugShader->Set("u_Model", glm::mat4(1.0f));
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }

        // Draw the mirror quad with screen texture.
		// Disable depth test so screen-space quad isn't discarded due to depth test.
        glDisable(GL_DEPTH_TEST); 
        ScreenShader->Bind();
        TObjectPtr<LImage2D> colorAttachment0 = LRenderer::GetViewportFramebuffer()->GetImage(0);
        glBindTexture(GL_TEXTURE_2D, colorAttachment0->GetRendererID());	// use the color attachment texture as the texture of the quad plane
        glDrawArrays(GL_TRIANGLES, 0, 6);

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

    void GenerateCubeVaoAndVbo(unsigned int& vao, unsigned int& vbo)
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

    void GeneratePlaneVaoAndVbo(unsigned int& vao, unsigned int& vbo)
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

    void GenerateScreenQuadVaoAndVbo(unsigned int& vao, unsigned int& vbo)
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