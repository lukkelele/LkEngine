#pragma once

#include "LkEngine/Core/Core.h"

namespace LkEngine {

    class PhysicsAPI
    {
    public:
        static TSharedPtr<PhysicsAPI> Create();
        static PhysicsAPI* Get() { return m_PhysicsAPI; }

    protected:
        inline static PhysicsAPI* m_PhysicsAPI = nullptr;
    };

}