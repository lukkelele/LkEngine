#include "LKpch.h"
#include "PhysicsSystem.h"


namespace LkEngine {

    void PhysicsSystem::SetPhysicsAPI(PhysicsAPIType physicsAPI)
    {
        m_PhysicsAPIType = physicsAPI;
    }

    PhysicsSystem::PhysicsSystem()
    {
        m_Instance = this;
    }

    void PhysicsSystem::Init()
    {
    }

    void PhysicsSystem::Simulate(Timestep ts)
    {
        //m_PhysicsAPIType;
    }


}

