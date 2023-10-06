#pragma once
#include "LkEngine/Core/Layer.h"



class TestLayer : public LkEngine::Layer
{
public:
    TestLayer();
    ~TestLayer();

    void OnAttach();
    void OnDetach();
    void OnImGuiRender();
};


