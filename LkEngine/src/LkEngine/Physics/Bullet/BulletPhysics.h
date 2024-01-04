#pragma once

#include "LkEngine/Physics/PhysicsAPI.h"
#include "LkEngine/Scene/Entity.h"

namespace LkEngine {

    class BulletPhysics : public PhysicsAPI
    {
    public:
        BulletPhysics();
        ~BulletPhysics() = default;
    };

}