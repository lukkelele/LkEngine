#include "Player.h"

#include "LkEngine/Scene/Components.h"

#include "LkEngine/Renderer/TextureLibrary.h"

#include "LkEngine/Core/Window.h"

#include "LkEngine/Editor/EditorLayer.h"


namespace LkEngine {

    Player::Player(Entity entity, Ref<Scene> scene, const std::string& name)
        : m_Name(name)
    {
        if (m_Name.empty())
            m_Name = entity.Name();
        m_JumpTimer.Reset();

        auto e = scene->GetEntityWithUUID(entity.GetUUID());
        if (e)
        {
            m_Entity = e;
            LK_CORE_DEBUG("Retrieved player entity for scene");
        }
        else
        {
            scene->CreateEntity(m_Name);
            LK_CORE_DEBUG("Creating new player entity for scene");
        }
    }

    Player::~Player()
    {
        Destroy();
    }
    
    void Player::Setup()
    {
        if (m_Entity.HasComponent<SpriteComponent>())
        {
            auto& sc = m_Entity.GetComponent<SpriteComponent>();
            //sc.Removable = false;
            //sc.SetSize(60, 130);
            //sc.Color = Color::RGBA::Transparent;
            //sc.Color = Color::Generate(); // Debugging
        }
        else
        {
            auto& sc = m_Entity.AddComponent<SpriteComponent>();
            //sc.Removable = false;
            sc.SetSize(60, 130);
            sc.Color = Color::RGBA::Transparent;
            //sc.Color = Color::Generate(); // Debugging
        }

        if (m_Entity.HasComponent<TransformComponent>())
        {

        }
        else
        {
            auto& sc = m_Entity.Sprite();
            auto& tc = m_Entity.AddComponent<TransformComponent>();
            //tc.Removable = false;
            // Add half sprite size to center origin
            tc.Translation.x -= (sc.Size.x * 0.50f);
            tc.Translation.y -= (sc.Size.y * 0.50f);
            tc.Translation.z = 0.90; // Z-fighting
        }

#if 0
        if (m_Entity.HasComponent<MaterialComponent>())
        {
            m_Entity.Material().SetTexture(TextureLibrary::Get()->GetTexture2D("atte"));
        }
        else
        {
            auto& tc = m_Entity.Transform();
            auto& sc = m_Entity.Sprite();
            auto& mc = m_Entity.AddComponent<MaterialComponent>();
            mc.SetTexture(TextureLibrary::Get()->GetTexture2D("atte"));

            auto& cameraComponent = m_Entity.AddComponent<CameraComponent>();
            cameraComponent.Camera = Ref<SceneCamera>::Create();

            SceneCamera& playerCam = m_Entity.Camera();
            //playerCam.SetOrthographic(Window::Get().GetViewportWidth(), Window::Get().GetViewportHeight(), -1.0f, 1.0f);
            playerCam.SetOrthographic(Window::Get().GetViewportWidth(), Window::Get().GetViewportHeight(), -1.0f, 1.0f);
            //playerCam.SetPos(tc.Translation); // Is reset in the initializing stage of GameLayer anyways
            //playerCam.SetOffset({ Window::Get().GetViewportWidth() * 0.50f - m_Entity.Sprite().GetWidth() * 2.0f, 0});
            playerCam.SetKeyboardEnabled(false); // Player handles input itself and sets camera accordingly

            // From enemy.cpp
            RigidBody2DComponent rigidbody;
            rigidbody.BodyType = RigidBody2DComponent::Type::Static;
            rigidbody.GravityScale = 1.0f;
            m_Entity.AddComponent<RigidBody2DComponent>(rigidbody);
            //b2Body* playerRigidBody = LK_CAST_TO_b2Body(rigidbody.RuntimeBody);

            BoxCollider2DComponent boxCollider;
            boxCollider.Size = { 0.50f * (sc.Size.x * tc.Scale.x), 0.50f * (sc.Size.y * tc.Scale.y) };
            m_Entity.AddComponent<BoxCollider2DComponent>(boxCollider);

            b2Body* playerBody = LK_CAST_TO_b2Body(m_Entity.RigidBody2D().RuntimeBody);
            playerBody->SetAwake(true);

            // Create gun
            m_GunEntity = Scene::GetActiveScene()->CreateEntity("Big-Gun");
            auto& gunTC = m_GunEntity.AddComponent<TransformComponent>();
            auto& gunSC = m_GunEntity.AddComponent<SpriteComponent>();
            gunSC.SetSize(100, 90);
            //auto& gunMC = m_GunEntity.AddComponent<MaterialComponent>();
            //gunMC.SetTexture(TextureLibrary::Get()->GetTexture2D("SuperMario-gun"));
            gunTC.Translation.x = m_Pos.x + gunSC.GetSize().x;
            gunTC.Translation.y = m_Pos.y + gunSC.GetSize().y;
        }
#endif
    }

    void Player::Destroy()
    {
    }

