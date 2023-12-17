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
        //m_Scene = std::shared_ptr<Scene>(Scene::GetActiveScene());
        m_Scene = Scene::Create("GameLayer");

        auto imagesInAssetsDir = File::GetFilesInDirectory("assets/img");
        auto textureLibrary = TextureLibrary::Get();
        std::string bgFilename = "sky-background-2d";
        m_BgTexture = textureLibrary->GetTexture2D(bgFilename);
        LK_ASSERT(m_BgTexture);

        Entity playerEntity = m_Scene->CreateEntity("Player");
        m_Player = std::make_shared<Player>(playerEntity, "Mario");
        LOG_DEBUG("Created new player: {}, uuid: {}", m_Player->GetName(), m_Player->GetUUID());

        Entity bgEntity = m_Scene->CreateEntity("Background");
        auto& bgSprite = bgEntity.AddComponent<SpriteComponent>();
        auto& bgTransform = bgEntity.AddComponent<TransformComponent>();
        bgTransform.Translation.y = 450.0f;
        m_Background = std::make_shared<Entity>(bgEntity);

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
            // Set texture instead of color
            auto& sc = enemyEntity.GetComponent<SpriteComponent>().TextureName = "atte_square";
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

        DrawBackground();
        // Handle 2D raycast
        Input::HandleScene(*m_Scene); 

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
        for (const auto& enemy : m_Enemies)
        {
            enemy->OnImGuiRender();
        }
    }

    void GameLayer::DrawBackground()
    {
        auto window = Window::Get();
        auto& tc = m_Background->GetComponent<TransformComponent>();
        float bgStartX = tc.GetTranslation().x;
        float bgStartY = tc.GetTranslation().y;
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

        glm::vec3 startPos = { bgStartX, bgStartY, 0.0f};
        glm::vec2 size = { bgEndX * 3.0f, bgEndY * 1.0f };

        RenderCommand::DrawQuad(startPos, size, m_BgTexture);
    }



}