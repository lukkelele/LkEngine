#include "GameLayer.h"


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
        m_Scene = std::make_shared<Scene>();
        m_Player = std::make_shared<Player>("Mario");
        LOG_DEBUG("Created new player: {}", m_Player->GetName());

        //m_Scene->
        Entity playerEntity = m_Scene->CreateEntity("Player");
        m_Player->SetEntity(playerEntity);
        LOG_DEBUG("Player UUID set to: {}", m_Player->GetUUID());
    }

    void GameLayer::OnDetach()
    {
    }

}