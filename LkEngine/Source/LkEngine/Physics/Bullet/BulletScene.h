#pragma once

#if 0
#include "LkEngine/Physics/PhysicsScene.h"
#endif

namespace LkEngine {

#if 0
    class BulletScene : public PhysicsScene
    {
    public:
        BulletScene();
        ~BulletScene();

        virtual void Simulate(const float DeltaTime) override;
        virtual void Destroy() override;

		virtual std::shared_ptr<PhysicsBody> GetEntityBodyByID(LUUID entityID) const override;
        virtual void DestroyBody(LEntity Entity) override;

        virtual float GetGravity() const override;

    };
#endif

}
