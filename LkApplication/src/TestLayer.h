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

public:
    glm::vec3 Translation;
    glm::vec3 Scale;
    glm::vec3 Pos;
    glm::mat4 Model;
    glm::vec3 RectPos;
    glm::vec4 ColorSlider;
    glm::quat Rot;

    glm::mat4 View;
    glm::mat4 Projection;

    float Yaw, Pitch;

    float ScalerSlider1, ScalerSlider2;

private:
    VertexArray* m_VAO = nullptr;
    VertexBuffer* m_VBO = nullptr;
    IndexBuffer* m_IBO = nullptr;
    Shader* m_Shader = nullptr;

    s_ptr<Scene> m_Scene = nullptr;
};


