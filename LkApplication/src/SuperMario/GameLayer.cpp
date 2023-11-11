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

        Entity playerEntity = m_Scene->CreateEntity("Player");
        m_Player = std::make_shared<Player>(playerEntity, "Mario");
        LOG_DEBUG("Created new player: {}, uuid: {}", m_Player->GetName(), m_Player->GetUUID());

        Debug::CreateDebugSprite(*m_Scene, { 120, 180 }, { 400, 500 });

        Camera& cam = playerEntity.GetComponent<CameraComponent>();
        m_Scene->SetActiveCamera(cam);

        float enemyPosY = 0.0f;
        // Create 3 enemies on different y positions
        for (int i = 1; i < 4; i++)
        {
            Entity enemyEntity = m_Scene->CreateEntity("Enemy-" + std::to_string(i));
            s_ptr<Enemy> enemy = std::make_shared<Enemy>(enemyEntity);
            enemyPosY += enemy->GetHeight() * (DebugLayer::s_DebugEntities * 1.50f);
            enemy->SetPos(0, enemyPosY);
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

        for (const auto& enemy : m_Enemies)
        {
            enemy->OnUpdate(ts);
        }

        m_Scene->EndScene();
        m_Renderer2D->EndScene();
    }

    void GameLayer::OnImGuiRender()
    {
        m_Scene->OnImGuiRender();

        for (const auto& enemy : m_Enemies)
        {
            enemy->OnImGuiRender();
        }
    }

}