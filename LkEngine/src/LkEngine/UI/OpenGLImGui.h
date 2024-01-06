#pragma once

#include "ImGuiUtilities.h"

#include "LkEngine/Renderer/Texture.h"


namespace LkEngine::UI {

    void InitOpenGLImGui();

    extern s_ptr<Texture> Texture_1;
    extern s_ptr<Texture> Texture_2;

}