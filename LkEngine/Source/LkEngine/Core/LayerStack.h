#pragma once

#include "Layer.h"


namespace LkEngine {

	class LLayerStack : public LObject
	{
	public:
		LLayerStack() = default;
		~LLayerStack();

		void PushLayer(LLayer* layer);
		void PushOverlay(LLayer* overlay);
		void PopLayer(LLayer* layer);
		void PopOverlay(LLayer* overlay);

		LLayer* operator[](const size_t LayerIndex)
		{
			LK_ASSERT((LayerIndex >= 0) && (LayerIndex < m_Layers.size()), "Invalid layer index: {}", LayerIndex);
			return m_Layers[LayerIndex];
		}

		FORCEINLINE uint8_t Size() const { return m_Layers.size(); }

		std::vector<LLayer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<LLayer*>::iterator end() { return m_Layers.end(); }

	private:
		std::vector<LLayer*> m_Layers;
		unsigned int m_LayerInsertIndex = 0;

		LCLASS(LLayerStack);
	};


}
