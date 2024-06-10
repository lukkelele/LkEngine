#pragma once

#include "PhysicsBody.h"


namespace LkEngine {

    class PhysicsScene
    {
    public:
        virtual ~PhysicsScene() = default;

		virtual void Simulate(float ts) = 0;
		virtual void Destroy() = 0;

		s_ptr<PhysicsBody> GetEntityBodyByID(UUID entityID) const;
		s_ptr<PhysicsBody> GetEntityBody(Entity entity) const { return GetEntityBodyByID(entity.GetUUID()); }
		virtual void DestroyBody(Entity entity) = 0;

        virtual float GetGravity() const = 0;
    };

}