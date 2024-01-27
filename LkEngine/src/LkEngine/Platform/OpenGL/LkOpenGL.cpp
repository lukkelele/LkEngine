#include "LKpch.h"
#include "LkOpenGL.h"

#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/Renderer2D.h"

#include "OpenGLRenderer.h"
#include "OpenGLRenderer2D.h"
#include "OpenGLShader.h"


namespace LkEngine {

	namespace GLUtils {

		int GetOpenGLSourceBlendFunction(const SourceBlendFunction& srcFunc)
		{
			switch (srcFunc)
			{
				case SourceBlendFunction::Zero:  return GL_ZERO;
				case SourceBlendFunction::One:   return GL_ONE;
				case SourceBlendFunction::Alpha: return GL_ALPHA;
				case SourceBlendFunction::Color: return GL_COLOR;
				case SourceBlendFunction::One_Minus_DestinationAlpha: return GL_ONE_MINUS_DST_ALPHA;
				default: throw std::runtime_error("Source blend function could not be retrieved correctly");
			}
		}
	
		int GetOpenGLDestinationBlendFunction(const DestinationBlendFunction& dstFunc)
		{
			switch (dstFunc)
			{
				case DestinationBlendFunction::Zero:  return GL_ZERO;
				case DestinationBlendFunction::One:   return GL_ONE;
				case DestinationBlendFunction::Alpha: return GL_ALPHA;
				case DestinationBlendFunction::Color: return GL_COLOR;
				case DestinationBlendFunction::One_Minus_SourceAlpha: return GL_ONE_MINUS_SRC_ALPHA;
				default: throw std::runtime_error("Destination blend function could not be retrieved correctly");
			}
		}

		const GLubyte* GetExtensions()
		{
			return glGetString(GL_EXTENSIONS);
		}

		void PrintOpenGLExtensions()
		{
			int n;
			glGetIntegerv(GL_NUM_EXTENSIONS, &n);
			for (int i = 0; i < n; i++)
			{
				const GLubyte* extension = glGetStringi(GL_EXTENSIONS, i);
				LK_CORE_DEBUG_TAG("OpenGL Extension", "{}", extension);
			}
		}

		GLenum OpenGLImageFormat(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGB:     return GL_RGB;
				case ImageFormat::RGBA:
				case ImageFormat::RGBA8:
				case ImageFormat::RGBA16F:
				case ImageFormat::RGBA32F: return GL_RGBA;

				case ImageFormat::SRGB:    return GL_SRGB;
				case ImageFormat::SRGBA:   return GL_SRGB_ALPHA;
			}
			LK_CORE_ASSERT(false, "Unknown OpenGLImageFormat {}", Utils::ImageFormatToString(format));
		}

