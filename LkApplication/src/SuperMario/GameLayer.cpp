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

        TextureSpecification bgTextureSpecification;
        bgTextureSpecification.Width = Window::Get()->GetWidth();
        bgTextureSpecification.Height = Window::Get()->GetHeight();
        m_BgTexture = Texture2D::Create(bgTextureSpecification);

        Entity playerEntity = m_Scene->CreateEntity("Player");
        m_Player = std::make_shared<Player>(playerEntity, "Mario");
        LOG_DEBUG("Created new player: {}, uuid: {}", m_Player->GetName(), m_Player->GetUUID());

        Debug::CreateDebugSprite(*m_Scene, { 120, 180 }, { 400, 500 });

        Camera& cam = playerEntity.GetComponent<CameraComponent>();
        m_Scene->SetActiveCamera(cam);

        // Create enemies on different y positions
        float enemyPosY = 0.0f;
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
        // Get main character objects
        Entity playerEntity = m_Player->GetEntity();
        Camera& playerCam = playerEntity.GetComponent<CameraComponent>();

        m_Renderer2D->BeginScene(playerCam);
        m_Scene->BeginScene(playerCam); // if no cam is passed, m_ActiveCamera is used instead

        // First and foremost draw background
        DrawBackground();
        // Handle 2D raycast
        Input::HandleScene(*m_Scene); 

        m_Player->OnUpdate(ts);
        for (const auto& enemy : m_Enemies)
            enemy->OnUpdate(ts);

        m_Scene->EndScene();
        m_Renderer2D->EndScene();
    }

    void GameLayer::OnImGuiRender()
    {
        for (const auto& enemy : m_Enemies)
        {
            enemy->OnImGuiRender();
        }
    }

    void GameLayer::DrawBackground()
    {
        m_BgTexture->Bind();
        auto window = Window::Get();
        float bgStartX = 0.0f;
        float bgStartY = 300.0f;
        float bgEndX = (float)window->GetWidth();
        float bgEndY = (float)window->GetHeight();
        // If editor layer is enabled, use the editor viewport pos for the bottom left screen coordinates
        if (EditorLayer::IsEnabled())
        {
            auto editorLayer = EditorLayer::Get();
            //glm::vec2 viewportBounds = editorLayer->EditorViewportBounds[0];
            glm::vec2 viewportBounds = editorLayer->EditorWindowPos;
            bgStartX = viewportBounds.x;
            bgStartY = viewportBounds.y;
            //bgEndX += viewportBounds.x;
            //bgEndY += viewportBounds.y;
            Debug::PrintVec2(viewportBounds, "Viewport bounds");
        }
        Debug::PrintVec2({ bgEndX, bgEndY }, "Bg End Coords");

        //RenderCommand::DrawQuad({ bgStartX, bgStartY, 0.0f }, { bgEndX - bgStartX, bgEndY - bgStartY }, Color::RGBA::White );
        RenderCommand::DrawQuad({ bgStartX, bgStartY, 0.0f }, { bgEndX , bgEndY }, Color::RGBA::White );
    }



}