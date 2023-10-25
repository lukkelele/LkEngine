#pragma once
#include "LkEngine/Core/Layer.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Scene/Scene.h"


using namespace LkEngine;


class TestLayer : public Layer
{
public:
    TestLayer();
    ~TestLayer();

    void OnAttach();
    void OnDetach();
    void OnUpdate(float ts = 0.0f);
    void OnImGuiRender();

private:
    s_ptr<Scene> m_Scene = nullptr;
};


