#pragma once

#include "Player.h"

namespace LkEngine {

    class Enemy : public Player
    {
    public:
        Enemy(Entity& entity, const std::string& name = "Enemy");
        ~Enemy() = default;

        void Setup() override;   // inherited from RuntimeAsset
        void Destroy() override; // inherited from RuntimeAsset

        void OnUpdate(float ts = 1.0f) override;
        void OnImGuiRender() override;

        void SetWalkDirection(WalkDirection newDir) { m_Dir = newDir; }

    private:
        WalkDirection m_Dir = WalkDirection::Left;

    };

}