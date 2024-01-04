#include "Enemy.h"



namespace LkEngine {

    Enemy::Enemy(Entity& entity, const std::string& name)
        : Player(entity, name)
    {
        m_Entity = entity;
    }

    void Enemy::Setup()
    {
        auto& sc = m_Entity.AddComponent<SpriteComponent>();
        sc.SetSize(80, 130);

        // Add SpriteComponent and set its size before TransformComponent to automatically
        // re-center the origin in OnAddedComponent in Scene
        auto& tc = m_Entity.AddComponent<TransformComponent>();
        auto& mc = m_Entity.AddComponent<MaterialComponent>();
        mc.SetTexture(TextureLibrary::Get()->GetTexture2D("atte_square"));

        RigidBody2DComponent rigidbody;
        rigidbody.BodyType = RigidBody2DComponent::Type::Dynamic;
        rigidbody.GravityScale = 0.0f;
        m_Entity.AddComponent<RigidBody2DComponent>(rigidbody);

        BoxCollider2DComponent boxCollider;
        //boxCollider.Size = { 100, 100 };
        boxCollider.Size = { sc.Size.x * tc.Scale.x, sc.Size.y * tc.Scale.y };
        m_Entity.AddComponent<BoxCollider2DComponent>(boxCollider);

        sc.Color = Color::RGBA::Red;
        sc.Color.a = 0.10f;
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
        tc.Translation.x += distance;

        b2Body* body = static_cast<b2Body*>(GetEntity().GetComponent<RigidBody2DComponent>().RuntimeBody);
        body->SetTransform(b2Vec2(tc.Translation.x, tc.Translation.y), 0.0f);

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