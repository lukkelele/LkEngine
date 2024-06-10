#include "LKpch.h"
#include "ContactListener2D.h"

#include "LkEngine/Core/Base.h"
#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"


namespace LkEngine {

    ContactListener2D::ContactListener2D()
    {
    }

    ContactListener2D::~ContactListener2D()
    {
    }

    void ContactListener2D::BeginContact(b2Contact* contact)
    {
        auto scene = Scene::GetActiveScene();

        UUID aID = (UUID)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
        UUID bID = (UUID)contact->GetFixtureB()->GetBody()->GetUserData().pointer;

        LEntity a = scene->GetEntityWithUUID(aID);
        LEntity b = scene->GetEntityWithUUID(bID);

        /* Exit if no contact. */
        if (!a || !b)
        {
            return;
        }

        auto& aBody = a.RigidBody2D();
        auto& bBody = b.RigidBody2D();

        // If either body is static, exit.
        // This is because of debugging purposes to track dynamic collisions
        if (aBody.BodyType == RigidBody2DComponent::Type::Static 
            || bBody.BodyType == RigidBody2DComponent::Type::Static)
        {
            return;
        }
        

        LK_CORE_WARN("BeginContact --> ({}   and   {})", a.Name(), b.Name());
        //callOnCollision2DBegin(a, b);
        //callOnCollision2DBegin(b, a);
    }

    void ContactListener2D::EndContact(b2Contact* contact) 
    {
        //LK_CORE_WARN("ContactListener: EndContact");
        auto scene = Scene::GetActiveScene();
        UUID aID = (UUID)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
        UUID bID = (UUID)contact->GetFixtureB()->GetBody()->GetUserData().pointer;
        LEntity a = scene->GetEntityWithUUID(aID);
        LEntity b = scene->GetEntityWithUUID(bID);

        /* Exit if no contact. */
        if (!a || !b)
        {
            return;
        }

        auto& aBody = a.RigidBody2D();
        auto& bBody = b.RigidBody2D();

        // If either body is static, exit.
        // This is because of debugging purposes to track dynamic collisions
        if (aBody.BodyType == RigidBody2DComponent::Type::Static 
            || bBody.BodyType == RigidBody2DComponent::Type::Static)
        {
            return;
        }


        LK_CORE_WARN("EndContact --> ({}   and   {})", a.Name(), b.Name());
    }

}