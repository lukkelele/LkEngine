#pragma once

#include "LkEngine/LkEngine.h"


namespace LkEngine {

    class Player
    {
    public:
        Player(const std::string& name = "Player");
        ~Player();

        std::string GetName() const { return m_Name; }

        void SetUUID(UUID& id) { m_ID = id; }
        UUID GetUUID() const { return m_ID; }
        Entity GetEntity() const { return m_Entity; }
        void SetEntity(const Entity& entity); // { m_Entity = entity; }

        void OnUpdate(float ts = 1.0f);
        void OnImGuiRender();

    private:
        UUID m_ID;
        std::string m_Name;
        float m_Health = 5.0f; // 
        float m_Speed = 1.0f;
        float m_JumpHeight = 50.0f;
        glm::vec2 m_Pos = { 0.0f, 0.0f };
        glm::vec4 m_Color = { 1.0f, 1.0f, 1.0f, 1.0f }; // Remove

        Entity m_Entity;
    };

}