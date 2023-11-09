#pragma once

#include "LkEngine/LkEngine.h"

#include "Player.h"


namespace LkEngine {

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

    private:
        s_ptr<Scene> m_Scene = nullptr;
        s_ptr<Player> m_Player = nullptr;
        s_ptr<Renderer2D> m_Renderer2D = nullptr;
    };

}