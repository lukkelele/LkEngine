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
        LOG_DEBUG("Created new player: {}, uuid: {}", m_Player->GetName(), m_Player->GetUUID());

        Entity bgEntity = m_Scene->CreateEntity("Background");
        auto& bgSprite = bgEntity.AddComponent<SpriteComponent>();
        auto& bgTransform = bgEntity.AddComponent<TransformComponent>();
        //auto& bgMC = bgEntity.AddComponent<MaterialComponent>(m_BgTexture);
        bgEntity.AddComponent<MaterialComponent>(skyTexture);
        bgTransform.Translation.y = 450.0f;
        m_Background = std::make_shared<Entity>(bgEntity);

        Entity groundEntity = m_Scene->CreateEntity("Ground");
        auto& groundSprite = groundEntity.AddComponent<SpriteComponent>();
        auto& groundTransform = groundEntity.AddComponent<TransformComponent>();
        groundEntity.AddComponent<MaterialComponent>(groundTexture);
        groundTransform.Translation.x = 50.0f;
        groundTransform.Translation.y = 50.0f;
        groundSprite.Size = { 200.0f, 200.0f };
        m_Ground = std::make_shared<Entity>(groundEntity);

        float bgStartX = bgTransform.GetTranslation().x;
        float bgStartY = bgTransform.GetTranslation().y;
        float bgEndX = (float)window->GetWidth();
        float bgEndY = (float)window->GetHeight();
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
        //Debug::CreateDebugSprite(*m_Scene, { 120, 180 }, { -100, 300 });
        //Debug::CreateDebugSprite(*m_Scene, { 80, 110 }, { -240, 0 });
        //Debug::CreateDebugSprite(*m_Scene, { 80, 320 }, { 140, 300 }); // Bugged out when raycasting

        SceneCamera& cam = playerEntity.GetComponent<CameraComponent>();
        cam.SetOrthographic(Window::Get()->GetWidth(), Window::Get()->GetHeight(), -1.0f, 1.0f);
        m_Scene->SetActiveCamera(cam);

        // Create enemies on different y positions
        float enemyPosY = 0.0f;
        for (int i = 0; i < 1; i++)
        {
            // Create enemy entity and enemy object
            Entity enemyEntity = m_Scene->CreateEntity("Enemy-" + std::to_string(i + 1));
            s_ptr<Enemy> enemy = std::make_shared<Enemy>(enemyEntity);

            auto& mc = enemyEntity.GetComponent<MaterialComponent>();
            mc.SetTexture(textureLibrary->GetTexture2D("atte_square"));

            // Set the enemy position
            enemyPosY += (-window->GetHeight() * 0.50f) + enemy->GetHeight() * (DebugLayer::s_DebugEntities * 1.50f);
            enemy->SetPos(0, enemyPosY);

            // Set texture instead of color
            //auto& sc = enemyEntity.GetComponent<SpriteComponent>().TextureName = "atte_square";
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

        RenderCommand::DrawLine({ -200, 20 }, { 200, 40 }, Color::RGBA::Blue);

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



}