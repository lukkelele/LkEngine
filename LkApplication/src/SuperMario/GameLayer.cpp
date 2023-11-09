#include "GameLayer.h"
#include "Enemy.h"


namespace LkEngine {

    GameLayer::GameLayer()
        : Layer("GameLayer")
    {
    }

    GameLayer::~GameLayer()
    {
    }

    void GameLayer::OnAttach()
    {
        m_Renderer2D = std::shared_ptr<Renderer2D>(Renderer::GetRenderer2D());
        m_Scene = std::make_shared<Scene>();
        m_Player = std::make_shared<Player>("Mario");
        LOG_DEBUG("Created new player: {}", m_Player->GetName());

        Entity playerEntity = m_Scene->CreateEntity("Player");
        m_Player->SetEntity(playerEntity);
        LOG_DEBUG("Player UUID set to: {}", m_Player->GetUUID());
        Camera& cam = playerEntity.GetComponent<CameraComponent>();
        //m_Scene->SetActiveCamera(m_Player->GetEntity().GetComponent<CameraComponent>());
        m_Scene->SetActiveCamera(cam);

        for (int i = 0; i < 3; i++)
        {
            //m_Enemies.push_back(std::make_shared<Enemy>("Enemy-" + std::to_string(i)));
            s_ptr<Enemy> enemy= std::make_shared<Enemy>("Enemy-" + std::to_string(i));
            Entity enemyEntity = m_Scene->CreateEntity(enemy->GetName());
            enemy->SetEntity(enemyEntity);
            m_Enemies.push_back(enemy);
        }
    }

    void GameLayer::OnDetach()
    {
    }

    void GameLayer::OnUpdate(float ts)
    {
        //m_Renderer2D->BeginScene();
	    //RenderCommand::DrawGrid(5, 5, { 0.80f, 0.50f, 0.80f, 0.20f }, 20, 20);
        m_Renderer2D->BeginScene(m_Player->GetEntity().GetComponent<CameraComponent>());
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

        RenderCommand::DrawQuad({ 300, 400 }, { 100, 200 }, { 0.80f, 0.10f, 0.20f, 1.0f });

        m_Player->OnUpdate(ts);

        for (auto& enemy : m_Enemies)
        {
            enemy->OnUpdate(ts);
        }

        m_Scene->EndScene();
        m_Renderer2D->EndScene();
    }

    void GameLayer::OnImGuiRender()
    {
        m_Scene->OnImGuiRender();
    }

}