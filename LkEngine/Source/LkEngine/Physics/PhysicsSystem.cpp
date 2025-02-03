#include "LKpch.h"
#include "PhysicsSystem.h"


namespace LkEngine {

    LPhysicsSystem::LPhysicsSystem()
    {
    }

    void LPhysicsSystem::Simulate(const float DeltaTime)
    {
#if 0
        /* 2D Physics. */
		static signed int VelocityIterations = 20;
		static signed int PositionIterations = 8;

        auto activeScene = Scene::GetActiveScene();
        if (activeScene)
        {
	        auto& world = activeScene->GetBox2DWorld();
	        world.World->Step((1.0f / 60.0f), velocityIterations, positionIterations);
        }
#endif
    }

	void LPhysicsSystem::SetAPI(const EPhysicsAPI InAPI)
	{
	}

}

