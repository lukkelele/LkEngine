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
        m_Renderer2D = Renderer::GetRendererAPI()->GetRenderer2D();
        m_Scene = Scene::Create("GameLayer");

        auto* window = Window::Get();
        auto* textureLibrary = TextureLibrary::Get();

        CreateGround();
        auto& groundTransform = m_Ground->GetComponent<TransformComponent>();
        auto& groundSprite = m_Ground->GetComponent<SpriteComponent>();

        CreateSky();
        auto& bgTransform = m_Background->GetComponent<TransformComponent>();
        auto& bgSprite = m_Background->GetComponent<SpriteComponent>();

        Entity playerEntity = m_Scene->CreateEntity("Player");
        m_Player = std::make_shared<Player>(playerEntity, "Mario");
        m_Player->Setup();
        m_Player->SetSize(120, 150);
        m_Scene->SetActiveCamera(&m_Player->GetCamera());
        // Place player on the left side and on top of the ground entity
        float player_posY = groundTransform.Translation.y + groundSprite.Size.y * 0.50f;
        player_posY += m_Player->GetHeight() * 0.50f;
        m_Player->SetPos(groundTransform.GetTranslation().x - (groundSprite.GetWidth() * 0.50f) + 2 * m_Player->GetWidth(), player_posY);

        // TODO: Weird raycast bug whenever a certain type of rectangle dimension is used. 
        // Only the thinner ones bug out, need to figure out why
        Debug::CreateDebugSprite(*m_Scene, { 120, 180 }, { -200, -400, 0});
        Debug::CreateDebugSprite(*m_Scene, { 120, 180 }, { 100, 300 , 0 });

        // Create enemies on different y positions
        float enemyPosY = 0.0f;
        for (int i = 0; i < 1; i++)
        {
            // Create enemy entity and enemy object
            Entity enemyEntity = m_Scene->CreateEntity("Enemy-" + std::to_string(i + 1));
            s_ptr<Enemy> enemy = std::make_shared<Enemy>(enemyEntity);
            enemy->Setup();

            enemy->SetSpawnPoint({
                groundTransform.GetTranslation().x + groundSprite.GetWidth() * 0.50f,
                player_posY + enemyPosY
            });
            // Set end distance to be the X of the left placed points of the ground entity
            enemy->SetEndDistance(groundTransform.GetTranslation().x - (groundSprite.GetWidth() * 0.50f) + 2);

            // Set the enemy position
            enemyPosY += enemy->GetHeight() * (DebugLayer::s_DebugEntities * 1.50f);
            enemy->SetPos(enemy->GetSpawnPoint());

            m_Enemies.push_back(enemy);
        }
    }

    void GameLayer::OnDetach()
    {
    }

    void GameLayer::OnUpdate(float ts)
    {
        auto* window = Window::Get();
        auto* editor = EditorLayer::Get();

        // Get main objects
        Entity& playerEntity = m_Player->GetEntity();
        Camera& playerCam = playerEntity.GetComponent<CameraComponent>();

        m_Renderer2D->BeginScene(playerCam);
        m_Scene->BeginScene();

        // Scale mouse pos with window resolution
        auto mousePos = Mouse::GetPos();
        RenderCommand::DrawLine({ mousePos.x - 40, (mousePos.y - 40) }, { mousePos.x + 40, (mousePos.y + 40) }, Color::RGBA::Blue);
        //RenderCommand::DrawLine({ mousePos.x + 40, (mousePos.y + 40) }, { mousePos.x - 40, (mousePos.y - 40) }, Color::RGBA::Red);

        m_Player->OnUpdate(ts);
        PlayerMetadata& playerData = m_Player->GetMetadata();

        // Check player lower point y-pos versus ground upper point y-pos to determine if player has jumped or not
        float ground_posY = m_Ground->GetComponent<TransformComponent>().GetTranslation().y;
        ground_posY += m_Ground->GetComponent<SpriteComponent>().Size.y * 0.50f;
        float player_posY = m_Player->GetPos().y - m_Player->GetSize().y * 0.50f;

#if 0
        // Player has jumped - IN AIR
        if (player_posY > ground_posY)
        {
            // Initial jump 
            if (playerData.JumpActive == false)
            {
                playerData.JumpActive = true;
                playerData.LastJumpPos = m_Player->GetPos();
            }
            // Apply simple physics
            else
            {
                auto& playerPos = m_Player->GetPos();
                playerPos.y -= m_GravityScalar;
            }
        }
        // Player has not jumped - ON GROUND
        else if (player_posY <= ground_posY)
        {
            // Player landing
            if (playerData.JumpActive == true)
            {
                playerData.JumpActive = false;
            }
        }
#endif
        // Temporary solution for 'detecting' contact with the ground beneath the player
        if (player_posY < ground_posY)
        {
            if (playerData.JumpActive == true)
                playerData.JumpActive = false;
        }

        for (const auto& enemy : m_Enemies)
            enemy->OnUpdate(ts);

        m_Scene->EndScene();
        m_Renderer2D->EndScene();
    }

    void GameLayer::OnImGuiRender()
    {
        m_Player->OnImGuiRender();

        for (const auto& enemy : m_Enemies)
            enemy->OnImGuiRender();
    }

    void GameLayer::CreateGround()
    {
        auto* window = Window::Get();
        auto groundTexture = TextureLibrary::Get()->GetTexture2D("SuperMario-ground_block");

        Entity groundEntity = m_Scene->CreateEntity("Ground");
        auto& groundMaterial = groundEntity.AddComponent<MaterialComponent>(groundTexture);
        auto& groundSprite = groundEntity.AddComponent<SpriteComponent>();
        auto& groundTransform = groundEntity.AddComponent<TransformComponent>();

        groundTransform.Translation.x = -(window->GetViewportWidth() * 0.50f);  // Lower left
        groundTransform.Translation.y = -(window->GetViewportHeight() * 0.50f); // Bottom half

        groundSprite.Size = { 3.0f * (float)window->GetViewportWidth(), 400.0f };
        groundSprite.SetPassthrough(false);

        //groundTransform.Translation.x -= groundSprite.Size.x * 0.50f;

        m_Ground = std::make_shared<Entity>(groundEntity);
    }

    void GameLayer::CreateSky()
    {
        auto* window = Window::Get();
        auto skyTexture = TextureLibrary::Get()->GetTexture2D("sky-background-2d");
        Entity bgEntity = m_Scene->CreateEntity("Background");

        auto& bgMc = bgEntity.AddComponent<MaterialComponent>(skyTexture);
        auto& bgSprite = bgEntity.AddComponent<SpriteComponent>();
        auto& bgTransform = bgEntity.AddComponent<TransformComponent>();

        bgTransform.Translation.y = 450.0f;
        bgSprite.SetPassthrough(true);

        m_Background = std::make_shared<Entity>(bgEntity);

        float bgStartX = bgTransform.GetTranslation().x;
        float bgStartY = bgTransform.GetTranslation().y;
        float bgEndX = (float)window->GetViewportWidth();
        float bgEndY = (float)window->GetViewportHeight();

        // Draw calls use origin in the middle, so offset this to the middle of the render window
        bgStartX += bgEndX * 0.50f;
        bgStartY += bgEndY * 0.50f;

        // If editor layer is enabled, use the editor viewport pos for the bottom left screen coordinates
        auto editorLayer = EditorLayer::Get();
        if (editorLayer->IsEnabled())
        {
            glm::vec2 viewportBounds = editorLayer->EditorViewportBounds[0];
            bgStartX += viewportBounds.x * 0.50f;
            bgStartY += viewportBounds.y * 0.50f;
        }

        glm::vec3 startPos = { bgStartX, bgStartY, 0.0f };
        glm::vec2 size = { bgEndX * 3.0f, bgEndY * 1.0f };
        bgTransform.Translation = startPos;
        bgSprite.SetSize(size);
    }


    float GameLayer::Ground_GetLeftBound()
    {
        return (m_Ground->GetComponent<TransformComponent>().GetTranslation().x
                - m_Ground->GetComponent<SpriteComponent>().Size.x * 0.50f);
    }

}