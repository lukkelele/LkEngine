#pragma once

#include "LkEngine/LkEngine.h"

#include "Player.h"

#include <assimp/Importer.hpp>


namespace LkEngine {

    class Enemy;

    // Super Mario wannabe
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
        Ref<Scene> m_Scene = nullptr;

        s_ptr<Renderer2DAPI> m_Renderer2D = nullptr;
        s_ptr<Player> m_Player = nullptr;

        Entity m_Ground{};
        Entity m_Background{};

        glm::vec2 m_BackgroundPos = { 0, 300.0f };

        float m_GravityScalar = 3.0f;

        std::vector<s_ptr<Enemy>> m_Enemies = {};

        friend class EditorLayer;
        friend class SceneManagerPanel;
    };

}