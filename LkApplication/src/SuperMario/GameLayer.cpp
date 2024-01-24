#include "GameLayer.h"
#include "Enemy.h"

#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/SceneCamera.h"

#include "LkEngine/Renderer/Renderer.h"
//#include "LkEngine/Renderer/Renderer2D.h"
#include "LkEngine/Renderer/TextureLibrary.h"

#include "LkEngine/Debug/DebugLayer.h"

//#define DONT_SAVE_SCENE
#define USE_SAVED_SCENE


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
#ifndef USE_SAVED_SCENE
        m_Scene = Ref<Scene>::Create("GameLayer", true, true);
#else
        // Use saved scene
        SceneSerializer sceneSerializer;
        sceneSerializer.Deserialize(std::filesystem::path("scene.lukkelele"));
        m_Scene = sceneSerializer.LoadScene();
#endif
        m_Renderer2D = s_ptr<Renderer2DAPI>(Renderer::GetRenderer2DAPI().Raw());
        auto& window = Window::Get();
        auto textureLibrary = TextureLibrary::Get();
        auto editorCamera = Editor::Get()->GetEditorCamera();

        //--------------------------------------------------------------------
        // USE WHEN LOADING SAVED SCENE
        //--------------------------------------------------------------------
#ifdef USE_SAVED_SCENE
        Entity playerEntity = m_Scene->FindEntity("Player");
        m_Player = std::make_shared<Player>(playerEntity, m_Scene, "Mario");
        m_Player->Setup();
        m_Player->SetSize(120, 150);
        CreateGround();
        CreateSky();
        editorCamera->SetActive(true);
        m_Scene->SetEditorCamera(editorCamera);
#endif

        uint8_t idx = 0;
        auto textures2D = TextureLibrary::Get()->GetTextures2D();
        
        CreateGround();
        auto& groundTransform = m_Ground.GetComponent<TransformComponent>();
        auto& groundSprite = m_Ground.GetComponent<SpriteComponent>();

        CreateSky();
        auto& bgTransform = m_Background.GetComponent<TransformComponent>();
        auto& bgSprite = m_Background.GetComponent<SpriteComponent>();

        editorCamera->SetActive(true);
        m_Scene->SetEditorCamera(editorCamera);

#ifndef USE_SAVED_SCENE
        Entity playerEntity = m_Scene->CreateEntity("Player");
        m_Player = std::make_shared<Player>(playerEntity, m_Scene, "Mario");
        m_Player->Setup();
        m_Player->SetSize(120, 150);
        //m_Scene->SetCamera(&m_Player->GetCamera());
        // Place player on the left side and on top of the ground entity
        float player_posY = groundTransform.Translation.y + groundSprite.Size.y * 0.50f;
        player_posY += m_Player->GetHeight() * 0.50f;
        m_Player->SetPos(groundTransform.GetTranslation().x - (groundSprite.GetWidth() * 0.50f) + 2 * m_Player->GetWidth(), player_posY);

        // TODO: Weird raycast bug whenever a certain type of rectangle dimension is used. 
        // Only the thinner ones bug out, need to figure out why
        Debug::CreateDebugSprite(*m_Scene, { 120, 180 }, { -200, -400, 0});
        Debug::CreateDebugSprite(*m_Scene, { 120, 180 }, { 100, 300 , 0 });
        Debug::CreateDebugSprite(*m_Scene, { 300, 600 }, { -200, 300 , 0 });
#endif

