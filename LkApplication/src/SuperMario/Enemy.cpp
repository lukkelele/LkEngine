#include "Enemy.h"



namespace LkEngine {

    Enemy::Enemy(Entity& entity, const std::string& name)
        : Player(entity, name)
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
        auto* editor = EditorLayer::Get();
        if (editor && editor->GetSelectedEntity() != m_Entity)
            return;

        UI::BeginSubwindow(UI_SELECTED_ENTITY_INFO);
        ImGui::SliderFloat("Speed", &m_Speed, 0.0f, 5.0f, "%.1f");
        UI::EndSubwindow();
    }

}