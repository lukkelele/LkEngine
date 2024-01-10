//=======================================================================
// >>> LkEngine
//
// TargetPlatform.h
//
//
//=======================================================================
#pragma once


namespace LkEngine {

    #ifdef LK_RENDERER_API_OPENGL
        #include "LkEngine/Platform/OpenGL/LkOpenGL.h"
    #endif

    #ifdef LK_RENDERER_API_VULKAN
    	#define LK_SHADER_VERSION "NOT DEFINED"
        #define LK_RENDERER_API_MAJOR_VERSION 0
        #define LK_RENDERER_API_MAJOR_VERSION 0
    #endif

}