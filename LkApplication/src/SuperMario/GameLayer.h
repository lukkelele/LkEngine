#pragma once

#include "LkEngine/LkEngine.h"

#include "Player.h"


namespace LkEngine {

    // Forward declaration
    class Enemy;

    /**
     * Super Mario wannabe
     */
    class GameLayer : public Layer
    {
    public:
        GameLayer();
        ~GameLayer();

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(float ts) override;
        void OnImGuiRender() override;

    private:
        void CreateGround();
        void CreateSky();

    private:
        s_ptr<Scene> m_Scene = nullptr;
        s_ptr<Player> m_Player = nullptr;
        s_ptr<Renderer2D> m_Renderer2D = nullptr;
        s_ptr<Entity> m_Ground = nullptr;
        s_ptr<Entity> m_Background = nullptr;
        glm::vec2 m_BackgroundPos = { 0, 300.0f };

        float m_GravityScalar = 3.0f;

        std::vector<s_ptr<Enemy>> m_Enemies = {};
    };

}