#if 0
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
#endif
    }

    void GameLayer::OnDetach()
    {
    }

    void GameLayer::OnUpdate(float ts)
    {
        auto& window = Window::Get();
        auto* editor = Editor::Get();

        if (m_Ground.HasComponent<MaterialComponent>())
            Renderer::SubmitQuad(m_Ground.Transform().Translation, m_Ground.Sprite().Size, m_Ground.Material().GetTexture(), 0);
        else
            Renderer::SubmitQuad(m_Ground.Transform().Translation, m_Ground.Sprite().Size, m_Ground.Sprite().Color, 0);

        m_Player->OnUpdate(ts);
#if 0

        PlayerMetadata& playerData = m_Player->GetMetadata();
        // Check player lower point y-pos versus ground upper point y-pos to determine if player has jumped or not
        float ground_posY = m_Ground.GetComponent<TransformComponent>().GetTranslation().y;
        ground_posY += m_Ground.GetComponent<SpriteComponent>().Size.y * 0.50f;
        float player_posY = m_Player->GetPos().y - m_Player->GetSize().y * 0.50f;

        // Temporary solution for 'detecting' contact with the ground beneath the player
        if (player_posY < ground_posY)
        {
            if (playerData.JumpActive == true)
                playerData.JumpActive = false;
        }
#endif

        // Render debug quads
        auto entities = m_Scene->GetEntities();
        for (auto& e : entities)
        {
            if (!e.HasComponent<MaterialComponent>() && e.HasComponent<SpriteComponent>())
            {
                //LK_CORE_DEBUG_TAG("GameLayer", "SubmitQuad: {}", e.Name());
                LK_CORE_VERIFY(e.Sprite().Size.x > 0 && e.Sprite().Size.y > 0, "Sprite size is not invalid for entity \"{}\"", e.Name());
                Renderer::SubmitQuad(e.Transform().Translation, e.Sprite().Size, e.Sprite().Color, 0);
            }
        }

#if 0
        for (auto& enemy : m_Enemies)
        {
            enemy->OnUpdate(ts);
            Renderer::SubmitQuad(enemy->GetPos(), enemy->GetSize(), enemy->GetEntity().Material().GetTexture(), 0);
        }
#endif
        if (m_Player->GetEntity().HasComponent<MaterialComponent>())
        {
            Renderer::SubmitQuad(m_Player->GetPos(), m_Player->GetSize(), m_Player->GetEntity().Material().GetTexture(), 0);
            m_Player->GetEntity().Transform().Translation;
        }

        //m_Scene->EndScene();
        m_Renderer2D->EndScene();
    }

    void GameLayer::OnImGuiRender()
    {
        m_Player->OnImGuiRender();

        for (auto& enemy : m_Enemies)
            enemy->OnImGuiRender();
    }

    void GameLayer::CreateGround()
    {
        auto& window = Window::Get();
        auto groundTexture = TextureLibrary::Get()->GetTexture2D("SuperMario-ground_block");

        TransformComponent groundTransform;
        SpriteComponent groundSprite;
        MaterialComponent groundMaterial;

        //Entity groundEntity = m_Scene->FindEntity("Ground");
        Entity groundEntity = m_Scene->FindEntity("Ground");
        //if (groundEntity == NULL)
        if (m_Scene->HasEntity(groundEntity) == false)
        {
            groundEntity = m_Scene->CreateEntity("Ground");
            //m_Ground = std::make_shared<Entity>(groundEntity);
            m_Ground = Entity(groundEntity);
            groundSprite = m_Ground.AddComponent<SpriteComponent>();
            groundMaterial = m_Ground.AddComponent<MaterialComponent>(groundTexture);
            groundTransform = m_Ground.AddComponent<TransformComponent>();

            groundTransform.Translation.x = -(window.GetViewportWidth() * 0.50f);  // Lower left
            groundTransform.Translation.y = -(window.GetViewportHeight() * 0.50f); // Bottom half
            groundTransform.Translation.z = -0.50; 
            //groundSprite.Size = { 3.0f * (float)window.GetViewportWidth(), 400.0f };
            groundSprite.Size = { 200, 200 };
            groundSprite.SetPassthrough(false);

            RigidBody2DComponent rigidbody(RigidBody2DComponent::Type::Static);
            rigidbody.Mass = 100;
            rigidbody.GravityScale = 0.0f;
            m_Ground.AddComponent<RigidBody2DComponent>(rigidbody);

            BoxCollider2DComponent boxCollider;
            boxCollider.Offset = { 0.0f, -50.0f };
            //boxCollider.Size = { 2000, 20 };
            boxCollider.Size = 0.50f * groundSprite.Size;
            m_Ground.AddComponent<BoxCollider2DComponent>(boxCollider);
        }
        else
        {
            m_Ground = Entity(groundEntity);
            //m_Ground = std::make_shared<Entity>(groundEntity);
            //groundMaterial = m_Ground.GetComponent<MaterialComponent>();
            //groundSprite = m_Ground.GetComponent<SpriteComponent>();
            //groundTransform = m_Ground.GetComponent<TransformComponent>();
        }
    }

    void GameLayer::CreateSky()
    {
        auto& window = Window::Get();
        auto skyTexture = TextureLibrary::Get()->GetTexture2D("sky-background-2d");

        TransformComponent transform;
        SpriteComponent sprite;
        MaterialComponent material;

        Entity bgEntity = m_Scene->FindEntity("Background");
        if (m_Scene->HasEntity(bgEntity) == false)
        {
            bgEntity = m_Scene->CreateEntity("Background");

            transform = bgEntity.AddComponent<TransformComponent>();
            sprite = bgEntity.AddComponent<SpriteComponent>();
            material = bgEntity.AddComponent<MaterialComponent>(skyTexture);

            transform.Translation.y = 450.0f;
            transform.Translation.z = -20.0f;

            sprite.SetPassthrough(true);

            float bgStartX = transform.GetTranslation().x;
            float bgStartY = transform.GetTranslation().y;
            float bgEndX = (float)window.GetViewportWidth();
            float bgEndY = (float)window.GetViewportHeight();

            //m_Background = std::make_shared<Entity>(bgEntity);
            m_Background = Entity(bgEntity);

            // Draw calls use origin in the middle, so offset this to the middle of the render window
            bgStartX += bgEndX * 0.50f;
            bgStartY += bgEndY * 0.50f;

            // If editor layer is enabled, use the editor viewport pos for the bottom left screen coordinates
            auto editor = Editor::Get();
            if (editor->IsEnabled())
            {
                glm::vec2 viewportBounds = editor->EditorViewportBounds[0];
                bgStartX += viewportBounds.x * 0.50f;
                bgStartY += viewportBounds.y * 0.50f;
            }

            glm::vec3 startPos = { bgStartX, bgStartY, 0.0f };
            glm::vec2 size = { bgEndX * 3.0f, bgEndY * 1.0f };
            transform.Translation = startPos;
            sprite.SetSize(size);
        }
        else
        {
            m_Background = Entity(bgEntity);
            //m_Background = std::make_shared<Entity>(bgEntity);
        }

    }


    float GameLayer::Ground_GetLeftBound()
    {
        return (m_Ground.GetComponent<TransformComponent>().GetTranslation().x - m_Ground.GetComponent<SpriteComponent>().Size.x * 0.50f);
    }

}