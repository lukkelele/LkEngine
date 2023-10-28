#include "TestLayer.h"
#include "LkEngine/Core/Application.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/Renderer2D.h"
#include "LkEngine/Math/Math.h"
#include "LkEngine/Editor/EditorLayer.h"
#include "LkEngine/Renderer/Camera.h"
#include "LkEngine/Scene/EntityFactory.h"
#include "LkEngine/Physics/2D/Physics2D.h"


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
    EntityFactory::CreateRectangle(*m_Scene, { 100, 100 }, { 400, 400.0f });
    //EntityFactory::CreateRectangle(*m_Scene, { 500, 500 }, { 840, 840 });
    //EntityFactory::CreateRectangle(*m_Scene, { 0, 0}, { 240, 240 });

    m_Renderer2D = std::make_shared<Renderer2D>();
    m_Renderer2D->Init();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void TestLayer::OnDetach()
{
}

void TestLayer::OnUpdate(float ts)
{
    m_Scene->OnUpdate(ts);
    auto mousePos = Mouse::GetMousePos();
    //auto raycastResults = Physics2D::Raycast(m_Scene, mousePos, mousePos);
    auto raycastResults = Physics2D::RaycastFromScreen(m_Scene);
    int raycastHits = raycastResults.size();
    if (raycastHits == 1)
    {
        Raycast2DResult raycast = raycastResults.at(0);
        Entity entity = raycast.HitEntity;
        LOG_WARN("Hit: {} -> ({}, {})", entity.GetName().c_str(), mousePos.x, mousePos.y);
        EditorLayer::SelectedEntityID = entity.GetComponent<IDComponent>().ID;
    }
    else if (raycastHits > 1)
    {
        //for (int i = 0; i < raycastHits; i++)
        for (auto& raycast : raycastResults)
        {
            //Raycast2DResult raycast = raycastResults.at(i);
            LOG_DEBUG("(more raycast hits) Hit Entity: {}", (uint32_t)raycast.HitEntity);
        }
    }

    //LOG_DEBUG("Raycast Results: {}", raycastResults.size());

    auto& cam = *m_Scene->GetActiveCamera();
    glm::vec2 camPos = cam.GetPos();
    m_Renderer2D->BeginScene(cam);
    m_Renderer2D->DrawQuad({ 900.0f, 450.0f }, { 140.0f + r * 0.50f, 320.0f }, { 0.5f, 0.2f, 0.3f, 1.0f });
    m_Renderer2D->DrawQuad({ 300.0f, 300.0f }, { 200.0 + 0.50 * r, 50.0f }, { 0.9f, 0.8f, 0.5f, 1.0f });
    m_Renderer2D->DrawQuad({ 100.0f, 100.0f }, { 100 + r, 90.0f }, { 0.9f, 0.1f, 0.8f, 0.45f });
    m_Renderer2D->DrawQuad({ 400.0f, 600.0f }, { 100 + 1.2 * r, 80.0f }, { 0.9f, 0.1f, 0.40f, 1.0f });
    r += 0.00025;
    m_Renderer2D->EndScene();
}

void TestLayer::OnImGuiRender()
{
    m_Scene->OnImGuiRender();
}


