#pragma once

#include "LkEngine/UI/Viewport.h"

namespace LkEngine::UI {

    extern ImGuiWindowClass* UIWindowClass;        
    extern ImGuiWindowClass* RendererWindowClass;   

    void BeginMainRenderWindow();
    void EndMainRenderWindow();
    void TopBar();
    void BottomBar();
    void LeftSidebar();
    void RightSidebar();

}