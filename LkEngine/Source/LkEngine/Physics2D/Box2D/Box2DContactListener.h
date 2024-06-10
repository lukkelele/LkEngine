#pragma once

#include <box2d/box2d.h>

//
// TODO: This will get implemented in the future, or just get removed.
//       I'm not entirely sure if i will ever change the 2D physics system but it could be nice to be able to switch
//       between multiple API's. Just for the sake of it making everything more difficult
// 

namespace LkEngine {

    class Box2DContactListener : public b2ContactListener
    {
    public:
        Box2DContactListener();
        ~Box2DContactListener();

        virtual void BeginContact(b2Contact* contact) override;
        virtual void EndContact(b2Contact* contact) override;

        virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override
        {
            B2_NOT_USED(contact);
            B2_NOT_USED(oldManifold);
        }

        virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override
        {
            B2_NOT_USED(contact);
            B2_NOT_USED(impulse);
        }

    };

}