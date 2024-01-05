#pragma once

#include "LkEngine/LkEngine.h"


namespace LkEngine {

    enum class WalkDirection
    {
        None = 0,
        Left,
        Right,
    };

    struct PlayerMetadata
    {
        glm::vec2 LastJumpPos = { 0.0f, 0.0f };
        bool JumpActive = false;
        llong JumpStarted = (llong)0;
    };

    class Player : public RuntimeAsset
    {
    public:
        Player(Entity& entity, const std::string& name = "");
        virtual ~Player();

        void OnUpdate(float ts = 1.0f);
        void OnImGuiRender();
        void Setup() override;
        void Destroy() override;

        glm::vec3 GetPos() const { return m_Pos; }
        glm::vec3& GetPos() { return m_Pos; }
        std::string GetName() const { return m_Name; }
        float GetWidth();
        float GetHeight();
        const glm::vec2 GetSize();
        void SetSize(float x, float y);
        void SetSize(const glm::vec2& size);
        UUID GetUUID() { return m_Entity.GetComponent<IDComponent>(); }
        Entity GetEntity() const { return m_Entity; }
        Entity& GetEntity() { return m_Entity; }
        void SetPos(float x, float y);
        void SetPos(const glm::vec2& pos);
        SceneCamera& GetCamera() { return m_Entity.GetComponent<CameraComponent>(); }
        PlayerMetadata& GetMetadata() { return m_Metadata; }

        virtual void SetSpawnPoint(const glm::vec2& spawnPoint);
        virtual glm::vec2 GetSpawnPoint() const { return m_SpawnPoint; }
        virtual void Respawn();

    protected:
        Entity m_Entity;
        std::string m_Name;
        PlayerMetadata m_Metadata;

        float m_Health = 5.0f; 
        float m_TravelSpeed = 1.0f;
        float m_JumpHeight = 1.0f;
        bool m_Jumping = false;
        float m_TotalJumpTime = 1.0f; // 1 second for an entire jump, so 0.5s up and 0.5s down
        Timer m_JumpTimer; 

        Entity m_GunEntity;

        glm::vec3 m_Pos = { 0.0f, 0.0f, 0.0f };
        glm::vec2 m_SpawnPoint = { 0.0f, 0.0f };
    };

}