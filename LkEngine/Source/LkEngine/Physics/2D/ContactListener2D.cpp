#include "LKpch.h"
#include "ContactListener2D.h"

#if 0
#include "LkEngine/Core/Core.h"
#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"
#endif

namespace LkEngine {

    #if 0 /// FIXME
    ContactListener2D::ContactListener2D()
    {
    }

    ContactListener2D::~ContactListener2D()
    {
    }

    void ContactListener2D::BeginContact(b2Contact* contact)
    {
        auto scene = LScene::GetActiveScene();

        UUID aID = (UUID)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
        UUID bID = (UUID)contact->GetFixtureB()->GetBody()->GetUserData().pointer;

        LEntity a = scene->GetEntityWithUUID(aID);
        LEntity b = scene->GetEntityWithUUID(bID);

        /* Exit if no contact. */
        if (!a || !b)
        {
            return;
        }

        auto& aBody = a.GetComponent<LRigidBody2DComponent>();
        auto& bBody = b.GetComponent<LRigidBody2DComponent>();

        // If either body is static, exit.
        // This is because of debugging purposes to track dynamic collisions
        if ((aBody.BodyType == LRigidBody2DComponent::Type::Static)
            || (bBody.BodyType == LRigidBody2DComponent::Type::Static))
        {
            return;
        }

        LK_CORE_DEBUG_TAG("ContactListener2D", "BeginContact --> ({}   and   {})", a.Name(), b.Name());
        //callOnCollision2DBegin(a, b);
        //callOnCollision2DBegin(b, a);
    }

    void ContactListener2D::EndContact(b2Contact* contact) 
    {
        if (TObjectPtr<LScene> scene = LScene::GetActiveScene())
        {
			UUID aID = (UUID)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
			UUID bID = (UUID)contact->GetFixtureB()->GetBody()->GetUserData().pointer;
			LEntity a = scene->GetEntityWithUUID(aID);
			LEntity b = scene->GetEntityWithUUID(bID);

			/* Exit if no contact. */
			if (!a || !b)
			{
				return;
			}

			auto& aBody = a.GetComponent<LRigidBody2DComponent>();
			auto& bBody = b.GetComponent<LRigidBody2DComponent>();

			// If either body is static, exit.
			if ((aBody.BodyType == LRigidBody2DComponent::Type::Static)
				|| (bBody.BodyType == LRigidBody2DComponent::Type::Static))
			{
				return;
			}

			LK_CORE_DEBUG_TAG("ContactListener2D", "EndContact --> ({}   and   {})", a.Name(), b.Name());
        }
    }

    #endif
}