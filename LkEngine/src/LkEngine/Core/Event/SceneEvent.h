#pragma once

#include "Event.h"

#include "LkEngine/Scene/Scene.h"


namespace LkEngine {

    class SceneCreatedEvent : public Event
    {
	public:
		SceneCreatedEvent(Ref<Scene> scene)
			: m_Scene(scene) {}

		Ref<Scene> GetScene() { return m_Scene; }

		EventType GetEventType() const { return EventType::SceneCreated; }
		const char* GetName() const override { return "SceneCreated"; }

	private:
		Ref<Scene> m_Scene = nullptr;
    };


    class SceneDeletedEvent : public Event
    {
	public:
		SceneDeletedEvent(Ref<Scene> scene)
			: m_Scene(scene) {}

		Ref<Scene> GetScene() { return m_Scene; }

		EventType GetEventType() const { return EventType::SceneDeleted; }
		const char* GetName() const override { return "SceneDeleted"; }

	private:
		Ref<Scene> m_Scene = nullptr;
    };

}
