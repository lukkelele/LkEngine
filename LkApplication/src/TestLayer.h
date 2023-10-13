#pragma once
#include "LkEngine/Core/Layer.h"
#include "LkEngine/Renderer/Renderer.h"

using namespace LkEngine;

class TestLayer : public Layer
{
public:
    TestLayer();
    ~TestLayer();

    void OnAttach();
    void OnDetach();
    void OnImGuiRender();

public:
    
};


