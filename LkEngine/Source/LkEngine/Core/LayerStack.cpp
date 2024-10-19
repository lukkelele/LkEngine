#include "LKpch.h"
#include "LkEngine/Core/LayerStack.h"


namespace LkEngine {

	LLayerStack::~LLayerStack()
	{
		LK_CORE_TRACE_TAG("LayerStack", "Releasing {} layers", Layers.size());
		for (const TObjectPtr<LLayer>& Layer : Layers)
		{
			PopLayer(Layer);
		}
	}

}