#pragma once

#include "LkEngine/Scene/Entity.h"


namespace LkEngine {

    class PhysicsBody
    {
    public:
		PhysicsBody(Entity entity)
			: m_Entity(entity)
		{
		}

		virtual ~PhysicsBody() = default;

		Entity GetEntity() { return m_Entity; }
		const Entity& GetEntity() const { return m_Entity; }

	protected:
		Entity m_Entity;
    };

}