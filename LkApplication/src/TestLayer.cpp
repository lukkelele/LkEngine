#include "TestLayer.h"
#include <LkEngine/UI/UILayer.h>
#include <LkEngine/Math/Math.h>
#include <LkEngine/Application.h>
#include <LkEngine/Renderer/Camera.h>
#include <LkEngine/Scene/EntityFactory.h>


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
    LOG_DEBUG("TestLayer::OnAttach()");
    m_Scene = create_s_ptr<Scene>();

    //EntityFactory::CreateRectangle(*m_Scene, { -100.0, -100.0f }, { 80.0f, 80.0f });
    //EntityFactory::CreateRectangle(*m_Scene, { 20, 20 },   { 140.0f, 140.0f });
    //EntityFactory::CreateRectangle(*m_Scene, { 100, 100 }, { 400, 400.0f });
    EntityFactory::CreateRectangle(*m_Scene, { 500, 500 }, { 840, 840 });
    //EntityFactory::CreateRectangle(*m_Scene, { 0, 0}, { 240, 240 });

    m_Renderer2D = std::make_shared<Renderer2D>();
    m_Renderer2D->Init();
}

void TestLayer::OnDetach()
{
}

void TestLayer::OnUpdate(float ts)
{
    m_Scene->OnUpdate(ts);
    auto mouse_pos = Mouse::GetMousePos();
    Physics2D::Raycast(m_Scene, mouse_pos, mouse_pos);

    auto& cam = *m_Scene->GetActiveCamera();
    glm::vec2 camPos = cam.GetPos();
    m_Renderer2D->BeginScene(cam);
    //m_Renderer2D->DrawQuad({ -0.50f + camPos.x, 0.0f + camPos.y }, { 0.1f, 0.1f }, { 0.8f, 0.2f, 0.3f, 1.0f });
    //m_Renderer2D->DrawQuad({ -0.50f, 0.50f }, { 0.1f, 0.1f }, { 0.5f, 0.2f, 0.3f, 1.0f });
    m_Renderer2D->DrawQuad({ 50.0f, 50.0f }, { 0.50f, 1.00f }, { 0.1f, 0.8f, 0.5f, 1.0f });
    //m_Renderer2D->DrawQuad({ 0.0f, 0.0f }, { 100.0f, 100.0f }, { 0.1f, 0.8f, 0.5f, 1.0f });
    m_Renderer2D->EndScene();
}

void TestLayer::OnImGuiRender()
{
    m_Scene->OnImGuiRender();
}


