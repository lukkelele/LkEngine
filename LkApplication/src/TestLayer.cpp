#include "TestLayer.h"
#include <iostream>


// TODO: Move to a better location to be used for geometry
static float box_vertices[8] = {
  -0.5f, -0.5f,  // 0
   0.5f, -0.5f,  // 1
   0.5f,  0.5f,  // 2
  -0.5f,  0.5f   // 3
};

static unsigned int box_indices[6] = {
    0, 1, 2,
    2, 3, 0
};


TestLayer::TestLayer()
{
    m_Name = "TestLayer";
}

TestLayer::~TestLayer()
{
}

void TestLayer::OnAttach()
{
    using namespace LkEngine;
    LOG_DEBUG("TestLayer::OnAttach()");
    m_VAO = new VertexArray();
    m_VBO = new VertexBuffer(box_vertices, LK_ARRAYSIZE(box_vertices));
    m_IBO = new IndexBuffer(box_indices, LK_ARRAYSIZE(box_indices));
    VertexBufferLayout layout;
    layout.Push<float>(2);
    m_VAO->AddBuffer(*m_VBO, layout);
    m_Shader = new Shader("assets/shaders/basic_test.shader");
    Renderer::Draw(*m_VAO, *m_IBO, *m_Shader);
    LOG_DEBUG("Created vao, vbo, ibo and shader");
    LOG_DEBUG("IndexBuffer indices: {}", m_IBO->GetCount());
}

void TestLayer::OnDetach()
{
}

void TestLayer::OnUpdate(float ts)
{
    Renderer::Draw(*m_VAO, *m_IBO, *m_Shader);
}

void TestLayer::OnImGuiRender()
{
    ImGui::Text("TestLayer::OnImGuiRender()");
    ImGui::SeparatorText("Separator Text");
}
