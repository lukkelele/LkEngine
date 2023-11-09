#include "Enemy.h"



namespace LkEngine {

    Enemy::Enemy(const std::string& name)
        : Player(name)
    {
    }

    void Enemy::OnUpdate(float ts)
    {
        float distance = m_Speed * ts;
        if (m_Dir == WalkDirection::Left)
            distance *= -1;
        auto& tc = m_Entity.GetComponent<TransformComponent>();
        tc.Translation.x += distance;

        if (tc.Translation.x < 0)
        {
            tc.Translation.x = Window::Get()->GetWidth();
        }
    }

    void Enemy::OnImGuiRender()
    {
    }

}