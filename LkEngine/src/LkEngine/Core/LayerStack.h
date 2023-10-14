#pragma once
#include "LkEngine/Core/Base.h"
#include "LkEngine/Core/Layer.h"

namespace LkEngine {

	class LayerStack
	{
	public:
		LayerStack() = default;
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		std::vector<Layer*>::iterator Begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator End() { return m_Layers.end(); }
		std::vector<Layer*>::reverse_iterator rBegin() { return m_Layers.rbegin(); }
		std::vector<Layer*>::reverse_iterator rEnd() { return m_Layers.rend(); }

		std::vector<Layer*>::const_iterator Begin() const { return m_Layers.begin(); }
		std::vector<Layer*>::const_iterator End()	const { return m_Layers.end(); }
		std::vector<Layer*>::const_reverse_iterator rBegin() const { return m_Layers.rbegin(); }
		std::vector<Layer*>::const_reverse_iterator rEnd() const { return m_Layers.rend(); }
	private:
		std::vector<Layer*> m_Layers;
		unsigned int m_LayerInsertIndex = 0;
	};


}
