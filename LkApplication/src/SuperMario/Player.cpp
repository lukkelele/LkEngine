#include "Player.h"


namespace LkEngine {

    Player::Player(Entity& entity)
        : m_Name(entity.GetName())
    {
        SetEntity(entity);
    }

    Player::Player(const std::string& name)
        : m_Name(name)
    {
    }

    Player::~Player()
    {
    }

    void Player::SetEntity(const Entity& entity)
    {
        m_Entity = entity;
        m_ID = m_Entity.GetUUID();

        //SpriteComponent sc;
        SpriteComponent& sc = m_Entity.AddComponent<SpriteComponent>();
        sc.SetSize(60, 130);
        TransformComponent& tc = m_Entity.AddComponent<TransformComponent>();
        tc.Translation.x += (sc.Size.x * 0.50f);
        tc.Translation.y += (sc.Size.y * 0.50f);
        CameraComponent& cameraComponent = m_Entity.AddComponent<CameraComponent>();

		auto window = Window::Get();
		float width = window->GetWidth();
		float height = window->GetHeight();
        OrthographicCamera* playerCam = new OrthographicCamera(0, width, 0, height);
        playerCam->SetPos(tc.Translation);
        cameraComponent.CameraRef = playerCam;

        LOG_DEBUG("Added sprite to \"{}\" with size ({}, {})", m_Name, sc.Size.x, sc.Size.y);
    }

    void Player::OnUpdate(float ts)
    {
        Camera& cam = m_Entity.GetComponent<CameraComponent>();

        auto& tc = m_Entity.GetComponent<TransformComponent>();
        tc.Translation = cam.GetPos();
        tc.Translation.x += m_CameraOffset.x;
        tc.Translation.y += m_CameraOffset.y;
    }

    void Player::OnImGuiRender()
    {
    }

}