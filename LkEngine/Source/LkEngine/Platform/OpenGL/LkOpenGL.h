#pragma once

#include <glad/glad.h>

#include "LkEngine/Core/Memory/MemoryUtils.h"

#include "LkEngine/Renderer/BlendingSpecification.h"
#include "LkEngine/Renderer/Image.h"

#include "LkEngine/Utility/StringUtils.h"

#include "LkEngine/Renderer/Framebuffer.h"
#include "LkEngine/Renderer/Shader.h"

#include "LkEngine/Renderer/VertexBuffer.h"



namespace LkEngine {

    class TextureArray;

    /**
     * LK_OpenGL
     * 
     *  Macro for invoking OpenGL functions.
     *  Handles potential errors and logs them before issuing a crash.
     */
    #define LK_OpenGL(_OPENGL_FUNCTION) \
        OpenGL_ClearError(); \
        _OPENGL_FUNCTION; \
        LK_ASSERT(OpenGL_LogCall(#_OPENGL_FUNCTION, __FILE__, __LINE__))


	FORCEINLINE static void OpenGL_ClearError() 
	{ 
		while (glGetError() != GL_NO_ERROR); 
	}

    FORCEINLINE static bool OpenGL_LogCall(const char* InFunction, const char* InFile, int InLine)
    {
        while (GLenum Error = glGetError())
        {
            LK_CORE_ERROR_TAG("OpenGL", "Error: {}   Function: {}   File: {}   Line: {}", Error, InFunction, InFile, InLine);
            return false;
        }

        return true;
    }

	FORCEINLINE static const char* OpenGL_GetVersion()
	{
		char buf[140];
		//sprintf(buf, "%s", glGetString(GL_VERSION));
		sprintf_s(buf, ARRAYSIZE(buf), "%s", glGetString(GL_VERSION));

		return buf;
	}

    constexpr int OpenGL_Major_Version = 4;
    constexpr int OpenGL_Minor_Version = 5;
    constexpr const char* OpenGL_GLSL_33 = "#version 330";
    constexpr const char* OpenGL_GLSL_45 = "#version 450";

    namespace GLUtils {

        int GetOpenGLSourceBlendFunction(const ESourceBlendFunction& InSourceBlendFunction);
        int GetOpenGLDestinationBlendFunction(const EDestinationBlendFunction& InDestinationBlendFunction);

        const GLubyte* GetExtensions();
        void PrintOpenGLExtensions();

        GLenum OpenGLFormatDataType(EImageFormat Format);

        FORCEINLINE static GLenum OpenGLImageFormat(const EImageFormat ImageFormat)
        {
            switch (ImageFormat)
            {
                case EImageFormat::RGB:     return GL_RGB;

                /* RGBA */
                case EImageFormat::RGBA:
                case EImageFormat::RGBA8:
                case EImageFormat::RGBA16F:
                case EImageFormat::RGBA32F: return GL_RGBA;

                /* SRGB */
                case EImageFormat::SRGB:    return GL_SRGB;
                case EImageFormat::SRGBA:   return GL_SRGB_ALPHA;
            }

            LK_CORE_ASSERT(false, "Unknown OpenGLImageFormat {}", static_cast<int>(ImageFormat));
            return GL_INVALID_ENUM;
        }

        GLenum OpenGLImageInternalFormat(const EImageFormat Format);
        GLenum OpenGLSamplerWrap(const ETextureWrap TextureWrap);
        GLenum OpenGLSamplerFilter(const ETextureFilter TextureFilter, const bool bEnableMipMap);
        GLenum ImageFormatToGLDataFormat(const EImageFormat Format);
        GLenum ImageFormatToGLInternalFormat(const EImageFormat Format);

        void BindTexture(const bool bMultisampled, const uint32_t TextureID);
        void CreateTextures(const bool bMultisampled, uint32_t* OutTextureID, uint32_t Count);
        GLenum TextureTarget(const bool bMultisampled);

        void AttachColorTexture(const uint32_t id, 
                                const int Samples, 
                                const GLenum InternalFormat, 
                                const GLenum Format, 
                                const uint32_t Width, 
                                const uint32_t Height, 
                                const int Index);

        void AttachDepthTexture(const uint32_t id, 
                                const int Samples, 
                                const GLenum Format, 
                                const GLenum AttachmentType, 
                                const uint32_t Width, 
                                const uint32_t Height);

        bool IsDepthFormat(const EImageFormat Format);
        GLenum FramebufferTextureFormatToGL(const EImageFormat Format);

        std::string FramebufferTextureFormatToString(const EFramebufferTextureFormat Format);
        std::string ImageFormatToString(const EImageFormat ImageFormat);

		FORCEINLINE static void ApplyTextureFilter(ETextureFilter TextureFilter, bool bGenerateMipmap)
		{
			if (TextureFilter == ETextureFilter::Linear)
			{
                if (bGenerateMipmap)
                {
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                }
                else
                {
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                }

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
			else if (TextureFilter == ETextureFilter::Nearest)
			{
                if (bGenerateMipmap)
                {
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
                }
                else
                {
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                }

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}
			else if (TextureFilter == ETextureFilter::None)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}
		}

		static void ApplyTextureWrap(ETextureWrap TextureWrap)
		{
			if (TextureWrap == ETextureWrap::Clamp)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
			}
			else if (TextureWrap == ETextureWrap::Repeat)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // S: x
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // T: y
			}
		}

		static void ApplyTextureFilter(const RendererID& rendererID, const ETextureFilter TextureFilter, bool bGenerateMipmap = true)
		{
            switch (TextureFilter)
            {
                case ETextureFilter::Linear:
			    {
                    if (bGenerateMipmap)
                    {
			    		glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                    }
                    else
                    {
			    		glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    }

			    	glTextureParameteri(rendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                    break;
			    }

                case ETextureFilter::Nearest:
			    {
            #if 0
                    if (bGenerateMipmap)
                    {
			    		glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
                    }
                    else
                    {
			    		glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    }
            #endif
			    	LK_OpenGL(glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, (bGenerateMipmap ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST)));

			    	glTextureParameteri(rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

                    break;
			    }

                case ETextureFilter::None:
			    {
            #if 0
                    if (bGenerateMipmap)
                    {
			    		glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
                    }
                    else
                    {
			    	    glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                    }
            #endif
			    	LK_OpenGL(glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, (bGenerateMipmap ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST)));
			    	LK_OpenGL(glTextureParameteri(rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

                    return;
			    }
            }
		}

		static void ApplyTextureWrap(const RendererID rendererID, const ETextureWrap TextureWrap)
		{
			switch (TextureWrap)
			{
				case ETextureWrap::None:
				{
					glTextureParameteri(rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT); 
					glTextureParameteri(rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT); 
                    break;
				}

				case ETextureWrap::Clamp:
				{
					glTextureParameteri(rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT); 
					glTextureParameteri(rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT); 
                    break;
				}

				case ETextureWrap::Repeat:
				{
					glTextureParameteri(rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTextureParameteri(rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT); 
                    break;
				}

                default: 
                    LK_CORE_ASSERT(false, "Unknown TextureWrap {}", static_cast<int>(TextureWrap));
                    return;
			}
		}

        /**
		 * Depth is returned normalized i.e in range { 0.0, 1.0 }
         */
		FORCEINLINE static float SampleDepth(int x, int y, int WindowWidth, int WindowHeight) 
		{
			GLint Viewport[4];
			glGetIntegerv(GL_VIEWPORT, Viewport);

			/* Convert screen coordinates to OpenGL viewport coordinates. */
			y = WindowHeight - y;

			float Depth = 0.0f;
			glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &Depth);

			return Depth;
		}

    }

    //=====================================================================
    // Debugging
    extern unsigned int CubeTexture_, FloorTexture_;

    extern unsigned int CubeVAO, CubeVBO;
    extern unsigned int PlaneVAO, PlaneVBO;
    extern unsigned int QuadVAO, QuadVBO;
    extern RendererID SkyboxVAO;
    extern unsigned int SkyboxVBO;

    extern TObjectPtr<LVertexBuffer> CubeVertexBuffer;
    extern TObjectPtr<LVertexBuffer> PlaneVertexBuffer;
    extern TObjectPtr<LTexture2D> CubeTexture;
    extern TObjectPtr<LTexture2D> PlaneTexture;

    inline static TObjectPtr<LShader> ScreenShader = nullptr;
    inline static TObjectPtr<LShader> DebugShader = nullptr;

    // Skybox
    inline static TObjectPtr<LVertexBuffer> SkyboxVertexBuffer;
    inline static TObjectPtr<LTextureCube> SkyboxTexture;
    inline static TObjectPtr<LShader> SkyboxShader;
    inline static unsigned int CubemapTexture;

    inline static glm::mat4 ModelMVP = glm::mat4(1.0f);
    inline static glm::mat4 View = glm::mat4(1.0f);
    inline static glm::mat4 Projection = glm::mat4(1.0f);

    static float Cube_Vertices[] = {
        // Positions         // Texture Coords
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,    1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,    0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,    1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,    0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,    0.0f, 1.0f
    };
    static float Cube_TextureCoords[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
    
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
    
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
    
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
    
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
    
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f
    };

    static float Plane_Vertices[] = {
        // Positions          // Texture Coords 
         5.0f, -0.5f,  5.0f,     2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,     0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,     0.0f, 2.0f,
    
         5.0f, -0.5f,  5.0f,     2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,     0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,     2.0f, 2.0f
    };
    static float Quad_Vertices[] = { 
        // positions   // texCoords
        -0.3f,  1.0f,  0.0f, 1.0f,
        -0.3f,  0.7f,  0.0f, 0.0f,
         0.3f,  0.7f,  1.0f, 0.0f,
    
        -0.3f,  1.0f,  0.0f, 1.0f,
         0.3f,  0.7f,  1.0f, 0.0f,
         0.3f,  1.0f,  1.0f, 1.0f
    };
    static float Skybox_Vertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    static unsigned int LoadTexture(const char* path)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int Width, Height, channels;
        unsigned char* data = stbi_load(path, &Width, &Height, &channels, 0);
        if (data)
        {
            GLenum Format;
            if (channels == 1)
                Format = GL_RED;
            else if (channels == 3)
                Format = GL_RGB;
            else if (channels == 4)
                Format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, Format, Width, Height, 0, Format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            LK_CORE_ERROR_TAG("OpenGLRenderer2D", "Texture failed to load at path {}", path);
            stbi_image_free(data);
        }
        return textureID;
    }

    void GenerateCubeVaoAndVbo(unsigned int& vao, unsigned int& vbo);
    void GeneratePlaneVaoAndVbo(unsigned int& vao, unsigned int& vbo);
    void GenerateScreenQuadVaoAndVbo(unsigned int& vao, unsigned int& vbo);
    void SetupTexturesAndShaders();

    void RenderMirrorTexture(const glm::mat4& view = glm::mat4(1.0f), const glm::mat4& proj = glm::mat4(1.0f));
    void RenderScreenTexture(const glm::mat4& view = glm::mat4(1.0f), const glm::mat4& proj = glm::mat4(1.0f));
    void RenderCubes(const glm::mat4& view = glm::mat4(1.0f), const glm::mat4& proj = glm::mat4(1.0f));
    void RenderFloor(const glm::mat4& view = glm::mat4(1.0f), const glm::mat4& proj = glm::mat4(1.0f));
    
    TObjectPtr<LShader> GetDebugShader();
    TObjectPtr<LShader> GetScreenShader();

	static unsigned int LoadCubemap(std::vector<std::string> faces)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int Width, Height, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
		    unsigned char* data = stbi_load(faces[i].c_str(), &Width, &Height, &nrChannels, 0);
		    if (data)
		    {
		        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
		                     0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
		        );
		        stbi_image_free(data);
		    }
		    else
		    {
                LK_CORE_ASSERT(false, "Failed to load texture {}", faces[i]);
		        stbi_image_free(data);
		    }
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureID;
	}  

    static void GenerateSkybox(RendererID& skyboxVAO, RendererID& skyboxVBO)
    {
        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);
        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Skybox_Vertices), &Skybox_Vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        SkyboxShader = LShader::Create("Assets/Shaders/OpenGL/Skybox.shader");
        SkyboxShader->Bind();
        SkyboxShader->Set("skybox", 0);
    }

    static void RenderSkybox(RendererID& texture, const glm::mat4& view, const glm::mat4& projection)
    {
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        SkyboxShader->Bind();
        SkyboxShader->Set("view", view);
        SkyboxShader->Set("projection", projection);

        // Skybox Cube
        glBindVertexArray(SkyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default
    }

}

constexpr int LK_RENDERER_API_OPENGL_MAJOR_VERSION = LkEngine::OpenGL_Major_Version;
constexpr int LK_RENDERER_API_OPENGL_VERSION = LkEngine::OpenGL_Minor_Version;
constexpr const char* LK_RENDERER_API_OPENGL_SHADER_VERSION = LkEngine::OpenGL_GLSL_45;

