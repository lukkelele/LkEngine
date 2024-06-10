#pragma once

#include "Player.h"

namespace LkEngine {

    class Enemy : public Player
    {
    public:
        Enemy(Entity entity, Ref<Scene> scene, const std::string& name = "Enemy");
        ~Enemy() = default;

        void Setup() override;   // inherited from RuntimeAsset
        void Destroy() override; // inherited from RuntimeAsset
        void OnUpdate(float ts = 1.0f);
        void OnImGuiRender();

        void SetEndDistance(float endDistance);
        void SetWalkDirection(WalkDirection newDir) { m_Dir = newDir; }

    private:
        WalkDirection m_Dir = WalkDirection::Left;
        float m_EndDistance = 0.0f; // Distance in X direction, if reached then enemy respawns 
    };

}