#include "Player.h"

#include "LkEngine/Scene/Components.h"
#include "LkEngine/Scene/Entity.h"


namespace LkEngine {

    Player::Player(Entity& entity, const std::string& name)
        : m_Entity(entity)
        , m_Name(name)
    {
        m_ID = m_Entity.GetUUID();
        if (name.empty())
            m_Name = entity.GetName();
        m_JumpTimer.Reset();
    }

    Player::~Player()
    {
        Destroy();
    }
    
    void Player::Setup()
    {
		auto window = Window::Get();
		float width = window->GetWidth();
		float height = window->GetHeight();

        auto& sc = m_Entity.AddComponent<SpriteComponent>();
        sc.SetSize(60, 130);
        sc.Color = Color::Generate(); // Debugging

        // Add SpriteComponent and set its size before TransformComponent to automatically
        // re-center the origin in OnAddedComponent in Scene
        auto& tc = m_Entity.AddComponent<TransformComponent>();
        auto& mc = m_Entity.AddComponent<MaterialComponent>();
        auto& cameraComponent = m_Entity.AddComponent<CameraComponent>();

        // Add half sprite size to center origin
        tc.Translation.x -= (sc.Size.x * 0.50f);
        tc.Translation.y -= (sc.Size.y * 0.50f);

        SceneCamera* playerCam = new SceneCamera();
        //playerCam->SetOrthographic(width, height, -1.0f, 1.0f);
        playerCam->SetOrthographic(Window::Get()->GetWidth(), Window::Get()->GetHeight(), -1.0f, 1.0f);
        playerCam->SetPos(tc.Translation); // Is reset in the initializing stage of GameLayer anyways
        playerCam->SetOffset({ Window::Get()->GetWidth() * 0.90f, 0});
        playerCam->SetKeyboardEnabled(false); // Player handles input itself and sets camera accordingly

        sc.Removable = false;
        tc.Removable = false;
        cameraComponent.CameraRef = playerCam;
        cameraComponent.Removable = false;
    }

    void Player::Destroy()
    {
    }

    void Player::OnUpdate(float ts)
    {
        SceneCamera& cam = m_Entity.GetComponent<CameraComponent>();
        auto& tc = m_Entity.GetComponent<TransformComponent>();

        if (m_Entity.HasComponent<CameraComponent>())
        {
            //tc.Translation = cam.GetPos();
            tc.Translation = m_Pos;
            cam.SetPos(tc.Translation);
        }

        // Input 
	    // WASD
	    if (Keyboard::IsKeyPressed(Key::W))
	    {
	    	//m_Pos += glm::vec3(0, 1, 0) * ts * m_TravelSpeed;
	    }
	    if (Keyboard::IsKeyPressed(Key::A))
	    {
	    	m_Pos -= glm::vec3(1, 0, 0) * ts * m_TravelSpeed;
	    }
	    if (Keyboard::IsKeyPressed(Key::S))
	    {
	    	//m_Pos -= glm::vec3(0, 1, 0) * ts * m_TravelSpeed;
	    }
	    if (Keyboard::IsKeyPressed(Key::D))
	    {
	    	m_Pos += glm::vec3(1, 0, 0) * ts * m_TravelSpeed;
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
            LOG_DEBUG("-JUMP");
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
    }

    void Player::OnImGuiRender()
    {
        UI::BeginSubwindow(UI_SELECTED_ENTITY_INFO, ImGuiWindowFlags_NoMove);
        //ImGui::SliderFloat2("Camera Offset", &m_CameraOffset.x, -500.0f, 500.f, "%1.f");
        ImGui::Text("Jump Flag: %s", m_Metadata.JumpActive ? "ACTIVE" : "NOT ACTIVE");
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
            cam.SetPos(tc.Translation);
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

        auto& tc = m_Entity.GetComponent<TransformComponent>();
        tc.Translation.x = m_Pos.x;
        tc.Translation.y = m_Pos.y;

        LOG_DEBUG("Player '{}' -> Respawn!", m_Name);
    }

    void Player::SetSpawnPoint(const glm::vec2& spawnPoint)
    {
        m_SpawnPoint = spawnPoint;
    }

}