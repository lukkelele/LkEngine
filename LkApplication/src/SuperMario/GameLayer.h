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
        void OnImGuiRender() override;
        void OnUpdate(float ts) override;

        void DrawBackground();

    private:
        s_ptr<Scene> m_Scene = nullptr;
        s_ptr<Player> m_Player = nullptr;
        s_ptr<Texture2D> m_BgTexture = nullptr;
        s_ptr<Renderer2D> m_Renderer2D = nullptr;
        s_ptr<Entity> m_Background = nullptr;

        glm::vec2 m_BackgroundPos = { 0, 300.0f };

        std::vector<s_ptr<Enemy>> m_Enemies = {};
    };

}