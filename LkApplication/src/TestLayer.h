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
    void OnUpdate(float ts = 0.0f);
    void OnImGuiRender();

public:
    glm::vec4 ColorSlider;
    glm::vec3 RectPos;
private:
    VertexArray* m_VAO = nullptr;
    VertexBuffer* m_VBO = nullptr;
    IndexBuffer* m_IBO = nullptr;
    Shader* m_Shader = nullptr;
};


