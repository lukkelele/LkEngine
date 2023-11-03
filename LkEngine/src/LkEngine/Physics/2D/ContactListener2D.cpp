#include "LKpch.h"
#include "LkEngine/Physics/2D/ContactListener2D.h"
#include "LkEngine/Core/Base.h"


namespace LkEngine {

    ContactListener2D::ContactListener2D()
    {
    }

    ContactListener2D::~ContactListener2D()
    {
    }

    void ContactListener2D::BeginContact(b2Contact* contact)
    {
        Scene* scene = Scene::GetActiveScene();

        UUID aID = (UUID)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
        UUID bID = (UUID)contact->GetFixtureB()->GetBody()->GetUserData().pointer;

        Entity a = scene->GetEntityWithUUID(aID);
        Entity b = scene->GetEntityWithUUID(bID);

        // Exit if no contact
        if (!a || !b)
        {
            return;
        }

        //callOnCollision2DBegin(a, b);
        //callOnCollision2DBegin(b, a);
    }

    void ContactListener2D::EndContact(b2Contact* contact) 
    {
    }

}