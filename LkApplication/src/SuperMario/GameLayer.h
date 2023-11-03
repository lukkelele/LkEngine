#pragma once

#include "LkEngine/LkEngine.h"

#include "Player.h"


namespace LkEngine {

    /**
     * Super Mario wannabee
     */
    class GameLayer : public Layer
    {
    public:
        GameLayer();
        ~GameLayer();

        void OnAttach() override;
        void OnDetach() override;


    private:
        s_ptr<Player> m_Player = nullptr;
        s_ptr<Scene> m_Scene = nullptr;

    };

}