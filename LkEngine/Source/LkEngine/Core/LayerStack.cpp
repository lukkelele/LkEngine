#include "LKpch.h"
#include "LkEngine/Core/LayerStack.h"


namespace LkEngine {

	LLayerStack::~LLayerStack()
	{
		for (LLayer* Layer : m_Layers)
		{
			PopLayer(Layer);
		}
	}

	void LLayerStack::PushLayer(LLayer* Layer)
	{
		auto Iter = m_Layers.emplace((m_Layers.begin() + m_LayerInsertIndex), Layer);
		m_LayerInsertIndex++;

		(*Iter)->OnAttach();
	}

	void LLayerStack::PushOverlay(LLayer* Overlay)
	{
		auto Iter = m_Layers.emplace_back(Overlay);

		Iter->OnAttach();
	}

	void LLayerStack::PopLayer(LLayer* Layer)
	{
		auto Iter = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, Layer);
		if (Iter != (m_Layers.begin() + m_LayerInsertIndex))
		{
			Layer->OnDetach();

			m_Layers.erase(Iter);
			m_LayerInsertIndex--;
		}
	}

	void LLayerStack::PopOverlay(LLayer* Overlay)
	{
		auto Iter = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), Overlay);
		if (Iter != m_Layers.end())
		{
			Overlay->OnDetach();

			m_Layers.erase(Iter);
		}
	}
}