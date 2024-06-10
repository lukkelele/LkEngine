#include "LKpch.h"
#include "BulletPhysics.h"
#include "LkEngine/Physics/PhysicsSystem.h"


namespace LkEngine {

    BulletPhysics::BulletPhysics()
    {
        m_PhysicsAPI = this;
        PhysicsSystem::SetPhysicsAPI(EPhysicsAPI::Bullet3);
    }

}