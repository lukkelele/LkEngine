#include "LKpch.h"
#include "LkEngine/Core/LayerStack.h"


namespace LkEngine {

	LLayerStack::~LLayerStack()
	{
		LK_CORE_DEBUG_TAG("LayerStack", "Releasing {} layers", Layers.size());

		/* Pop overlays first (from InsertIndex to the end). */
		for (auto Iter = Layers.begin() + InsertIndex; Iter != Layers.end();)
		{
			LK_CORE_DEBUG_TAG("LayerStack", "Popping overlay: {} ({})", (*Iter)->GetName(), (*Iter)->GetClass()->GetName());
			(*Iter)->OnDetach();
			Iter = Layers.erase(Iter);
		}

		/* Pop regular layers (from the beginning to InsertIndex). */
		for (auto Iter = Layers.begin(); Iter != Layers.end();)
		{
			LK_CORE_DEBUG_TAG("LayerStack", "Popping layer: {} ({})", (*Iter)->GetName(), (*Iter)->GetClass()->GetName());
			(*Iter)->OnDetach();
			Iter = Layers.erase(Iter);
			InsertIndex--;
		}
	}

}
