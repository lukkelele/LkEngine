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

    EntityFactory::CreateRectangle(*m_Scene, { 20, 20 },   { 140.0f, 140.0f });
    EntityFactory::CreateRectangle(*m_Scene, { 100, 100 }, { 400, 400.0f });
    EntityFactory::CreateRectangle(*m_Scene, { 500, 500 }, { 840, 840 });

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

    // TODO: Snapping movement with the mouse can switch selected entities 
    //       because the mousepos is used and not translation for each sprite
    auto mousePos = Mouse::GetMousePos();
    auto raycastResults = Physics2D::RaycastFromScreen(m_Scene);
    m_Scene->HandleRaycast(raycastResults);
    if (Keyboard::IsKeyPressed(Key::Escape) && EditorLayer::SelectedEntityID != 0)
    {
        EditorLayer::SelectedEntity = { (entt::entity)NULL, &*m_Scene };
        EditorLayer::SelectedEntityID = 0;
    }

    glm::vec2 p0, p1;
    p0 = { 0.1, 0.3 };
    p1 = { 0.5, 0.5 };
    glm::vec4 lineColor = { 0.0f, 1.0f, 0.5f, 1.0f };
    m_Renderer2D->DrawLine(p0, p1, lineColor);

    m_Scene->EndScene();
    m_Renderer2D->EndScene();
}

void TestLayer::OnImGuiRender()
{
    m_Scene->OnImGuiRender();
}


