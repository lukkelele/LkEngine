#pragma once

#include "LkEngine/Physics/PhysicsScene.h"


namespace LkEngine {

    class BulletScene : public PhysicsScene
    {
    public:
        BulletScene();
        ~BulletScene();

		s_ptr<PhysicsBody> GetEntityBodyByID(UUID entityID) const;
        void DestroyBody(Entity entity);

        float GetGravity() const;
    };

}