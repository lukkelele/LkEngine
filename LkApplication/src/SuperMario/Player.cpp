#include "Player.h"

#include "LkEngine/Scene/Components.h"
#include "LkEngine/Scene/Entity.h"


namespace LkEngine {

    Player::Player(Entity& entity, const std::string& name)
        : m_Entity(entity)
        , m_Name(name)
    {
        if (name.empty())
            m_Name = entity.GetName();
    }

    Player::~Player()
    {
    }
    
    void Player::Setup()
    {
        m_ID = m_Entity.GetUUID();

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
        playerCam->SetOrthographic(width, height, -1.0f, 1.0f);
        playerCam->SetPos(tc.Translation);
        cameraComponent.CameraRef = playerCam;

        sc.Removable = false;
        tc.Removable = false;
        cameraComponent.Removable = false;

        m_CameraOffset = { -window->GetViewportWidth() * 0.50f + 100.0f, 0.0f };
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
            tc.Translation = cam.GetPos();
            //tc.Translation.x += m_CameraOffset.x;
            //tc.Translation.y += m_CameraOffset.y;
        }
    }

    void Player::OnImGuiRender()
    {
        UI::BeginSubwindow(UI_SELECTED_ENTITY_INFO, ImGuiWindowFlags_NoMove);
        ImGui::SliderFloat2("Camera Offset", &m_CameraOffset.x, -500.0f, 500.f, "%1.f");
        UI::EndSubwindow();
    }

    void Player::SetEntity(const Entity& entity)
    {
        m_Entity = entity;
        m_ID = m_Entity.GetUUID();

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
        playerCam->SetOrthographic(width, height, -1.0f, 1.0f);
        playerCam->SetPos(tc.Translation);
        cameraComponent.CameraRef = playerCam;

        sc.Removable = false;
        tc.Removable = false;
        cameraComponent.Removable = false;
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
        SpriteComponent& sc = m_Entity.GetComponent<SpriteComponent>();
        return sc.GetSize();
    }

    float Player::GetWidth()
    {
        return GetSize().x;
    }

    float Player::GetHeight()
    {
        return GetSize().y;
    }


}