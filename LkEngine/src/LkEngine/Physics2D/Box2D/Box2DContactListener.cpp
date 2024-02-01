#include "LKpch.h"
#include "Box2DContactListener.h"
#include "LkEngine/Core/Base.h"
#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"


namespace LkEngine {

    Box2DContactListener::Box2DContactListener()
    {
    }

    Box2DContactListener::~Box2DContactListener()
    {
    }

    void Box2DContactListener::BeginContact(b2Contact* contact)
    {
        auto scene = Scene::GetActiveScene();

        UUID aID = (UUID)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
        UUID bID = (UUID)contact->GetFixtureB()->GetBody()->GetUserData().pointer;

        Entity a = scene->GetEntityWithUUID(aID);
        Entity b = scene->GetEntityWithUUID(bID);

        LK_CORE_WARN("BeginContact --> ({}   and   {})", a.Name(), b.Name());
        // Exit if no contact
        if (!a || !b)
        {
            return;
        }
    }

    void Box2DContactListener::EndContact(b2Contact* contact) 
    {
        auto scene = Scene::GetActiveScene();

        UUID aID = (UUID)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
        UUID bID = (UUID)contact->GetFixtureB()->GetBody()->GetUserData().pointer;

        Entity a = scene->GetEntityWithUUID(aID);
        Entity b = scene->GetEntityWithUUID(bID);

        LK_CORE_WARN("EndContact --> ({}   and   {})", a.Name(), b.Name());
    }

}