#pragma once

#include "PhysicsAPI.h"

#include "LkEngine/Physics/2D/Physics2D.h"


namespace LkEngine {

    enum class EPhysicsAPI
    {
        Null = 0,
        Bullet3, /* bullet3 */
    };

    class PhysicsSystem
    {
    public:
        PhysicsSystem();
        ~PhysicsSystem() = default;

        void Initialize() {}
        void Simulate(const FTimestep DeltaTime);

        FORCEINLINE static EPhysicsAPI GetPhysicsAPI() { return PhysicsAPI; }

        FORCEINLINE static void SetPhysicsAPI(EPhysicsAPI InPhysicsAPI) 
        { 
            PhysicsAPI = InPhysicsAPI; 
        }

        /// REMOVE ?
        static PhysicsSystem* Get() { return m_Instance; }

    private:
        FTimestep Timestep = (1.0f / 60.0f);

        inline static EPhysicsAPI PhysicsAPI = EPhysicsAPI::Null;

        inline static PhysicsSystem* m_Instance = nullptr;
    };

}
