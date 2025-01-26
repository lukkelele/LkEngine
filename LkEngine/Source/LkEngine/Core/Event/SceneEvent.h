#pragma once

#include "Event.h"

#include "LkEngine/Scene/Scene.h"


namespace LkEngine {

    class SceneCreatedEvent : public LEvent
    {
	public:
		/// USE HARDREF's here
		SceneCreatedEvent(TObjectPtr<LScene> InCreatedScene)
			: Scene(InCreatedScene) 
		{
		}

		FORCEINLINE TObjectPtr<LScene> GetScene() 
		{ 
			return Scene; 
		}

		EEventType GetType() const { return EEventType::SceneCreated; }
		const char* GetName() const override { return "SceneCreated"; }

	private:
		TObjectPtr<LScene> Scene = nullptr;
    };


    class SceneDeletedEvent : public LEvent
    {
	public:
		/// USE HARDREF's here
		SceneDeletedEvent(TObjectPtr<LScene> scene)
			: m_Scene(scene) 
		{
		}

		TObjectPtr<LScene> GetScene() 
		{ 
			return m_Scene; 
		}

		EEventType GetType() const { return EEventType::SceneDeleted; }
		const char* GetName() const override { return "SceneDeleted"; }

	private:
		TObjectPtr<LScene> m_Scene = nullptr;
    };

}
