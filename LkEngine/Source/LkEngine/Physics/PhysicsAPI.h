/**
 * @file
 * @brief Physics API Interface.
 */
#pragma once

/**
 * @defgroup Physics
 */

namespace LkEngine {

    /**
     * @interface IPhysicsAPI
	 *
	 * Interface for physics APIs.
	 *
	 * @note Supports: Bullet3, Box2D.
     */
    class IPhysicsAPI
    {
    public:
		virtual ~IPhysicsAPI() = default;
    };

}
