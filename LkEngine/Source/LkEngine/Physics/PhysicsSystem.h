#pragma once

#include "PhysicsAPI.h"

#include "LkEngine/Physics2D/Physics2D.h"


namespace LkEngine {

    enum class PhysicsAPIType
    {
        None = 0,
        Bullet
    };

    class PhysicsSystem
    {
    public:
        PhysicsSystem();
        ~PhysicsSystem() = default;

        static PhysicsSystem* Get() { return m_Instance; }
        static void SetPhysicsAPI(PhysicsAPIType physicsAPI);

        void Init();
        void Simulate(Timestep ts);
        PhysicsAPIType GetPhysicsAPIType() const { return m_PhysicsAPIType; }
        Timestep GetStep() const { return m_Step; }

    private:
        Timestep m_Step = (1.0f / 60);

        inline static PhysicsAPIType m_PhysicsAPIType;
        inline static PhysicsSystem* m_Instance = nullptr;
    };

}