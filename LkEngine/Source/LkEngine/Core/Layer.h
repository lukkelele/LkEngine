#pragma once

#include "LkEngine/Core/Core.h"
#include "LkEngine/Core/LObject/Object.h"

#include "LkEngine/Core/Event/Event.h"


namespace LkEngine {

	/**
	 * LLayer
	 *
	 * Base layer implementation.
	 */
	class LLayer : public virtual LObject
	{
	public:
		LLayer(std::string_view InName = "Layer");
		virtual ~LLayer() = default;

		/** */
		virtual void OnAttach() {}
		/** */
		virtual void OnDetach() {}

		/** */
		virtual void OnUpdate(float ts) {}
		/** */
		virtual void OnImGuiRender() {}

		virtual void OnEvent(LEvent& event) 
		{
		}

		std::string_view GetName() const { return Name; }

	protected:
		std::string Name{};

	private:
		LCLASS(LLayer);
	};
}
