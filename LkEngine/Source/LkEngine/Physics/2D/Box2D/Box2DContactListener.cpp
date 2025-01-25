#include "LKpch.h"
#include "Box2DContactListener.h"

#include "LkEngine/Core/Core.h"
#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"


namespace LkEngine {

    /// FIXME: DISABLED / REMOVE LATER ON
    #if 0
    Box2DContactListener::Box2DContactListener()
    {
    }

    Box2DContactListener::~Box2DContactListener()
    {
    }

    void Box2DContactListener::BeginContact(b2Contact* contact)
    {
        if (TObjectPtr<LScene> Scene = LScene::GetActiveScene())
        {
			UUID aID = (UUID)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
			UUID bID = (UUID)contact->GetFixtureB()->GetBody()->GetUserData().pointer;

			LEntity a = Scene->GetEntityWithUUID(aID);
			LEntity b = Scene->GetEntityWithUUID(bID);
			LK_CORE_DEBUG_TAG("Box2DContactListener", "BeginContact --> ({}   and   {})", a.Name(), b.Name());

			/* Exit if no contact.*/
			if (!a || !b)
			{
				return;
			}
        }
    }

    void Box2DContactListener::EndContact(b2Contact* contact) 
    {
        if (TObjectPtr<LScene> Scene = LScene::GetActiveScene())
        {
			UUID aID = (UUID)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
			UUID bID = (UUID)contact->GetFixtureB()->GetBody()->GetUserData().pointer;

			LEntity a = Scene->GetEntityWithUUID(aID);
			LEntity b = Scene->GetEntityWithUUID(bID);
			LK_CORE_DEBUG_TAG("Box2DContactListener", "EndContact --> ({}   and   {})", a.Name(), b.Name());
        }
    }
    #endif

}