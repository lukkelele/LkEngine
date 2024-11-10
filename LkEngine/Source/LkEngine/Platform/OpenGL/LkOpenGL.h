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
    #define LK_OpenGL(OpenGLFunction) \
        LOpenGL_Internal::ClearPendingErrors(); \
        OpenGLFunction; \
        LK_ASSERT(LOpenGL_Internal::SafeFunctionInvoke(#OpenGLFunction, __FILE__, __LINE__))

    /**
     * LOpenGL_Internal
	 * 
	 *  Internal OpenGL namespace, do not use directly.
     */
    namespace LOpenGL_Internal
	{
		FORCEINLINE static void ClearPendingErrors() 
		{ 
			while (glGetError() != GL_NO_ERROR) {}
		}

		FORCEINLINE static bool SafeFunctionInvoke(const char* InFunction, const char* InFile, int InLine)
		{
			while (GLenum Error = glGetError())
			{
				LK_CORE_ERROR_TAG("OpenGL", "Error: {}\n Function: {}\n File: {}\n Line: {}", 
								  Error, InFunction, InFile, InLine);
				return false;
			}

			return true;
		}
	}

    namespace LOpenGL 
    {
		// REMOVE
		static constexpr int Major_Version = 4;
		static constexpr int Minor_Version = 5;
		static constexpr const char* GLSL_33 = "#version 330";
		static constexpr const char* GLSL_45 = "#version 450";

		/* Forward declarations. */
		FORCEINLINE static GLenum TextureTarget(const bool IsMultisampled);

		FORCEINLINE static std::string GetVersion()
		{
			char CharBuf[140];
			std::sprintf(CharBuf, "%s", glGetString(GL_VERSION));

			return std::string(CharBuf);
		}

		/**
		 * @brief Convert ESourceBlendFunction to OpenGL source blend function.
		 */
		FORCEINLINE static int GetSourceBlendFunction(const ESourceBlendFunction& InBlendFunction)
		{
			switch (InBlendFunction)
			{
				case ESourceBlendFunction::Zero:  return GL_ZERO;
				case ESourceBlendFunction::One:   return GL_ONE;
				case ESourceBlendFunction::Alpha: return GL_SRC_ALPHA;
				case ESourceBlendFunction::Color: return GL_SRC_COLOR;
				case ESourceBlendFunction::One_Minus_DestinationAlpha: return GL_ONE_MINUS_DST_ALPHA;
			}

			LK_CORE_ASSERT(false, "Invalid source blend function");
			return -1;
		}
	
		/**
		 * @brief Convert EDestinationBlendFunction to OpenGL destination blend function.
		 */
		FORCEINLINE static int GetDestinationBlendFunction(const EDestinationBlendFunction& InBlendFunction)
		{
			switch (InBlendFunction)
			{
				case EDestinationBlendFunction::Zero:  return GL_ZERO;
				case EDestinationBlendFunction::One:   return GL_ONE;
				case EDestinationBlendFunction::Alpha: return GL_DST_ALPHA;
				case EDestinationBlendFunction::Color: return GL_DST_COLOR;
				case EDestinationBlendFunction::One_Minus_SourceAlpha: return GL_ONE_MINUS_SRC_ALPHA;
			}

			LK_CORE_ASSERT(false, "Invalid destination blend function");
			return -1;
		}

		FORCEINLINE static const GLubyte* GetExtensions() 
        { 
            return glGetString(GL_EXTENSIONS); 
        }

		FORCEINLINE static void PrintOpenGLExtensions()
		{
			int Extensions{};
			LK_OpenGL(glGetIntegerv(GL_NUM_EXTENSIONS, &Extensions));
			for (int Index = 0; Index < Extensions; Index++)
			{
				const GLubyte* Extension = glGetStringi(GL_EXTENSIONS, Index);
				LK_CORE_INFO("OpenGL Extension: {}", std::string(reinterpret_cast<const char*>(Extension)));
			}
		}

		FORCEINLINE static GLenum GetFormatDataType(EImageFormat ImageFormat)
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

        FORCEINLINE static GLenum GetImageFormat(const EImageFormat ImageFormat)
        {
            switch (ImageFormat)
            {
                case EImageFormat::RGB:     return GL_RGB;

                /* RGBA. */
                case EImageFormat::RGBA:
                case EImageFormat::RGBA8:
                case EImageFormat::RGBA16F:
                case EImageFormat::RGBA32F: return GL_RGBA;

                /* SRGB. */
                case EImageFormat::SRGB:    return GL_SRGB;
                case EImageFormat::SRGBA:   return GL_SRGB_ALPHA;
            }

            LK_CORE_ASSERT(false, "Unknown OpenGLImageFormat {}", static_cast<int>(ImageFormat));
            return GL_INVALID_ENUM;
        }

		FORCEINLINE static GLenum GetImageInternalFormat(const EImageFormat Format)
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

			LK_CORE_ASSERT(false, "Invalid image format");
			return GL_INVALID_ENUM;
		}

		FORCEINLINE static GLenum GetSamplerWrap(const ETextureWrap TextureWrap)
		{
			switch (TextureWrap)
			{
				case ETextureWrap::Clamp:   return GL_CLAMP_TO_EDGE;
				case ETextureWrap::Repeat:  return GL_REPEAT;
			}

			LK_CORE_ASSERT(false, "Unknown OpenGLSamplerWrap: {}", static_cast<int>(TextureWrap));
			return GL_INVALID_VALUE;
		}

		FORCEINLINE static GLenum GetSamplerFilter(const ETextureFilter TextureFilter, const bool bUseMipmap)
		{
			switch (TextureFilter)
			{
				case ETextureFilter::Linear:  return bUseMipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
				case ETextureFilter::Nearest: return bUseMipmap ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
			}

			LK_CORE_ASSERT(false, "Unknown OpenGLSamplerFilter: {}, mipmap: {}", static_cast<int>(TextureFilter), bUseMipmap);
			return GL_INVALID_VALUE;
		}


		FORCEINLINE static GLenum ImageFormatToGLDataFormat(EImageFormat ImageFormat)
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

		FORCEINLINE static GLenum ImageFormatToGLInternalFormat(EImageFormat ImageFormat)
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

		FORCEINLINE static void CreateTextures(const bool bMultisampled, uint32_t* OutTextureID, const uint32_t Count)
		{
			LK_OpenGL(glCreateTextures(TextureTarget(bMultisampled), Count, OutTextureID));
		}

		FORCEINLINE static void BindTexture(const bool bMultisampled, const uint32_t ID)
		{
			LK_OpenGL(glBindTexture(TextureTarget(bMultisampled), ID));
		}

		FORCEINLINE static GLenum TextureTarget(const bool IsMultisampled)
		{
			return (IsMultisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D);
		}

		FORCEINLINE static void AttachColorTexture(const uint32_t ID, const int Samples, 
                                                   const GLenum InternalFormat, const GLenum Format, 
                                                   const uint32_t Width, const uint32_t Height, const int Index)
		{
			const bool bMultisampled = (Samples > 1);
			if (bMultisampled)
			{
				LK_OpenGL(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Samples, InternalFormat, Width, Height, GL_FALSE));
			}
			else
			{
				LK_OpenGL(glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, Width, Height, 0, Format, GL_UNSIGNED_BYTE, nullptr));
				LK_OpenGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
				LK_OpenGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
				LK_OpenGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
				LK_OpenGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
				LK_OpenGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
			}

			LK_OpenGL(glFramebufferTexture2D(
                GL_FRAMEBUFFER, 
                GL_COLOR_ATTACHMENT0 + Index, 
                TextureTarget(bMultisampled), 
                ID, 
                0)
            );
		}

		FORCEINLINE static void AttachDepthTexture(const uint32_t ID, const int Samples, 
                                                   const GLenum Format, const GLenum AttachmentType, 
                                                   const uint32_t Width, const uint32_t Height)
		{
			const bool bMultisampled = (Samples > 1);
			if (bMultisampled)
			{
				LK_OpenGL(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Samples, Format, Width, Height, GL_FALSE));
			}
			else
			{
				LK_OpenGL(glTexStorage2D(GL_TEXTURE_2D, 1, Format, Width, Height));
				LK_OpenGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
				LK_OpenGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
				LK_OpenGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
				LK_OpenGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
				LK_OpenGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
			}

			LK_OpenGL(glFramebufferTexture2D(GL_FRAMEBUFFER, AttachmentType, TextureTarget(bMultisampled), ID, 0));
		}

        std::string FramebufferTextureFormatToString(const EFramebufferTextureFormat Format);

		FORCEINLINE static GLenum FramebufferTextureFormatToGL(const EImageFormat ImageFormat)
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

			LK_CORE_ASSERT(false, "Invalid ImageFormat");
			return GL_INVALID_VALUE;
		}

        FORCEINLINE static std::string FramebufferTextureFormatToString(const EFramebufferTextureFormat Format)
		{
			switch (Format)
			{
				case EFramebufferTextureFormat::RGBA8:           return "RGBA8";
				case EFramebufferTextureFormat::RED_INTEGER:     return "RED_INTEGER";
				case EFramebufferTextureFormat::DEPTH24STENCIL8: return "DEPTH24STENCIL8";
				case EFramebufferTextureFormat::None:            return "None";
			}

			LK_CORE_ASSERT(false, "Invalid FramebufferTextureFormat");
			return "";
		};

		FORCEINLINE static void ApplyTextureFilter(ETextureFilter TextureFilter, bool bGenerateMipmap)
		{
			if (TextureFilter == ETextureFilter::Linear)
			{
                if (bGenerateMipmap)
                {
					LK_OpenGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
                }
                else
                {
					LK_OpenGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
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

		static void ApplyTextureFilter(const LRendererID& RendererID, const ETextureFilter TextureFilter, bool bGenerateMipmap = true)
		{
            switch (TextureFilter)
            {
                case ETextureFilter::Linear:
			    {
					/* Min Filter. */
					LK_OpenGL(
						glTextureParameteri(RendererID, 
											GL_TEXTURE_MIN_FILTER, 
											(bGenerateMipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR))
					);

					/* Mag Filter. */
					LK_OpenGL(glTextureParameteri(RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR) );

                    break;
			    }

                case ETextureFilter::Nearest:
			    {
					/* Min Filter. */
			    	LK_OpenGL(
						glTextureParameteri(
							RendererID, 
							GL_TEXTURE_MIN_FILTER, 
							(bGenerateMipmap ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST)
						)
					);

			    	LK_OpenGL(glTextureParameteri(RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

                    break;
			    }

                case ETextureFilter::None:
			    {
					/* Min Filter. */
			    	LK_OpenGL(
						glTextureParameteri(
								RendererID, 
								GL_TEXTURE_MIN_FILTER, 
								(bGenerateMipmap ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST)
						)
					);

					/* Mag Filter. */
			    	LK_OpenGL(glTextureParameteri(RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

                    return;
			    }
            }
		}

		FORCEINLINE static void ApplyTextureWrap(const LRendererID RendererID, const ETextureWrap TextureWrap)
		{
			switch (TextureWrap)
			{
				case ETextureWrap::None:
				{
					LK_OpenGL(glTextureParameteri(RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT)); 
					LK_OpenGL(glTextureParameteri(RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT)); 
                    break;
				}

				case ETextureWrap::Clamp:
				{
					LK_OpenGL(glTextureParameteri(RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT)); 
					LK_OpenGL(glTextureParameteri(RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT)); 
                    break;
				}

				case ETextureWrap::Repeat:
				{
					LK_OpenGL(glTextureParameteri(RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT));
					LK_OpenGL(glTextureParameteri(RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT)); 
                    break;
				}

                default: 
                    LK_CORE_ASSERT(false, "Unknown texture wrap: {}", static_cast<int>(TextureWrap));
			}
		}

        /**
		 * Depth is returned normalized i.e in range { 0.0, 1.0 }
         */
		FORCEINLINE static float SampleDepth(int x, int y, 
											 int WindowWidth, int WindowHeight) 
		{
			GLint Viewport[4];
			LK_OpenGL(glGetIntegerv(GL_VIEWPORT, Viewport));

			/* Convert screen coordinates to OpenGL viewport coordinates. */
			y = WindowHeight - y;

			float Depth = 0.0f;
			LK_OpenGL(glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &Depth));

			return Depth;
		}

    }

	// TODO: REMOVE!!!!!!!!!
    //=====================================================================
    // Debugging

	namespace LOpenGL::Debug
	{
		extern uint32_t CubeTexture_;
		extern uint32_t FloorTexture_;

		extern uint32_t CubeVAO;
		extern uint32_t CubeVBO;
		extern uint32_t PlaneVAO;
		extern uint32_t PlaneVBO;
		extern uint32_t QuadVAO;
		extern uint32_t QuadVBO;
		extern LRendererID SkyboxVAO;
		extern uint32_t SkyboxVBO;

		extern TObjectPtr<LVertexBuffer> CubeVertexBuffer;
		extern TObjectPtr<LVertexBuffer> PlaneVertexBuffer;
		extern TObjectPtr<LTexture2D> CubeTexture;
		extern TObjectPtr<LTexture2D> PlaneTexture;

		inline static TObjectPtr<LShader> ScreenShader = nullptr;
		inline static TObjectPtr<LShader> DebugShader = nullptr;

		/* Skybox. */
		inline static TObjectPtr<LVertexBuffer> SkyboxVertexBuffer;
		inline static TObjectPtr<LTextureCube> SkyboxTexture;
		inline static TObjectPtr<LShader> SkyboxShader;
		inline static uint32_t CubemapTexture;

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
		/* Positions */    /* Texture Coordinates */
			-0.3f,  1.0f,        0.0f, 1.0f,
			-0.3f,  0.7f,        0.0f, 0.0f,
			 0.3f,  0.7f,        1.0f, 0.0f,
		
			-0.3f,  1.0f,        0.0f, 1.0f,
			 0.3f,  0.7f,        1.0f, 0.0f,
			 0.3f,  1.0f,        1.0f, 1.0f
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

		FORCEINLINE static uint32_t LoadTexture(const char* path)
		{
			uint32_t TextureID;
			LK_OpenGL(glGenTextures(1, &TextureID));

			int Width;
			int Height;
			int Channels;
			unsigned char* data = stbi_load(path, &Width, &Height, &Channels, 0);
			if (data)
			{
				GLenum Format;
				if (Channels == 1)
				{
					Format = GL_RED;
				}
				else if (Channels == 3)
				{
					Format = GL_RGB;
				}
				else if (Channels == 4)
				{
					Format = GL_RGBA;
				}
				else
				{
					LK_CORE_ASSERT(false, "Invalid GLFormat");
					Format = GL_INVALID_ENUM;
				}

				LK_OpenGL(glBindTexture(GL_TEXTURE_2D, TextureID));
				LK_OpenGL(glTexImage2D(GL_TEXTURE_2D, 0, Format, Width, Height, 0, Format, GL_UNSIGNED_BYTE, data));
				LK_OpenGL(glGenerateMipmap(GL_TEXTURE_2D));

				LK_OpenGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
				LK_OpenGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
				LK_OpenGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
				LK_OpenGL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

				stbi_image_free(data);
			}
			else
			{
				LK_CORE_ERROR_TAG("OpenGLRenderer2D", "Texture failed to load at path {}", path);
				stbi_image_free(data);
			}

			return TextureID;
		}

		void GenerateCubeVaoAndVbo(uint32_t& vao, uint32_t& vbo);
		void GeneratePlaneVaoAndVbo(uint32_t& vao, uint32_t& vbo);
		void GenerateScreenQuadVaoAndVbo(uint32_t& vao, uint32_t& vbo);
		void SetupTexturesAndShaders();

		void RenderMirrorTexture(const glm::mat4& view = glm::mat4(1.0f), const glm::mat4& proj = glm::mat4(1.0f));
		void RenderScreenTexture(const glm::mat4& view = glm::mat4(1.0f), const glm::mat4& proj = glm::mat4(1.0f));
		void RenderCubes(const glm::mat4& view = glm::mat4(1.0f), const glm::mat4& proj = glm::mat4(1.0f));
		void RenderFloor(const glm::mat4& view = glm::mat4(1.0f), const glm::mat4& proj = glm::mat4(1.0f));
    
		TObjectPtr<LShader> GetDebugShader();
		TObjectPtr<LShader> GetScreenShader();

		static uint32_t LoadCubemap(std::vector<std::string> faces)
		{
			uint32_t TextureID;
			glGenTextures(1, &TextureID);
			glBindTexture(GL_TEXTURE_CUBE_MAP, TextureID);

			int Width, Height, nrChannels;
			for (uint32_t i = 0; i < faces.size(); i++)
			{
				unsigned char* data = stbi_load(faces[i].c_str(), &Width, &Height, &nrChannels, 0);
				if (data)
				{
					glTexImage2D(
						GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
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

			return TextureID;
		}  

		static void GenerateSkybox(LRendererID& skyboxVAO, LRendererID& skyboxVBO)
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

		static void RenderSkybox(LRendererID& texture, const glm::mat4& view, const glm::mat4& projection)
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

}

