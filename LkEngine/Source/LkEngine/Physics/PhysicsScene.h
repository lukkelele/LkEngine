#pragma once

/** @fixme: FIXME */
#if 0
#include "PhysicsBody.h"
#endif

namespace LkEngine {

#if 0
    class PhysicsScene
    {
    public:
        virtual ~PhysicsScene() = default;

		virtual void Simulate(const float DeltaTime) = 0;
		virtual void Destroy() = 0;

		virtual std::shared_ptr<PhysicsBody> GetEntityBodyByID(LUUID EntityID) const = 0;
		virtual std::shared_ptr<PhysicsBody> GetEntityBody(LEntity Entity) const 
        { 
            return GetEntityBodyByID(Entity.GetUUID()); 
        }

		virtual void DestroyBody(LEntity Entity) = 0;

        virtual float GetGravity() const = 0;
    };
#endif

}
