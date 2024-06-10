#include "LKpch.h"
#include "PhysicsAPI.h"

#ifdef LK_PHYSICS_API_BULLET3
    #include "LkEngine/Physics/Bullet/BulletPhysics.h"
#else
    #include "LkEngine/Physics/OtherPhysicsSDK/OtherPhysicsSDK.h"
#endif

namespace LkEngine {

    TSharedPtr<PhysicsAPI> PhysicsAPI::Create()
    {
    #ifdef LK_PHYSICS_API_BULLET3
        return std::make_shared<BulletPhysics>();
    #else
        return nullptr;
    #endif
    }

}