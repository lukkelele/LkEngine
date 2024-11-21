#pragma once

#include <glad/glad.h>

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"
#include "LkEngine/Core/Memory/MemoryUtils.h"

#include "LkEngine/Renderer/TextureEnum.h"
#include "LkEngine/Renderer/Image.h"
#include "LkEngine/Renderer/Framebuffer.h"
#include "LkEngine/Renderer/Shader.h"
#include "LkEngine/Renderer/VertexBuffer.h"
#include "LkEngine/Renderer/BlendingSpecification.h"
#include "LkEngine/Renderer/ArrayTextureSpecification.h"

#include "LkEngine/Utility/StringUtils.h"


/**
 * LK_OpenGL_Verify
 *
 *  Macro for invoking and verifying the result of OpenGL functions.
 *  Handles potential errors and logs them before issuing a crash.
 */
#define LK_OpenGL_Verify(OpenGLFunction)    \
	LOpenGL_Internal::CheckForErrors(); \
	OpenGLFunction;                         \
	LK_ASSERT(LOpenGL_Internal::VerifyFunctionResult(#OpenGLFunction, __FILE__, __LINE__))

/**
 * LK_OpenGL_Verify_Func
 * 
 *  Internal component for the GL function verification process.
 */
#define LK_OpenGL_Verify_Func(Func, ...) { LK_OpenGL_Verify(Func(__VA_ARGS__)); }

/**
 * Wrap raw OpenGL functions with LK_OpenGL_Verify to reduce clutter in the code
 * by having to do it for every GL call.
 */
#if 0
#define glTexImage2D(...)    LK_OpenGL_Verify_Func(glTexImage2D, __VA_ARGS__)
#define glTexSubImage2D(...) LK_OpenGL_Verify_Func(glTexSubImage2D, __VA_ARGS__)
#endif

namespace LkEngine {

	class LSceneCamera;
	class LArrayTexture;

	/**
	 * LOpenGL_Internal
	 *
	 *  Internal OpenGL namespace, do not use directly.
	 */
	namespace LOpenGL_Internal 
	{
		FORCEINLINE static void CheckForErrors()
		{
			while (glGetError() != GL_NO_ERROR)
			{
			}
		}

		FORCEINLINE static bool VerifyFunctionResult(const char* InFunction, const char* InFile, int InLine)
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

	/**
	 * LOpenGLBase
	 */
	struct LOpenGLBase
	{
		FORCEINLINE static std::string GetVersion()
		{
			char CharBuf[140];
			std::sprintf(CharBuf, "%s", glGetString(GL_VERSION));

			return std::string(CharBuf);
		}
	};

	/**
	 * LOpenGL3
	 *
	 *  Platform implementation for OpenGL3.
	 */
	struct LOpenGL3 : public LOpenGLBase
	{
	};

	/**
	 * LOpenGL4
	 *
	 *  Platform implementation for OpenGL4.
	 */
	struct LOpenGL4 : public LOpenGLBase
	{
		/**
		 * @brief Convert ESourceBlendFunction to OpenGL source blend function.
		 */
		FORCEINLINE static int GetSourceBlendFunction(const ESourceBlendFunction InBlendFunction)
		{
			switch (InBlendFunction)
			{
				case ESourceBlendFunction::Zero:					   return GL_ZERO;
				case ESourceBlendFunction::One:						   return GL_ONE;
				case ESourceBlendFunction::Alpha:					   return GL_SRC_ALPHA;
				case ESourceBlendFunction::Color:					   return GL_SRC_COLOR;
				case ESourceBlendFunction::One_Minus_DestinationAlpha: return GL_ONE_MINUS_DST_ALPHA;
			}

			LK_CORE_ASSERT(false, "Invalid source blend function");
			return -1;
		}

		/**
		 * @brief Convert EDestinationBlendFunction to OpenGL destination blend function.
		 */
		FORCEINLINE static int GetDestinationBlendFunction(const EDestinationBlendFunction InBlendFunction)
		{
			switch (InBlendFunction)
			{
				case EDestinationBlendFunction::Zero:				   return GL_ZERO;
				case EDestinationBlendFunction::One:				   return GL_ONE;
				case EDestinationBlendFunction::Alpha:				   return GL_DST_ALPHA;
				case EDestinationBlendFunction::Color:				   return GL_DST_COLOR;
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
			LK_OpenGL_Verify(glGetIntegerv(GL_NUM_EXTENSIONS, &Extensions));
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
				case EImageFormat::RGBA8:	return GL_UNSIGNED_BYTE;
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
				case EImageFormat::RGB: return GL_RGB;

				/* RGBA. */
				case EImageFormat::RGBA:
				case EImageFormat::RGBA8:
				case EImageFormat::RGBA16F:
				case EImageFormat::RGBA32F: return GL_RGBA;

				/* SRGB. */
				case EImageFormat::SRGB:  return GL_SRGB;
				case EImageFormat::SRGBA: return GL_SRGB_ALPHA;
			}

			LK_CORE_ASSERT(false, "Unknown OpenGLImageFormat {}", static_cast<int>(ImageFormat));
			return GL_INVALID_ENUM;
		}

		FORCEINLINE static GLenum GetImageInternalFormat(const EImageFormat Format)
		{
			switch (Format)
			{
				case EImageFormat::RGB:				return GL_RGB8;
				case EImageFormat::RGBA:			return GL_RGBA8;
				case EImageFormat::RGBA8:			return GL_RGBA8;
				case EImageFormat::RGBA16F:			return GL_RGBA16F;
				case EImageFormat::RGBA32F:			return GL_RGBA32F;
				case EImageFormat::DEPTH24STENCIL8: return GL_DEPTH24_STENCIL8;
				case EImageFormat::DEPTH32F:		return GL_DEPTH_COMPONENT32F;
			}

			LK_CORE_ASSERT(false, "Invalid image format");
			return GL_INVALID_ENUM;
		}

		FORCEINLINE static GLenum GetSamplerWrap(const ETextureWrap TextureWrap)
		{
			switch (TextureWrap)
			{
				case ETextureWrap::Clamp:  return GL_CLAMP_TO_EDGE;
				case ETextureWrap::Repeat: return GL_REPEAT;
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

		FORCEINLINE static GLenum ImageFormatToDataFormat(EImageFormat ImageFormat)
		{
			switch (ImageFormat)
			{
				case EImageFormat::RGBA:
				case EImageFormat::RGBA8:
				case EImageFormat::RGBA16F:
				case EImageFormat::RGBA32F: return GL_RGBA;

				case EImageFormat::RG8:
				case EImageFormat::RG16F:
				case EImageFormat::RG32F:	return GL_RG;

				case EImageFormat::RGB:
				case EImageFormat::RGB8:	return GL_RGB;

				case EImageFormat::RED8UI:
				case EImageFormat::RED16UI:
				case EImageFormat::RED32UI: return GL_RED_INTEGER;
				case EImageFormat::RED32F:	return GL_RED_INTEGER;
			}

			LK_CORE_ASSERT(false, "Invalid ImageFormat: {}", static_cast<int>(ImageFormat));
			return GL_INVALID_VALUE;
		}

		FORCEINLINE static constexpr GLenum ImageFormatToInternalFormat(EImageFormat ImageFormat)
		{
			switch (ImageFormat)
			{
				case EImageFormat::RGB:		return GL_RGB32F;
				case EImageFormat::RGB8:	return GL_RGB8;

				case EImageFormat::RGBA:	return GL_RGBA32F;
				case EImageFormat::RGBA8:	return GL_RGBA8;
				case EImageFormat::RGBA16F: return GL_RGBA16F;
				case EImageFormat::RGBA32F: return GL_RGBA32F;

				case EImageFormat::RG16F:	return GL_RG16F;
				case EImageFormat::RG32F:	return GL_RG32F;

				case EImageFormat::RED8UI:	return GL_R8UI;
				case EImageFormat::RED16UI: return GL_R16UI;
				case EImageFormat::RED32UI: return GL_R32UI;
				case EImageFormat::RED32F:	return GL_R32F;
			}

			LK_CORE_ASSERT(false, "Invalid internal ImageFormat: {}", static_cast<int>(ImageFormat));
			return GL_INVALID_VALUE;
		}

		FORCEINLINE static constexpr GLenum ShaderDataTypeToOpenGLBaseType(EShaderDataType type)
		{
			switch (type)
			{
				case EShaderDataType::Float:  return GL_FLOAT;
				case EShaderDataType::Float2: return GL_FLOAT;
				case EShaderDataType::Float3: return GL_FLOAT;
				case EShaderDataType::Float4: return GL_FLOAT;
				case EShaderDataType::Mat3:	  return GL_FLOAT;
				case EShaderDataType::Mat4:	  return GL_FLOAT;
				case EShaderDataType::Int:
				case EShaderDataType::Int2:
				case EShaderDataType::Int3:
				case EShaderDataType::Int4:	  return GL_INT;
				case EShaderDataType::Bool:	  return GL_BOOL;
			}

			return GL_INVALID_ENUM;
		}

		FORCEINLINE static GLenum TextureTarget(const bool IsMultisampled)
		{
			return (IsMultisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D);
		}

		FORCEINLINE static void CreateTextures(const bool bMultisampled, uint32_t* OutTextureID, const uint32_t Count)
		{
			LK_OpenGL_Verify(
				glCreateTextures(TextureTarget(bMultisampled), Count, OutTextureID)
			);
		}

		FORCEINLINE static void BindTexture(const bool bMultisampled, const uint32_t ID)
		{
			LK_OpenGL_Verify(
				glBindTexture(TextureTarget(bMultisampled), ID)
			);
		}

		FORCEINLINE static void AttachColorTexture(const uint32_t ID, const int Samples, const GLenum InternalFormat,
												   const GLenum Format, const uint32_t Width, const uint32_t Height,
												   const int Index)
		{
			const bool bMultisampled = (Samples > 1);
			if (bMultisampled)
			{
				LK_OpenGL_Verify(
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Samples, InternalFormat, Width, Height, GL_FALSE)
				);
			}
			else
			{
				LK_OpenGL_Verify(glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, Width, Height, 0, Format, GL_UNSIGNED_BYTE, nullptr));
				LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
				LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
				LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
				LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
				LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
			}

			LK_OpenGL_Verify(
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + Index, TextureTarget(bMultisampled), ID, 0)
			);
		}

		FORCEINLINE static void AttachDepthTexture(const uint32_t ID, const int Samples, const GLenum Format,
												   const GLenum AttachmentType, const uint32_t Width, const uint32_t Height)
		{
			const bool bMultisampled = (Samples > 1);
			if (bMultisampled)
			{
				LK_OpenGL_Verify(
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Samples, Format, Width, Height, GL_FALSE)
				);
			}
			else
			{
				LK_OpenGL_Verify(glTexStorage2D(GL_TEXTURE_2D, 1, Format, Width, Height));
				LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
				LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
				LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
				LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
				LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
			}

			LK_OpenGL_Verify(glFramebufferTexture2D(GL_FRAMEBUFFER, AttachmentType, TextureTarget(bMultisampled), ID, 0));
		}

		FORCEINLINE static GLenum GetFramebufferTextureFormat(const EImageFormat ImageFormat)
		{
			switch (ImageFormat)
			{
				case EImageFormat::RGBA8:			return GL_RGBA8;
				case EImageFormat::RGBA16F:			return GL_RGBA16F;
				case EImageFormat::RGBA32F:			return GL_RGBA32F;
				case EImageFormat::RED8UI:
				case EImageFormat::RED8UN:
				case EImageFormat::RED16UI:
				case EImageFormat::RED32UI:			return GL_RED_INTEGER;
				case EImageFormat::DEPTH24STENCIL8: return GL_DEPTH24_STENCIL8;
			}

			LK_CORE_ASSERT(false, "Invalid ImageFormat");
			return GL_INVALID_VALUE;
		}

		FORCEINLINE static void ApplyTextureFilter(ETextureFilter TextureFilter, bool bGenerateMipmap)
		{
			if (TextureFilter == ETextureFilter::Linear)
			{
				if (bGenerateMipmap)
				{
					LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
				}
				else
				{
					LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
				}

				LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			}
			else if (TextureFilter == ETextureFilter::Nearest)
			{
				if (bGenerateMipmap)
				{
					LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));
				}
				else
				{
					LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
				}

				LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
			}
			else if (TextureFilter == ETextureFilter::None)
			{
				LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
				LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
			}
		}

		static void ApplyTextureWrap(ETextureWrap TextureWrap)
		{
			if (TextureWrap == ETextureWrap::Clamp)
			{
				/* S: x    Y: y */
				LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
				LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
			}
			else if (TextureWrap == ETextureWrap::Repeat)
			{
				/* S: x    Y: y */
				LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)); /* S: x */
				LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT)); /* T: y */
			}
		}

		static void ApplyTextureFilter(const LRendererID& RendererID, const ETextureFilter TextureFilter,
									   bool bGenerateMipmap = true)
		{
			switch (TextureFilter)
			{
				case ETextureFilter::Linear:
				{
					/* Min Filter. */
					LK_OpenGL_Verify(glTextureParameteri(RendererID, GL_TEXTURE_MIN_FILTER,
												  (bGenerateMipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR)));

					/* Mag Filter. */
					LK_OpenGL_Verify(glTextureParameteri(RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

					break;
				}

				case ETextureFilter::Nearest:
				{
					/* Min Filter. */
					LK_OpenGL_Verify(glTextureParameteri(RendererID, GL_TEXTURE_MIN_FILTER,
												  (bGenerateMipmap ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST)));

					LK_OpenGL_Verify(glTextureParameteri(RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

					break;
				}

				case ETextureFilter::None:
				{
					/* Min Filter. */
					LK_OpenGL_Verify(glTextureParameteri(RendererID, GL_TEXTURE_MIN_FILTER,
												  (bGenerateMipmap ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST)));

					/* Mag Filter. */
					LK_OpenGL_Verify(glTextureParameteri(RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

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
					LK_OpenGL_Verify(glTextureParameteri(RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT));
					LK_OpenGL_Verify(glTextureParameteri(RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT));
					break;
				}

				case ETextureWrap::Clamp:
				{
					LK_OpenGL_Verify(glTextureParameteri(RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT));
					LK_OpenGL_Verify(glTextureParameteri(RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT));
					break;
				}

				case ETextureWrap::Repeat:
				{
					LK_OpenGL_Verify(glTextureParameteri(RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT));
					LK_OpenGL_Verify(glTextureParameteri(RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT));
					break;
				}

				default: LK_CORE_ASSERT(false, "Unknown texture wrap: {}", static_cast<int>(TextureWrap));
			}
		}

		/**
		 * Depth is returned normalized i.e in range { 0.0, 1.0 }
		 */
		FORCEINLINE static float GetSampleDepth(int x, int y, int WindowWidth, int WindowHeight)
		{
			GLint Viewport[4];
			LK_OpenGL_Verify(glGetIntegerv(GL_VIEWPORT, Viewport));

			/* Convert screen coordinates to OpenGL viewport coordinates. */
			y = WindowHeight - y;

			float Depth = 0.0f;
			LK_OpenGL_Verify(glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &Depth));

			return Depth;
		}

		FORCEINLINE static std::pair<int, int> ConvertDimensionsToWidthAndHeight(const EArrayTextureDimension TexDim)
		{
			switch (TexDim)
			{
				case EArrayTextureDimension::Dim_200x200:   return { 200, 200 };
				case EArrayTextureDimension::Dim_512x512:   return { 512, 512 };
				case EArrayTextureDimension::Dim_1024x1024: return { 1024, 1024 };
				case EArrayTextureDimension::Dim_2048x2048: return { 2048, 2048 };
				case EArrayTextureDimension::Dim_4096x4096: return { 4096, 4096 };
			}

			LK_CORE_ASSERT(false, "Unknown OpenGLTextureArrayDimension");
			return {};
		}

		FORCEINLINE static EArrayTextureDimension DetermineDimension(const int InWidth, const int InHeight)
		{
			LK_VERIFY(InWidth == InHeight, "Passed dimension size is not equal");
			switch (InWidth)
			{
				case 200:  return EArrayTextureDimension::Dim_200x200;
				case 512:  return EArrayTextureDimension::Dim_512x512;
				case 1024: return EArrayTextureDimension::Dim_1024x1024;
				case 2048: return EArrayTextureDimension::Dim_2048x2048;
				case 4096: return EArrayTextureDimension::Dim_4096x4096;
			}

			LK_CORE_ASSERT(false, "Failed to determine dimension");
			return {};
		}

		FORCEINLINE static void GenerateTextureArrayImage(const LRendererID ID, const FArrayTextureSpecification& Specification)
		{
			auto [Width, Height] = ConvertDimensionsToWidthAndHeight(Specification.Dimension);
			LK_OpenGL_Verify(glTextureStorage3D(ID, 
					                     Specification.Layers, 
							             ImageFormatToDataFormat(Specification.ImageFormat), 
							             Width, 
							             Height, 
							             0));
		}

        FORCEINLINE static int GetPrimitiveTopology(const EPrimitiveTopology Topology)
        {
            switch (Topology)
            {
                case EPrimitiveTopology::Points:     return GL_POINTS;
                case EPrimitiveTopology::Lines:      return GL_LINES;
                case EPrimitiveTopology::Triangles:  return GL_TRIANGLES;
            }

            LK_CORE_ASSERT(false, "Unknown topology, cannot convert to OpenGL format");
            return GL_INVALID_ENUM;
        }

	};

	#if defined(LK_OPENGL4)
		using LOpenGL = LOpenGL4; 
	#elif defined(LK_OPENGL3)
		using LOpenGL = LOpenGL3; 
	#else
	#	error "Invalid OpenGL version"
	#endif

	//-----------------------------------------------------------
	// LOpenGL Debugging
	//-----------------------------------------------------------
	namespace LOpenGL_Debug 
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

		extern TObjectPtr<LShader> ScreenShader;
		extern TObjectPtr<LShader> DebugShader;
		extern TObjectPtr<LShader> CubeDebugShader;

		/* Skybox. */
		extern TObjectPtr<LVertexBuffer> SkyboxVertexBuffer;
		extern TObjectPtr<LTextureCube> SkyboxTexture;
		extern TObjectPtr<LShader> SkyboxShader;
		extern uint32_t CubemapTexture;

		extern glm::mat4 ModelMVP;
		extern glm::mat4 View;
		extern glm::mat4 Projection;

		/* clang-format off */
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

		/**
		 * Cube vertices with normals and texture coordinates.
		 */
		static float Cube_Vertices_NT[] = {
			/*    Positions               Normals		  Texture Coords   */
			-0.5f, -0.5f, -0.5f,	0.0f,  0.0f, -1.0f,		0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,	0.0f,  0.0f, -1.0f,		1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,	0.0f,  0.0f, -1.0f,		1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,	0.0f,  0.0f, -1.0f,		1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,	0.0f,  0.0f, -1.0f,		0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,	0.0f,  0.0f, -1.0f,		0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,	0.0f,  0.0f, 1.0f,		0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,	0.0f,  0.0f, 1.0f,		1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,	0.0f,  0.0f, 1.0f,		1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,	0.0f,  0.0f, 1.0f,		1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,	0.0f,  0.0f, 1.0f,		0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,	0.0f,  0.0f, 1.0f,		0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,		1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,		1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,		0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,		0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,		0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,		1.0f, 0.0f,

			 0.5f,  0.5f,  0.5f,	1.0f,  0.0f,  0.0f,		1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,	1.0f,  0.0f,  0.0f,		1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,	1.0f,  0.0f,  0.0f,		0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,	1.0f,  0.0f,  0.0f,		0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,	1.0f,  0.0f,  0.0f,		0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,	1.0f,  0.0f,  0.0f,		1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,	0.0f, -1.0f,  0.0f,		0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,	0.0f, -1.0f,  0.0f,		1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,	0.0f, -1.0f,  0.0f,		1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,	0.0f, -1.0f,  0.0f,		1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,	0.0f, -1.0f,  0.0f,		0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,	0.0f, -1.0f,  0.0f,		0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,	0.0f,  1.0f,  0.0f,		0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,	0.0f,  1.0f,  0.0f,		1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,	0.0f,  1.0f,  0.0f,		1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,	0.0f,  1.0f,  0.0f,		1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,	0.0f,  1.0f,  0.0f,		0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,	0.0f,  1.0f,  0.0f,		0.0f, 1.0f
		};

		/* clang-format on */

		FORCEINLINE static uint32_t LoadTexture(const char* path)
		{
			uint32_t TextureID;
			LK_OpenGL_Verify(glGenTextures(1, &TextureID));

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

				LK_OpenGL_Verify(glBindTexture(GL_TEXTURE_2D, TextureID));
				LK_OpenGL_Verify(glTexImage2D(GL_TEXTURE_2D, 0, Format, Width, Height, 0, Format, GL_UNSIGNED_BYTE, data));
				LK_OpenGL_Verify(glGenerateMipmap(GL_TEXTURE_2D));

				LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
				LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
				LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
				LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

				stbi_image_free(data);
			}
			else
			{
				LK_CORE_ERROR_TAG("OpenGLRenderer2D", "Texture failed to load at path {}", path);
				stbi_image_free(data);
			}

			return TextureID;
		}

		/**
		 * @brief Setup debug environment with a plane and two cubes.
		 */
		void InitializeEnvironment();

		void SetupDebugCube();
		void GeneratePlaneVaoAndVbo(uint32_t& vao, uint32_t& vbo);
		void GenerateScreenQuadVaoAndVbo(uint32_t& vao, uint32_t& vbo);
		void SetupTexturesAndShaders();
		void SetupSkybox();

		void RenderMirrorTexture(const glm::mat4& view = glm::mat4(1.0f), const glm::mat4& proj = glm::mat4(1.0f));
		void RenderScreenTexture(const glm::mat4& view = glm::mat4(1.0f), const glm::mat4& proj = glm::mat4(1.0f));
		void RenderCubes(const glm::mat4& view = glm::mat4(1.0f), const glm::mat4& proj = glm::mat4(1.0f));
		void RenderFloor(const glm::mat4& view = glm::mat4(1.0f), const glm::mat4& proj = glm::mat4(1.0f));
		void RenderSkybox(const TObjectPtr<LSceneCamera> Camera);

		TObjectPtr<LShader> GetDebugShader();
		TObjectPtr<LShader> GetScreenShader();

		static uint32_t LoadCubemap(std::vector<std::string> faces)
		{
			uint32_t TextureID;
			glGenTextures(1, &TextureID);
			glBindTexture(GL_TEXTURE_CUBE_MAP, TextureID);

			int Width, Height, Channels;
			for (uint32_t i = 0; i < faces.size(); i++)
			{
				unsigned char* Data = stbi_load(faces[i].c_str(), &Width, &Height, &Channels, 0);
				if (Data)
				{
					LK_OpenGL_Verify(
						glTexImage2D((GL_TEXTURE_CUBE_MAP_POSITIVE_X + i), 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Data)
					);
					stbi_image_free(Data);
				}
				else
				{
					LK_CORE_ASSERT(false, "Failed to load texture {}", faces[i]);
					stbi_image_free(Data);
				}
			}

			LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
			LK_OpenGL_Verify(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

			return TextureID;
		}

		FORCEINLINE static void GenerateSkybox(LRendererID& skyboxVAO, LRendererID& skyboxVBO)
		{
			LK_OpenGL_Verify(glGenVertexArrays(1, &skyboxVAO));
			LK_OpenGL_Verify(glGenBuffers(1, &skyboxVBO));
			LK_OpenGL_Verify(glBindVertexArray(skyboxVAO));
			LK_OpenGL_Verify(glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO));
			LK_OpenGL_Verify(glBufferData(GL_ARRAY_BUFFER, sizeof(Skybox_Vertices), &Skybox_Vertices, GL_STATIC_DRAW));
			LK_OpenGL_Verify(glEnableVertexAttribArray(0));
			LK_OpenGL_Verify(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));

			SkyboxShader = LShader::Create("Assets/Shaders/OpenGL/Skybox.shader");
			SkyboxShader->Bind();
			SkyboxShader->Set("skybox", 0);
		}

		static void RenderSkybox(const LRendererID texture, const glm::mat4& view, const glm::mat4& projection)
		{
			/* Change depth function so depth test passes when values are equal to depth buffer's content. */
			LK_OpenGL_Verify(glDepthFunc(GL_LEQUAL));
			SkyboxShader->Bind();
			SkyboxShader->Set("view", view);
			SkyboxShader->Set("projection", projection);

			// Skybox Cube
			LK_OpenGL_Verify(glBindVertexArray(SkyboxVAO));
			LK_OpenGL_Verify(glActiveTexture(GL_TEXTURE0));
			LK_OpenGL_Verify(glBindTexture(GL_TEXTURE_CUBE_MAP, texture));
			LK_OpenGL_Verify(glDrawArrays(GL_TRIANGLES, 0, 36));
			LK_OpenGL_Verify(glBindVertexArray(0));
			LK_OpenGL_Verify(glDepthFunc(GL_LESS)); /* Set back to default. */
		}

	}

	/*****************************************************************************/

	namespace Enum {

		FORCEINLINE static constexpr const char* ToString(const EArrayTextureDimension TextureArrayDimension)
		{
			switch (TextureArrayDimension)
			{
				case EArrayTextureDimension::Dim_512x512:   return "512x512";
				case EArrayTextureDimension::Dim_1024x1024: return "1024x1024";
				case EArrayTextureDimension::Dim_2048x2048: return "2048x2048";
				case EArrayTextureDimension::Dim_4096x4096: return "4096x4096";
			}

			LK_CORE_ASSERT(false, "Unknown ETextureArrayDimension value: {}", static_cast<int>(TextureArrayDimension));
			return {};
		}

	}

}

