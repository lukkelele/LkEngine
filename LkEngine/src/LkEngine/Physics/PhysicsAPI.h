#pragma once

#include "LkEngine/Core/Base.h"


namespace LkEngine {

    class PhysicsAPI
    {
    public:
        static s_ptr<PhysicsAPI> Create();
        static PhysicsAPI* Get() { return m_PhysicsAPI; }

    protected:
        inline static PhysicsAPI* m_PhysicsAPI = nullptr;
    };

}