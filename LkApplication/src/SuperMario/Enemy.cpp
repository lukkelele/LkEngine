#include "Enemy.h"



namespace LkEngine {

    Enemy::Enemy(Entity& entity, const std::string& name)
        : Player(entity, name)
    {
        m_Entity = entity;
    }

    void Enemy::Setup()
    {
        m_ID = m_Entity.GetUUID();

		auto window = Window::Get();
		float width = window->GetWidth();
		float height = window->GetHeight();

        auto& sc = m_Entity.AddComponent<SpriteComponent>();
        sc.SetSize(60, 130);
        sc.Color = Color::Generate(); // Debugging

        // Add SpriteComponent and set its size before TransformComponent to automatically
        // re-center the origin in OnAddedComponent in Scene
        auto& tc = m_Entity.AddComponent<TransformComponent>();
        auto& mc = m_Entity.AddComponent<MaterialComponent>();
        mc.SetTexture(TextureLibrary::Get()->GetTexture2D("atte_square"));
    }

    void Enemy::Destroy()
    {
    }

    void Enemy::OnUpdate(float ts)
    {
        float distance = m_TravelSpeed * ts;
        if (m_Dir == WalkDirection::Left)
            distance *= -1;

        auto& tc = m_Entity.GetComponent<TransformComponent>();
        auto& sc = m_Entity.GetComponent<SpriteComponent>();
        m_Pos.x += distance;

        //tc.Translation.x += distance;
        //tc.Translation.x = m_Pos.x;
        tc.Translation.x += distance;

        // Whenever the enemy sprite is 'outside' of the screen to the left, reset the position to the right
        //if (tc.Translation.x < -(Window::Get()->GetViewportWidth() * 0.50f + sc.GetWidth()))
        if (tc.Translation.x < m_EndDistance)
        {
            //tc.Translation.x = Window::Get()->GetViewportWidth() * 0.50f;
            //tc.Translation.x = m_SpawnPoint.x;
            Respawn();
        }
    }

    void Enemy::OnImGuiRender()
    {
        auto* editor = EditorLayer::Get();
        if (editor && editor->GetSelectedEntity() != m_Entity)
            return;

        UI::BeginSubwindow(UI_SELECTED_ENTITY_INFO, ImGuiWindowFlags_NoMove);
        ImGui::SliderFloat("Speed", &m_TravelSpeed, 0.0f, 5.0f, "%.1f");
        UI::EndSubwindow();
    }

    void Enemy::SetEndDistance(float endDistance)
    {
        m_EndDistance = endDistance;
    }

}