		GLenum OpenGLImageInternalFormat(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGB:             return GL_RGB8;
				case ImageFormat::RGBA:            return GL_RGBA8;
				case ImageFormat::RGBA8:           return GL_RGBA8;
				case ImageFormat::RGBA16F:         return GL_RGBA16F;
				case ImageFormat::RGBA32F:         return GL_RGBA32F;
				case ImageFormat::DEPTH24STENCIL8: return GL_DEPTH24_STENCIL8;
				case ImageFormat::DEPTH32F:        return GL_DEPTH_COMPONENT32F;
			}
			LK_CORE_ASSERT(false, "Unknown OpenGLImageFormat {}", Utils::ImageFormatToString(format));
		}

		GLenum OpenGLFormatDataType(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGB:
				case ImageFormat::RGBA:    
				case ImageFormat::RGBA8:   return GL_UNSIGNED_BYTE;
				case ImageFormat::RGBA16F:
				case ImageFormat::RGBA32F: return GL_FLOAT;
			}
			LK_CORE_ASSERT(false, "Unknown OpenGLFormatDataType {}", Utils::ImageFormatToString(format));
		}

		GLenum OpenGLSamplerWrap(TextureWrap wrap)
		{
			switch (wrap)
			{
				case TextureWrap::Clamp:   return GL_CLAMP_TO_EDGE;
				case TextureWrap::Repeat:  return GL_REPEAT;
			}
			LK_CORE_ASSERT(false, "Unknown OpenGLSamplerWrap: {}", (int)wrap);
		}

		GLenum OpenGLSamplerFilter(TextureFilter filter, bool mipmap)
		{
			switch (filter)
			{
				case TextureFilter::Linear:  return mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
				case TextureFilter::Nearest: return mipmap ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
			}
			LK_CORE_ASSERT(false, "Unknown OpenGLSamplerFilter: {}, mipmap: {}", (int)filter, mipmap);
		}

		GLenum ImageFormatToGLDataFormat(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGBA:   
				case ImageFormat::RGBA8:   
				case ImageFormat::RGBA16F:   
				case ImageFormat::RGBA32F:  return GL_RGBA;

				case ImageFormat::RG8:     
				case ImageFormat::RG16F:     
				case ImageFormat::RG32F:    return GL_RG;

				case ImageFormat::RGB:  
				case ImageFormat::RGB8:     return GL_RGB;

				case ImageFormat::RED8UI:
				case ImageFormat::RED16UI:
				case ImageFormat::RED32UI:  return GL_RED_INTEGER;
				case ImageFormat::RED32F:   return GL_RED_INTEGER;

			}
			LK_CORE_ASSERT(false, "Unknown ImageFormat: {}", Utils::ImageFormatToString(format));
		}

		GLenum ImageFormatToGLInternalFormat(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGB:      return GL_RGB32F;
				case ImageFormat::RGB8:     return GL_RGB8;

				case ImageFormat::RGBA:     return GL_RGBA32F;
				case ImageFormat::RGBA8:    return GL_RGBA8;
				case ImageFormat::RGBA16F:  return GL_RGBA16F;
				case ImageFormat::RGBA32F:  return GL_RGBA32F;

				case ImageFormat::RG16F:    return GL_RG16F;
				case ImageFormat::RG32F:    return GL_RG32F;

				case ImageFormat::RED8UI:   return GL_R8UI;
				case ImageFormat::RED16UI:  return GL_R16UI;
				case ImageFormat::RED32UI:  return GL_R32UI;
				case ImageFormat::RED32F:   return GL_R32F;
			}
			LK_CORE_ASSERT(false, "Unknown internal ImageFormat: {}", Utils::ImageFormatToString(format));
		}

		GLenum TextureTarget(bool multisampled)
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count)
		{
			glCreateTextures(TextureTarget(multisampled), count, outID);
		}

		void BindTexture(bool multisampled, uint32_t id)
		{
			glBindTexture(TextureTarget(multisampled), id);
		}

		void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
		}

		void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
		}

		GLenum FramebufferTextureFormatToGL(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGBA8:            return GL_RGBA8;
				case ImageFormat::RGBA16F:          return GL_RGBA16F;
				case ImageFormat::RGBA32F:          return GL_RGBA32F;
				case ImageFormat::RED8UI:      
				case ImageFormat::RED8UN:      
				case ImageFormat::RED16UI:       
				case ImageFormat::RED32UI:          return GL_RED_INTEGER;
				case ImageFormat::DEPTH24STENCIL8:  return GL_DEPTH24_STENCIL8;
			}
			LK_CORE_ASSERT(false, "FramebufferTextureFormatToGL: ImageFormat not recognized!");
		}

        std::string FramebufferTextureFormatToString(FramebufferTextureFormat format)
		{
			switch (format)
			{
				case FramebufferTextureFormat::RGBA8:           return "RGBA8";
				case FramebufferTextureFormat::RED_INTEGER:     return "RED_INTEGER";
				case FramebufferTextureFormat::DEPTH24STENCIL8: return "DEPTH24STENCIL8";
				case FramebufferTextureFormat::None:            return "None";
			}
			LK_CORE_ASSERT(false, "Unknown FramebufferTextureFormat");
		};

	}

    //unsigned int FramebufferID;
    unsigned int TextureColorBufferID, DepthTextureID;
    unsigned int CubeTexture, FloorTexture;

    unsigned int CubeVAO, CubeVBO;
    unsigned int PlaneVAO, PlaneVBO;
    unsigned int QuadVAO, QuadVBO;

    void RenderMirrorTexture(const glm::mat4& view, const glm::mat4& proj)
    {
		if (!DebugShader || !ScreenShader)
		{
			ScreenShader = Renderer::GetShaderLibrary()->Get("Renderer2D_Screen");
			DebugShader  = Renderer::GetShaderLibrary()->Get("Renderer2D_Debug");
			return;
		}

        Renderer2DAPI::Get().As<OpenGLRenderer2D>()->GetFramebuffer()->Bind();
        glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        DebugShader->Bind();
        Model = glm::mat4(1.0f);
        DebugShader->Set("View", view);
        DebugShader->Set("Projection", proj);

        // Cubes
        {
            glBindVertexArray(CubeVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, CubeTexture);

            // Cube 1
            Model = glm::translate(Model, glm::vec3(-1.0f, 0.0f, -1.0f));
            DebugShader->Set("Model", Model);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // Cube 2
            Model = glm::mat4(1.0f);
            Model = glm::translate(Model, glm::vec3(2.0f, 0.0f, 0.0f));
            DebugShader->Set("Model", Model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        // Floor 
        {
            glBindVertexArray(PlaneVAO);
            glBindTexture(GL_TEXTURE_2D, FloorTexture);

            DebugShader->Set("Model", glm::mat4(1.0f));
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glBindVertexArray(0);
    }

    void RenderScreenTexture(const glm::mat4& view, const glm::mat4& proj)
    {
		if (!DebugShader || !ScreenShader)
		{
			ScreenShader = Renderer::GetShaderLibrary()->Get("Renderer2D_Screen");
			DebugShader  = Renderer::GetShaderLibrary()->Get("Renderer2D_Debug");
			return;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.1f, 0.2f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Model = glm::mat4(1.0f);
        DebugShader->Bind();
        DebugShader->Set("View", view);
        DebugShader->Set("Projection", proj);

        // Cubes
        {
            // Cube 1
            glBindVertexArray(CubeVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, CubeTexture);
            Model = glm::translate(Model, glm::vec3(-1.0f, 0.0f, -1.0f));
            DebugShader->Set("Model", Model);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // Cube 2
            Model = glm::mat4(1.0f);
            Model = glm::translate(Model, glm::vec3(2.0f, 0.0f, 0.0f));
            DebugShader->Set("Model", Model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        // Floor
        {
            glBindVertexArray(PlaneVAO);
            glBindTexture(GL_TEXTURE_2D, FloorTexture);
            DebugShader->Set("Model", glm::mat4(1.0f));
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }

        // Now draw the mirror quad with screen texture
        glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
        ScreenShader->Bind();
        glBindVertexArray(QuadVAO);

        //glBindTexture(GL_TEXTURE_2D, TextureColorBufferID);	// use the color attachment texture as the texture of the quad plane
        unsigned int colorAttachment0 = Renderer2DAPI::Get().As<OpenGLRenderer2D>()->GetFramebuffer()->GetColorAttachmentRendererID(0);
        glBindTexture(GL_TEXTURE_2D, colorAttachment0);	// use the color attachment texture as the texture of the quad plane
        glDrawArrays(GL_TRIANGLES, 0, 6);

        DebugShader->Unbind();
        ScreenShader->Unbind();
    }

    void GenerateCubeVaoAndVbo(unsigned int& vao, unsigned int& vbo)
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Cube_Vertices), &Cube_Vertices, GL_STATIC_DRAW);

        // Vertex indexing
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

        // Texture indexing
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }

    void GeneratePlaneVaoAndVbo(unsigned int& vao, unsigned int& vbo)
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Plane_Vertices), &Plane_Vertices, GL_STATIC_DRAW);

        // Vertex indexing
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

        // Texture indexing
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }

    void GenerateScreenQuadVaoAndVbo(unsigned int& vao, unsigned int& vbo)
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Quad_Vertices), &Quad_Vertices, GL_STATIC_DRAW);

        // Vertex indexing
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        // Texture indexing
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }

    Ref<Shader> GetDebugShader() { return DebugShader; }
    Ref<Shader> GetScreenShader() { return ScreenShader; }

}