#pragma once

#include "LkEngine/Physics/PhysicsScene.h"


namespace LkEngine {

    class BulletScene : public PhysicsScene
    {
    public:
        BulletScene();
        ~BulletScene();

        virtual void Simulate(const float DeltaTime) override;
        virtual void Destroy() override;

		virtual TSharedPtr<PhysicsBody> GetEntityBodyByID(UUID entityID) const override;
        virtual void DestroyBody(LEntity Entity) override;

        virtual float GetGravity() const override;

    };

}