#pragma once

#include "LkEngine/LkEngine.h"
#include "LkEngine/Scene/Components.h"

namespace LkEngine {

    enum class WalkDirection
    {
        None = 0,
        Left,
        Right,
    };

    class Player
    {
    public:
        Player(Entity& entity, const std::string& name = "");
        virtual ~Player();

        std::string GetName() const { return m_Name; }

        void SetUUID(UUID& id) { m_ID = id; }
        UUID GetUUID() const { return m_ID; }
        Entity GetEntity() const { return m_Entity; }
        void SetEntity(const Entity& entity); // { m_Entity = entity; }
        void SetPos(float x, float y);
        void SetPos(const glm::vec2& pos);
        SceneCamera& GetCamera() { return *m_Entity.GetComponent<CameraComponent>().CameraRef; }
        //SceneCamera& GetCamera() { return m_Entity.GetComponent<CameraComponent>(); }

        const glm::vec2 GetSize();
        float GetWidth();
        float GetHeight();

        virtual void OnUpdate(float ts = 1.0f);
        virtual void OnImGuiRender();

    protected:
        glm::vec3& GetPos() { return m_Entity.GetComponent<TransformComponent>().Translation; }

    protected:
        UUID m_ID;
        std::string m_Name;
        float m_Health = 5.0f; // 
        float m_Speed = 1.0f;
        float m_JumpHeight = 50.0f;
        glm::vec2 m_Pos = { 0.0f, 0.0f };
        //glm::vec2 m_CameraOffset = { 140.0f, 180.0f };
        glm::vec2 m_CameraOffset = { 0.0f, 0.0f };

        Entity m_Entity;
    };

}