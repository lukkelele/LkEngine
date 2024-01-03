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
        auto skyTexture = textureLibrary->GetTexture2D("sky-background-2d");
        auto groundTexture = textureLibrary->GetTexture2D("SuperMario-ground_block");

        Entity playerEntity = m_Scene->CreateEntity("Player");
        m_Player = std::make_shared<Player>(playerEntity, "Mario");
        m_Player->Setup();
        LOG_DEBUG("Created new player: {}, uuid: {}", m_Player->GetName(), m_Player->GetUUID());

        Entity bgEntity = m_Scene->CreateEntity("Background");
        auto& bgSprite = bgEntity.AddComponent<SpriteComponent>();
        auto& bgTransform = bgEntity.AddComponent<TransformComponent>();
        bgSprite.SetPassthrough(true);
        bgEntity.AddComponent<MaterialComponent>(skyTexture);
        bgTransform.Translation.y = 450.0f;
        m_Background = std::make_shared<Entity>(bgEntity);

        Entity groundEntity = m_Scene->CreateEntity("Ground");
        auto& groundMaterial = groundEntity.AddComponent<MaterialComponent>(groundTexture);
        auto& groundSprite = groundEntity.AddComponent<SpriteComponent>();
        auto& groundTransform = groundEntity.AddComponent<TransformComponent>();
        groundSprite.SetPassthrough(false);
        //groundTransform.Translation.x = 0.0f;
        groundTransform.Translation.x = -(window->GetViewportWidth() * 0.50f);
        groundTransform.Translation.y = -(window->GetViewportHeight() * 0.50f);
        groundSprite.Size = { ((float)window->GetViewportWidth() + 100.0f), 400.0f };
        m_Ground = std::make_shared<Entity>(groundEntity);

        // Set player on top of ground and to the left of the screen
        SceneCamera& cam = playerEntity.GetComponent<CameraComponent>();
        cam.SetOrthographic(Window::Get()->GetWidth(), Window::Get()->GetHeight(), -1.0f, 1.0f);
        // Place the camera such that the player is in the lower left corner
        cam.SetOffset({ Window::Get()->GetWidth() * 0.90f, 0});
        float player_posY = groundTransform.Translation.y + groundSprite.Size.y * 0.50f;
        player_posY += m_Player->GetHeight() * 0.50f;
        m_Player->SetPos(-(groundSprite.GetWidth()) + 1.50f * m_Player->GetWidth(), player_posY);

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
        
        //RenderCommand::DrawQuad(startPos, size, m_BgTexture);

        // TODO: Weird raycast bug whenever a certain type of rectangle dimension is used. 
        // Only the thinner ones bug out, need to figure out why
        Debug::CreateDebugSprite(*m_Scene, { 120, 180 }, { -200, -400, 0});
        Debug::CreateDebugSprite(*m_Scene, { 120, 180 }, { 100, 300 , 0 });

        m_Scene->SetActiveCamera(&cam);

        // Create enemies on different y positions
        float enemyPosY = 0.0f;
        for (int i = 0; i < 1; i++)
        {
            // Create enemy entity and enemy object
            Entity enemyEntity = m_Scene->CreateEntity("Enemy-" + std::to_string(i + 1));
            s_ptr<Enemy> enemy = std::make_shared<Enemy>(enemyEntity);
            enemy->Setup();
            //auto& sc = enemyEntity.GetComponent<SpriteComponent>();

            // Set the enemy position
            enemyPosY += (-window->GetHeight() * 0.50f) + enemy->GetHeight() * (DebugLayer::s_DebugEntities * 1.50f);
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
        Entity& playerEntity = m_Player->GetEntity();
        Camera& playerCam = playerEntity.GetComponent<CameraComponent>();

        m_Renderer2D->BeginScene(playerCam);
        m_Scene->BeginScene();

        //RenderCommand::DrawLine({ -200, 20 }, { 200, 40 }, Color::RGBA::Blue);

        auto* window = Window::Get();
        auto* editor = EditorLayer::Get();

        // Scale mouse pos with window resolution
        //auto mousePos = Mouse::GetPos();
        auto mousePos = Mouse::GetPos();
        RenderCommand::DrawLine({ mousePos.x - 40, (mousePos.y - 40) }, { mousePos.x + 40, (mousePos.y + 40) }, Color::RGBA::Blue);
        //RenderCommand::DrawLine({ mousePos.x + 40, (mousePos.y + 40) }, { mousePos.x - 40, (mousePos.y - 40) }, Color::RGBA::Red);

        m_Player->OnUpdate(ts);

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



}