#include "LKpch.h"
#include "BulletScene.h"


namespace LkEngine {

    BulletScene::BulletScene()
    {
    }

    BulletScene::~BulletScene()
    {
    }

    s_ptr<PhysicsBody> BulletScene::GetEntityBodyByID(UUID entityID) const
    {
        return nullptr;
    }

    void BulletScene::DestroyBody(Entity entity)
    {
    }

    float BulletScene::GetGravity() const
    {
        return 0.0f;
    }

}