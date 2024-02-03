#pragma once

#include <glad/glad.h>

#include "LkEngine/Renderer/BlendingSpecification.h"
#include "LkEngine/Renderer/Image.h"

#include "LkEngine/Utilities/StringUtils.h"
#include "LkEngine/Utilities/MemoryUtils.h"

#include "LkEngine/Renderer/Framebuffer.h"
#include "LkEngine/Renderer/Shader.h"

#include "LkEngine/Renderer/VertexBuffer.h"



namespace LkEngine {

    #define GL_CALL(_FUNC) OpenGL_ClearError(); _FUNC; LK_ASSERT(OpenGL_LogCall(#_FUNC, __FILE__, __LINE__))

    class TextureArray;

	static void OpenGL_ClearError() 
	{ 
		while (glGetError() != GL_NO_ERROR); 
	}

    static bool OpenGL_LogCall(const char* function, const char* file, int line)
    {
        while (GLenum error = glGetError())
        {
            LK_CORE_ERROR_TAG("OpenGL", "Error: {}   Function: {}   File: {}   Line: {}", error, function, file, line);
            return false;
        }
        return true;
    }

	static const char* OpenGL_GetVersion()
	{
		char buf[120];
		sprintf(buf, "%s", glGetString(GL_VERSION));
		return buf;
	}

    constexpr int OpenGL_Major_Version = 4;
    constexpr int OpenGL_Minor_Version = 5;
    constexpr const char* OpenGL_GLSL_33 = "#version 330";
    constexpr const char* OpenGL_GLSL_45 = "#version 450";

    namespace GLUtils {

        int GetOpenGLSourceBlendFunction(const SourceBlendFunction& srcFunc);
        int GetOpenGLDestinationBlendFunction(const DestinationBlendFunction& dstFunc);

        const GLubyte* GetExtensions();
        void PrintOpenGLExtensions();

        GLenum OpenGLFormatDataType(ImageFormat format);
        GLenum OpenGLImageFormat(ImageFormat format);
        GLenum OpenGLImageInternalFormat(ImageFormat format);
        GLenum OpenGLSamplerWrap(TextureWrap wrap);
        GLenum OpenGLSamplerFilter(TextureFilter filter, bool mipmap);
        GLenum ImageFormatToGLDataFormat(ImageFormat format);
        GLenum ImageFormatToGLInternalFormat(ImageFormat format);

        void BindTexture(bool multisampled, uint32_t id);
        void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count);
        GLenum TextureTarget(bool multisampled);
        void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index);
        void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height);
        bool IsDepthFormat(ImageFormat format);
        GLenum FramebufferTextureFormatToGL(ImageFormat format);
        std::string FramebufferTextureFormatToString(FramebufferTextureFormat format);
        std::string ImageFormatToString(ImageFormat format);

    }

    //=====================================================================
    // Debugging
    extern unsigned int CubeTexture_, FloorTexture_;

    extern unsigned int CubeVAO, CubeVBO;
    extern unsigned int PlaneVAO, PlaneVBO;
    extern unsigned int QuadVAO, QuadVBO;

    extern Ref<VertexBuffer> CubeVertexBuffer;
    extern Ref<VertexBuffer> PlaneVertexBuffer;
    extern Ref<Texture2D> CubeTexture;
    extern Ref<Texture2D> PlaneTexture;

    inline static Ref<Shader> ScreenShader = nullptr;
    inline static Ref<Shader> DebugShader = nullptr;

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

    static unsigned int LoadTexture(const char* path)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, channels;
        unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
        if (data)
        {
            GLenum format;
            if (channels == 1)
                format = GL_RED;
            else if (channels == 3)
                format = GL_RGB;
            else if (channels == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
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
    
    Ref<Shader> GetDebugShader();
    Ref<Shader> GetScreenShader();

}

constexpr int LK_RENDERER_API_OPENGL_MAJOR_VERSION = LkEngine::OpenGL_Major_Version;
constexpr int LK_RENDERER_API_OPENGL_VERSION = LkEngine::OpenGL_Minor_Version;
constexpr const char* LK_RENDERER_API_OPENGL_SHADER_VERSION = LkEngine::OpenGL_GLSL_45;

