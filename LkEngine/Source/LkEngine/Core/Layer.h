#pragma once

#include "LkEngine/Core/Core.h"
#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "LkEngine/Core/Event/Event.h"

namespace LkEngine {

	/**
	 * @class LLayer
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

		virtual void Tick(const float DeltaTime) = 0;
		virtual void RenderUI() {};

		virtual void OnEvent(LEvent& Event) { LK_UNUSED(Event); }

		FORCEINLINE std::string_view GetName() const { return Name; }

	protected:
		std::string Name{};
	private:
		LCLASS(LLayer);
	};
}
