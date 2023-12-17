#include "TestLayer.h"
#include "LkEngine/Core/Application.h"
#include "LkEngine/Renderer/Renderer.h"
#include "LkEngine/Renderer/Renderer2D.h"
#include "LkEngine/Math/Math.h"
#include "LkEngine/Editor/EditorLayer.h"
#include "LkEngine/Renderer/Camera.h"
#include "LkEngine/Scene/EntityFactory.h"
#include "LkEngine/Physics2D/Physics2D.h"
#include "LkEngine/Renderer/Color.h"


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
    //m_Scene = create_s_ptr<Scene>();
    m_Scene = Scene::Create("TestLayer-Scene");

    EntityFactory::CreateQuad(*m_Scene, { 750, 200 }, { 240, 520 }, Color::Generate());
    EntityFactory::CreateQuad(*m_Scene, { 400, 320 }, { 400, 110 }, Color::Generate());

    auto& renderer2D = Renderer::GetRenderer2D();
    m_Renderer2D = std::shared_ptr<Renderer2D>(renderer2D);
    LK_ASSERT(m_Renderer2D);

    m_TestTexture = Texture::Create("assets/img/atte_square.png");
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
	//RenderCommand::DrawGrid(5, 5, { 0.80f, 0.50f, 0.80f, 0.20f }, 20, 20);

    m_Scene->BeginScene();

    // TODO: Snapping movement with the mouse can switch selected entities 
    //       because the mousepos is used and not translation for each sprite
    auto mousePos = Mouse::GetMousePos();
    auto raycastResults = Physics2D::RaycastFromScreen(*m_Scene);
    //m_Scene->HandleRaycast(raycastResults);
    if (Keyboard::IsKeyPressed(Key::Escape) && EditorLayer::SelectedEntityID != 0)
    {
        EditorLayer::SelectedEntity = { (entt::entity)NULL, &*m_Scene };
        EditorLayer::SelectedEntityID = 0;
    }

    m_Scene->EndScene();
    m_Renderer2D->EndScene();
}

void TestLayer::OnImGuiRender()
{
    m_Scene->OnImGuiRender();
}


