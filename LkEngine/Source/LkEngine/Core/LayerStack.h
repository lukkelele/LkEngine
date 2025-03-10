#pragma once

#include "Layer.h"


namespace LkEngine {

	class LLayerStack : public LObject
	{
	public:
		LLayerStack() = default;
		~LLayerStack();

		void Destroy();

		FORCEINLINE void PushLayer(LLayer* Layer)
		{
			if (auto Iter = Layers.emplace((Layers.begin() + InsertIndex), Layer); Iter != Layers.end())
			{
				InsertIndex++;
				(*Iter)->OnAttach();
			}
		}

		FORCEINLINE void PushLayer(TObjectPtr<LLayer> Layer)
		{
			if (auto Iter = Layers.emplace((Layers.begin() + InsertIndex), Layer); Iter != Layers.end())
			{
				InsertIndex++;
				(*Iter)->OnAttach();
			}
		}

		FORCEINLINE void PushOverlay(TObjectPtr<LLayer> Overlay)
		{
			if (Layers.emplace_back(Overlay))
			{
				Overlay->OnAttach();
			}
		}

		FORCEINLINE void PopLayer(TObjectPtr<LLayer> Layer)
		{
			auto Iter = std::find(Layers.begin(), Layers.begin() + InsertIndex, Layer);
			if (Iter != (Layers.begin() + InsertIndex))
			{
				Layer->OnDetach();

				Layers.erase(Iter);
				InsertIndex--;
			}
		}

		FORCEINLINE void PopOverlay(TObjectPtr<LLayer> Overlay)
		{
			auto Iter = std::find(Layers.begin() + InsertIndex, Layers.end(), Overlay);
			if (Iter != Layers.end())
			{
				Overlay->OnDetach();

				Layers.erase(Iter);
			}
		}

		FORCEINLINE TObjectPtr<LLayer> operator[](const std::size_t LayerIndex)
		{
			LK_ASSERT((LayerIndex >= 0) && (LayerIndex < Layers.size()), "Invalid layer index: {}", LayerIndex);
			return Layers[LayerIndex];
		}

		FORCEINLINE TObjectPtr<LLayer> operator[](const std::size_t LayerIndex) const
		{
			LK_ASSERT((LayerIndex >= 0) && (LayerIndex < Layers.size()), "Invalid layer index: {}", LayerIndex);
			return Layers.at(LayerIndex);
		}

		/**
		 * @brief Get the count of layers.
		 */
		FORCEINLINE int Count() const
		{
			return static_cast<int>(Layers.size());
		}

		std::vector<TObjectPtr<LLayer>>::iterator begin() { return Layers.begin(); }
		std::vector<TObjectPtr<LLayer>>::iterator end() { return Layers.end(); }

	private:
		std::vector<TObjectPtr<LLayer>> Layers{};
		uint32_t InsertIndex = 0;

		LCLASS(LLayerStack);
	};


}
