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
    EntityFactory::CreateRectangle(*m_Scene, { 20, 20 },   { 140.0f, 140.0f });
    EntityFactory::CreateRectangle(*m_Scene, { 100, 100 }, { 400, 400.0f });
    EntityFactory::CreateRectangle(*m_Scene, { 500, 500 }, { 840, 840 });
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
    auto& cam = *m_Scene->GetActiveCamera();
    cam.Update(ts);
    glm::vec2 camPos = cam.GetPos();

    m_Renderer2D->BeginScene(cam);
    m_Scene->BeginScene();

    auto mousePos = Mouse::GetMousePos();
    auto raycastResults = Physics2D::RaycastFromScreen(m_Scene);
    int raycastHits = raycastResults.size();
    if (raycastHits == 1)
    {
        Raycast2DResult raycast = raycastResults.at(0);
        Entity entity = raycast.HitEntity;
        uint32_t hitEntityID = entity;
        EditorLayer::SelectedEntityID = raycast.HitEntity.GetUUID();
        //LOG_DEBUG("Selected Entity ID: {}", EditorLayer::SelectedEntityID);
    }
    else if (raycastHits > 1)
    {
        for (const auto& raycast : raycastResults)
        {
            Entity entity = raycast.HitEntity;
            uint64_t hitEntityID = entity.GetUUID();
            if (Mouse::IsButtonPressed(MouseButton::ButtonLeft) && EditorLayer::SelectedEntityID == 0)
            {
                EditorLayer::SelectedEntityID = hitEntityID;
            }
        }
    }
    else if (raycastHits == 0 && Mouse::IsButtonPressed(MouseButton::ButtonLeft)) 
    {
        EditorLayer::SelectedEntityID = 0;
    }

    m_Scene->EndScene();
    m_Renderer2D->EndScene();
}

void TestLayer::OnImGuiRender()
{
    m_Scene->OnImGuiRender();
}


