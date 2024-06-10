#pragma once

#include <glad/glad.h>

#include "LkEngine/Renderer/BlendingSpecification.h"
#include "LkEngine/Renderer/Image.h"

#include "LkEngine/Utility/StringUtils.h"
#include "LkEngine/Utility/MemoryUtils.h"

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

        int GetOpenGLSourceBlendFunction(const ESourceBlendFunction& InSourceBlendFunction);
        int GetOpenGLDestinationBlendFunction(const EDestinationBlendFunction& InDestinationBlendFunction);

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

		static void ApplyTextureFilter(TextureFilter filter, bool mipmap)
		{
			// Texture Filter
			if (filter == TextureFilter::Linear)
			{
				if (mipmap)
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				else
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
			else if (filter == TextureFilter::Nearest)
			{
				if (mipmap)
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
				else
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}
			else if (filter == TextureFilter::None)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}
		}

		static void ApplyTextureWrap(TextureWrap wrap)
		{
			if (wrap == TextureWrap::Clamp)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 

			}
			else if (wrap == TextureWrap::Repeat)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // S: x
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // T: y
			}
		}

		static void ApplyTextureFilter(const RendererID& rendererID, const TextureFilter filter, bool mipmap = true)
		{
            switch (filter)
            {
                case TextureFilter::Linear:
			    {
			    	if (mipmap)
			    		glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			    	else
			    		glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			    	glTextureParameteri(rendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    return;
			    }
                case TextureFilter::Nearest:
			    {
			    	if (mipmap)
			    		glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			    	else
			    		glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			    	glTextureParameteri(rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    return;
			    }
                case TextureFilter::None:
			    {
			    	if (mipmap)
			    		glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
                    else
			    	    glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			    	glTextureParameteri(rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    return;
			    }
            }

		}

		static void ApplyTextureWrap(const RendererID& rendererID, const TextureWrap wrap)
		{
			switch (wrap)
			{
				case TextureWrap::None:
				{
					glTextureParameteri(rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT); 
					glTextureParameteri(rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT); 
					return;
				}
				case TextureWrap::Clamp:
				{
					glTextureParameteri(rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT); 
					glTextureParameteri(rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT); 
					return;
				}
				case TextureWrap::Repeat:
				{
					glTextureParameteri(rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTextureParameteri(rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT); 
                    return;
				}
			}
            LK_CORE_ASSERT(false, "Unknown TextureWrap {}", (int)wrap);
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

    extern Ref<VertexBuffer> CubeVertexBuffer;
    extern Ref<VertexBuffer> PlaneVertexBuffer;
    extern Ref<Texture2D> CubeTexture;
    extern Ref<Texture2D> PlaneTexture;

    inline static Ref<Shader> ScreenShader = nullptr;
    inline static Ref<Shader> DebugShader = nullptr;

    // Skybox
    inline static Ref<VertexBuffer> SkyboxVertexBuffer;
    inline static Ref<TextureCube> SkyboxTexture;
    inline static Ref<Shader> SkyboxShader;
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

	static unsigned int LoadCubemap(std::vector<std::string> faces)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
		    unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		    if (data)
		    {
		        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
		                     0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
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

        SkyboxShader = Shader::Create("Assets/Shaders/OpenGL/Skybox.shader");
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

