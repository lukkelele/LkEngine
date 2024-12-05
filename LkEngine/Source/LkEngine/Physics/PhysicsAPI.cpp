#include "LKpch.h"
#include "PhysicsAPI.h"

#if defined(LK_PHYSICS_API_BULLET3)
#	include "LkEngine/Physics/Bullet/BulletPhysics.h"
#else
#	include "LkEngine/Physics/OtherPhysicsSDK/OtherPhysicsSDK.h"
#endif

namespace LkEngine {

    std::shared_ptr<PhysicsAPI> PhysicsAPI::Create()
    {
    #if defined(LK_PHYSICS_API_BULLET3)
        return std::make_shared<BulletPhysics>();
	#else
	#	error "Unknown physics API"
		LK_CORE_ASSERT(false, "Unknown physics API");
        return nullptr;
    #endif
    }

}
