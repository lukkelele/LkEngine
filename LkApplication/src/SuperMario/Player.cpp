#include "Player.h"


namespace LkEngine {

    Player::Player(const std::string& name)
        : m_Name(name)
    {
        //SpriteComponent sc;
        //sc.SetSize(20, 40);
        //m_Entity.AddComponent<SpriteComponent>(sc);
    }

    Player::~Player()
    {
    }

    void Player::SetEntity(const Entity& entity)
    {
        m_Entity = entity;
        m_ID = m_Entity.GetUUID();

        SpriteComponent sc;
        sc.SetSize(20, 60);
        m_Entity.AddComponent<SpriteComponent>(sc);
        LOG_DEBUG("Added sprite to \"{}\" with size ({}, {})", m_Name, sc.Size.x, sc.Size.y);
    }

    void Player::OnUpdate(float ts)
    {
        auto& sc = m_Entity.GetComponent<SpriteComponent>();
        RenderCommand::DrawQuad(m_Pos, m_Entity.GetComponent<SpriteComponent>().Size, sc.Color, m_Entity);
    }

    void Player::OnImGuiRender()
    {
    }

}