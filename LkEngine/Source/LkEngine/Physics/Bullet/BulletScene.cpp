#include "LKpch.h"
#include "BulletScene.h"


namespace LkEngine {

    BulletScene::BulletScene()
    {
        // empty for now
    }

    BulletScene::~BulletScene()
    {
        // empty for now
    }

    void BulletScene::Simulate(const float DeltaTime)
    {
        // empty for now
    }

    void BulletScene::Destroy()
    {
        // empty for now
    }

    TSharedPtr<PhysicsBody> BulletScene::GetEntityBodyByID(UUID entityID) const
    {
        return nullptr;
    }

    void BulletScene::DestroyBody(LEntity Entity)
    {
    }

    float BulletScene::GetGravity() const
    {
        return 0.0f;
    }

}