    void Player::OnUpdate(float ts)
    {
        SceneCamera& cam = m_Entity.Camera();
        auto& tc = m_Entity.Transform();

        tc.Translation.x = m_Pos.x;
        tc.Translation.y = m_Pos.y;
        //cam.SetPos(m_Pos);
        auto& size = GetSize();

        if (m_GunEntity)
        {
            auto& gunSC = m_GunEntity.Sprite();
            auto& gunTC = m_GunEntity.Transform();
            gunTC.Translation.x = m_Pos.x + size.x * 0.50f + gunSC.GetSize().x * 0.50f;
            gunTC.Translation.y = m_Pos.y + size.y * 0.50f - gunSC.GetSize().y;
        }

        // Input 
	    // WASD
	    if (Keyboard::IsKeyPressed(Key::W))
	    {
	    	m_Pos += glm::vec3(0, 1, 0) * ts * m_TravelSpeed;
	    }
	    if (Keyboard::IsKeyPressed(Key::A))
	    {
	    	m_Pos -= glm::vec3(1, 0, 0) * ts * m_TravelSpeed;
	    }
	    if (Keyboard::IsKeyPressed(Key::S))
	    {
	    	m_Pos -= glm::vec3(0, 1, 0) * ts * m_TravelSpeed;
	    }
	    if (Keyboard::IsKeyPressed(Key::D))
	    {
	    	m_Pos += glm::vec3(1, 0, 0) * ts * m_TravelSpeed;
	    }
	    if (Keyboard::IsKeyPressed(Key::Q))
		{
			//cam.GetZoom() += 0.10f;
            //m_Camera->MouseZoom(-0.10f);
		}
		if (Keyboard::IsKeyPressed(Key::R))
		{
			//cam.GetZoom() -= 0.10f;
            //m_Camera->MouseZoom(-0.10f);
		}

		if (Keyboard::IsKeyPressed(Key::Space))
		{
            if (m_Metadata.JumpActive == false)
            {
                m_Metadata.JumpActive = true;
                m_JumpTimer.Reset();
                //LOG_TRACE("Jump started ---> {}", m_Metadata.JumpStarted);
            }
		}
		if (Keyboard::IsKeyPressed(Key::Backspace))
		{
            LK_INFO("-JUMP");
			m_Pos -= glm::vec3(0, 1, 0) * ts * m_JumpHeight;
		}

        // Jump
        if (m_Metadata.JumpActive == true)
        {
            float jumpTimeMS = m_JumpTimer.ElapsedMs();
            // If less than 0.5 seconds
            if (jumpTimeMS < 400 * 1.0f)
            {
                // First period
                if (jumpTimeMS < 200 * 1.0f)
                {
			        m_Pos += glm::vec3(0, 1, 0) * ts * 1.50f * m_JumpHeight;
                }
                else // Second period
                {
			        m_Pos += glm::vec3(0, 1, 0) * ts * m_JumpHeight;
                }
            }
            // If equal or more than 0.5 seconds
            else if (jumpTimeMS >= 400 * 1.0f)
            {
                // First period
                if (jumpTimeMS > 400 && jumpTimeMS < 600)
                {
			        m_Pos -= glm::vec3(0, 1, 0) * ts * m_JumpHeight;
                }
                else // Second period
                {
			        m_Pos -= glm::vec3(0, 1, 0) * ts * 1.50f * m_JumpHeight;
                }
            }
        }

        // Update rigidbody
        b2Body* body = static_cast<b2Body*>(m_Entity.RigidBody2D().RuntimeBody);
        body->SetTransform(b2Vec2(m_Pos.x, m_Pos.y), 0.0f);
    }

    void Player::OnImGuiRender()
    {
        UI::BeginSubwindow(UI_SELECTED_ENTITY_INFO, ImGuiWindowFlags_NoMove);

        ImGui::Text("Jump Flag: %s", m_Metadata.JumpActive ? "ACTIVE" : "NOT ACTIVE");

        //auto& rb = GetEntity().GetComponent<RigidBody2DComponent>();
        //b2Body* body = static_cast<b2Body*>(GetEntity().GetComponent<RigidBody2DComponent>().RuntimeBody);
        //ImGui::Text("Pos: (%1.f, %1.f)", m_Pos.x, m_Pos.y);
        //ImGui::Text("RigidBody Pos: (%1.f, %1.f)", body->GetPosition().x, body->GetPosition().y);

        UI::EndSubwindow();
    }

    void Player::SetPos(float x, float y)
    {
        SetPos({ x, y });
    }

    void Player::SetPos(const glm::vec2& pos)
    {
        auto& tc = m_Entity.GetComponent<TransformComponent>();
        m_Pos.x = pos.x;
        m_Pos.y = pos.y;
        tc.Translation.x = m_Pos.x;
        tc.Translation.y = m_Pos.y;

        if (m_Entity.HasComponent<CameraComponent>())
        {
            SceneCamera& cam = m_Entity.GetComponent<CameraComponent>();
            //cam.SetPos(tc.Translation);
        }
    }

    const glm::vec2 Player::GetSize() 
    {
        return m_Entity.GetComponent<SpriteComponent>().GetSize();
    }

    void Player::SetSize(float x, float y)
    {
        SetSize({ x, y });
    }

    void Player::SetSize(const glm::vec2& size)
    {
        m_Entity.GetComponent<SpriteComponent>().SetSize(size);
    }

    float Player::GetWidth()
    {
        return GetSize().x;
    }

    float Player::GetHeight()
    {
        return GetSize().y;
    }

    void Player::Respawn()
    {
        m_Pos.x = m_SpawnPoint.x;
        m_Pos.y = m_SpawnPoint.y;

        auto& tc = m_Entity.Transform();
        tc.Translation.x = m_Pos.x;
        tc.Translation.y = m_Pos.y;

        b2Body* body = static_cast<b2Body*>(m_Entity.RigidBody2D().RuntimeBody);
        body->SetTransform(b2Vec2(m_Pos.x, m_Pos.y), 0.0f);

        LK_CORE_DEBUG("Player '{}' -> Respawn!", m_Name);
    }

    void Player::SetSpawnPoint(const glm::vec2& spawnPoint)
    {
        m_SpawnPoint = spawnPoint;
    }

    void Player::SetLinearVelocity(const glm::vec2& v)
    {
        b2Body* body = static_cast<b2Body*>(m_Entity.RigidBody2D().RuntimeBody);
        body->SetLinearVelocity({ v.x, v.y });
    }

}