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
        // 2D Physics
		static signed int velocityIterations = 20;
		static signed int positionIterations = 8;
	    auto& world = Scene::GetActiveScene()->GetBox2DWorld();
	    world.World->Step((1.0f / 60.0f), velocityIterations, positionIterations);
    }


}

