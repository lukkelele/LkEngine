#pragma once

#include "LkEngine/Scene/Entity.h"


namespace LkEngine {

    class PhysicsBody
    {
    public:
		PhysicsBody(LEntity Entity)
			: m_Entity(Entity)
		{
		}

		virtual ~PhysicsBody()
		{
		}

		FORCEINLINE LEntity GetEntity() 
		{ 
			return m_Entity; 
		}

		FORCEINLINE const LEntity& GetEntity() const 
		{ 
			return m_Entity; 
		}

	protected:
		LEntity m_Entity;
    };

}