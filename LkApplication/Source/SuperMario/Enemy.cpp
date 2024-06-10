#include "Enemy.h"

#include "LkEngine/Editor/EditorLayer.h"



namespace LkEngine {

    Enemy::Enemy(Entity entity, Ref<Scene> scene, const std::string& name)
        : Player(entity, scene, name)
    {
        //m_Entity = entity;
        Entity e = scene->GetEntityWithUUID(entity.GetUUID());
        if (e) 
            m_Entity = e;
        else
            m_Entity = entity;
    }

    void Enemy::Setup()
    {
        auto& sc = m_Entity.AddComponent<SpriteComponent>();
        sc.SetSize(80, 130);

        // Add SpriteComponent and set its size before TransformComponent to automatically
        // re-center the origin in OnAddedComponent in Scene
        auto& tc = m_Entity.AddComponent<TransformComponent>();
        tc.Translation.z = 0.30f;

        //auto& mc = m_Entity.AddComponent<MaterialComponent>();

        RigidBody2DComponent rigidbody;
        rigidbody.BodyType = RigidBody2DComponent::Type::Dynamic;
        rigidbody.GravityScale = 1.0f;
        m_Entity.AddComponent<RigidBody2DComponent>(rigidbody);

        BoxCollider2DComponent boxCollider;
        boxCollider.Size = { sc.Size.x * tc.Scale.x, sc.Size.y * tc.Scale.y };
        m_Entity.AddComponent<BoxCollider2DComponent>(boxCollider);

        b2Body* body = LK_CAST_TO_b2Body(m_Entity.RigidBody2D().RuntimeBody);
        body->SetLinearVelocity(b2Vec2(-500.0f, 0.0f));

        sc.Color = Color::RGBA::Red;
        sc.Color.a = 0.10f;
    }

    void Enemy::Destroy()
    {
    }

    void Enemy::OnUpdate(float ts)
    {
        //float distance = m_TravelSpeed * ts;
        //if (m_Dir == WalkDirection::Left)
        //    distance *= -1;

        auto& tc = m_Entity.Transform();
        auto& sc = m_Entity.Sprite();

        b2Body* body = LK_CAST_TO_b2Body(m_Entity.RigidBody2D().RuntimeBody);
        auto& bodyPos = body->GetPosition();

        m_Pos.x = bodyPos.x;
        m_Pos.y = bodyPos.y;

        //m_Pos.x += distance;
        tc.Translation.x = m_Pos.x;
        tc.Translation.y = m_Pos.y;
        //body->SetTransform(b2Vec2(m_Pos.x, m_Pos.y), 0.0f);

        // Whenever the enemy sprite is 'outside' of the screen to the left, reset the position to the right
        if (tc.Translation.x < m_EndDistance)
            Respawn();
    }

    void Enemy::OnImGuiRender()
    {
        auto* editor = EditorLayer::Get();
        if (editor && SELECTION::SelectedEntity != m_Entity)
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