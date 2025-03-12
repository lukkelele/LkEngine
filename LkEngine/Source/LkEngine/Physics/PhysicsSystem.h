#pragma once

#include "PhysicsAPI.h"

namespace LkEngine {

    enum class EPhysicsAPI
    {
        None = 0,
        Bullet3,
    };

    class LPhysicsSystem
    {
    public:
        LPhysicsSystem();
        ~LPhysicsSystem() = default;

        void Simulate(const float DeltaTime);

        static EPhysicsAPI GetAPI() { return API; }

		static void SetAPI(const EPhysicsAPI InAPI);

    private:
		float Timestep = 0.0f;

        inline static EPhysicsAPI API = EPhysicsAPI::None;
    };

}
