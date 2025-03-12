#pragma once

#include <box2d/box2d.h>
#include <box2d/collision.h>

namespace LkEngine {

    #if 0 /// FIXME
    class ContactListener2D : public b2ContactListener
    {
    public:
        ContactListener2D();
        virtual ~ContactListener2D();

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
    #endif

}