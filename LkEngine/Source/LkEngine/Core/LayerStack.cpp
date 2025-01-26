#include "LKpch.h"
#include "LkEngine/Core/LayerStack.h"


namespace LkEngine {

	LLayerStack::~LLayerStack()
	{
		if (!Layers.empty())
		{
			Destroy();
		}
	}

	void LLayerStack::Destroy()
	{
		LK_CORE_DEBUG_TAG("LayerStack", "Releasing {} layers", Layers.size());

		/* Pop overlays first. */
		for (auto Iter = Layers.begin() + InsertIndex; Iter != Layers.end();)
		{
			LK_CORE_TRACE_TAG("LayerStack", "Popping overlay: {} ({}) (layers left: {})", 
							  (*Iter)->GetName(), (*Iter)->GetClass()->GetName(), Layers.size() - 1);
			(*Iter)->OnDetach();
			Iter = Layers.erase(Iter);
		}

		/* Pop the rest of the layers. */
		for (auto Iter = Layers.begin(); Iter != Layers.end();)
		{
			LK_CORE_TRACE_TAG("LayerStack", "Popping layer: {} ({}) (layers left: {})", 
							  (*Iter)->GetName(), (*Iter)->GetClass()->GetName(), Layers.size() - 1);
			(*Iter)->OnDetach();
			Iter = Layers.erase(Iter);
			InsertIndex--;
		}
	}

}
