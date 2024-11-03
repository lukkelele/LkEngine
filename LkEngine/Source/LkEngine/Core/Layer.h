#pragma once

#include "LkEngine/Core/Core.h"
#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "LkEngine/Core/Event/Event.h"


namespace LkEngine {

	/**
	 * LLayer
	 *
	 * Base layer implementation.
	 */
	class LLayer : public LObject
	{
	public:
		LLayer(std::string_view InName = "Layer");
		virtual ~LLayer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnUpdate(const float Timestep) = 0;

		virtual void OnRender() {};
		virtual void OnRenderUI() {}

		/* TODO: Need to figure out the proper handling of events. */
		virtual void OnEvent(LEvent& Event)
		{
			LK_UNUSED(Event);
		}

		FORCEINLINE std::string_view GetName() const { return Name; }

	protected:
		std::string Name{};
	private:
		LCLASS(LLayer);
	};
}
