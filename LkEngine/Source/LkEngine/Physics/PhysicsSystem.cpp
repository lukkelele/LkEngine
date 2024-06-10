#include "LKpch.h"
#include "PhysicsSystem.h"


namespace LkEngine {

    PhysicsSystem::PhysicsSystem()
    {
        m_Instance = this;
    }

    void PhysicsSystem::Init()
    {
    }

    void PhysicsSystem::Simulate(const FTimestep DeltaTime)
    {
        // 2D Physics
		static signed int VelocityIterations = 20;
		static signed int PositionIterations = 8;

#if 0
        auto activeScene = Scene::GetActiveScene();
        if (activeScene)
        {
	        auto& world = activeScene->GetBox2DWorld();
	        world.World->Step((1.0f / 60.0f), velocityIterations, positionIterations);
        }
#endif
    }


}

