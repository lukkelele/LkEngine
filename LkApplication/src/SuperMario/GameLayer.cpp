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
        m_Renderer2D = Renderer::GetRenderer2D();
        m_Scene = std::make_shared<Scene>();
        m_Player = std::make_shared<Player>("Mario");
        LOG_DEBUG("Created new player: {}", m_Player->GetName());

        Entity playerEntity = m_Scene->CreateEntity("Player");
        Debug::CreateDebugSprite(*m_Scene, { 120, 180 }, { 400, 500 });

        m_Player->SetEntity(playerEntity);
        LOG_DEBUG("Player UUID set to: {}", m_Player->GetUUID());
        Camera& cam = playerEntity.GetComponent<CameraComponent>();
        m_Scene->SetActiveCamera(cam);

        for (int i = 0; i < 2; i++)
        {
            Entity enemyEntity = m_Scene->CreateEntity("Enemy-" + std::to_string(i));
            s_ptr<Enemy> enemy = std::make_shared<Enemy>(enemyEntity);
            m_Enemies.push_back(enemy);
        }
    }

    void GameLayer::OnDetach()
    {
    }

    void GameLayer::OnUpdate(float ts)
    {
        // Main Character 
        Entity playerEntity = m_Player->GetEntity();
        Camera& playerCam = playerEntity.GetComponent<CameraComponent>();

        m_Renderer2D->BeginScene(playerCam);
        m_Scene->BeginScene();
        Input::HandleScene(*m_Scene); // Raycast handling

        //RenderCommand::DrawQuad({ 300, 400 }, { 100, 200 }, { 0.80f, 0.10f, 0.20f, 1.0f });
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