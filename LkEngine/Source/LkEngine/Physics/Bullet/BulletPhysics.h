#pragma once

#include "LkEngine/Physics/PhysicsAPI.h"

namespace LkEngine {

    class LBulletPhysics : public IPhysicsAPI
    {
    public:
        LBulletPhysics() = default;
        ~LBulletPhysics() = default;
    };

}
