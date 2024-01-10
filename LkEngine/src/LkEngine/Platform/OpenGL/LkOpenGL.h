#pragma once
#ifdef LK_RENDERER_API_OPENGL

#include <glad/glad.h>

namespace LkEngine {

    // OpenGL error handling functions
    void OpenGL_ClearError();
    bool OpenGL_LogCall(const char* function, const char* file, int line);
    const char* OpenGL_GetVersion();

    // OpenGL call macro to use for invoking opengl functions, provides error handling
    #define GL_CALL(_FUNC) OpenGL_ClearError(); _FUNC; LK_ASSERT(OpenGL_LogCall(#_FUNC, __FILE__, __LINE__))

    constexpr int OpenGL_Major_Version = 4;
    constexpr int OpenGL_Minor_Version = 5;
    constexpr const char* OpenGL_GLSL_33 = "#version 330";
    constexpr const char* OpenGL_GLSL_45 = "#version 450";

}

constexpr int LK_RENDERER_API_MAJOR_VERSION = LkEngine::OpenGL_Major_Version;
constexpr int LK_RENDERER_API_MINOR_VERSION = LkEngine::OpenGL_Minor_Version;
constexpr const char* LK_SHADER_VERSION = LkEngine::OpenGL_GLSL_45;


#endif // LK_RENDERER_API_OPENGL