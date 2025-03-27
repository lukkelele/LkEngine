/**
 * @file
 * @brief Layer stack.
 */
#pragma once

#include "Layer.h"

namespace LkEngine {

	/**
	 * @class LLayerStack
	 *
	 * @details Data structure for layers.
	 *          There are two types of layers:
	 *          1) Layer (Regular)
	 *          2) Overlay
	 * 
	 *          A layer is inserted based on the number of layers present in the stack.
	 *          An overlay is always inserted on top of the entire stack.
	 * 
	 * @ingroup Core
	 */
	class LLayerStack : public LObject
	{
	public:
		LLayerStack() = default;
		~LLayerStack();

		/**
		 * @brief Destroy the layer stack, releasing all layers.
		 */
		void Destroy();

		/**
		 * @brief Push a layer to top of the layer stack.
		 */
		FORCEINLINE void PushLayer(LLayer* Layer)
		{
			if (auto Iter = Layers.emplace((Layers.begin() + InsertIndex), Layer); Iter != Layers.end())
			{
				InsertIndex++;
				(*Iter)->OnAttach();
			}
		}

		/**
		 * @brief Push a layer to top of the layer stack.
		 */
		FORCEINLINE void PushLayer(TObjectPtr<LLayer> Layer)
		{
			if (auto Iter = Layers.emplace((Layers.begin() + InsertIndex), Layer); Iter != Layers.end())
			{
				InsertIndex++;
				(*Iter)->OnAttach();
			}
		}

		/**
		 * @brief Push an overlay to top of the stack.
		 */
		FORCEINLINE void PushOverlay(TObjectPtr<LLayer> Overlay)
		{
			if (Layers.emplace_back(Overlay))
			{
				Overlay->OnAttach();
			}
		}

		/**
		 * @brief Pop a layer from the stack.
		 */
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

		/**
		 * @brief Pop an overlay from the stack.
		 */
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
		/**
		 * @var Layers
		 * Collection of layer pointers.
		 */
		std::vector<TObjectPtr<LLayer>> Layers{};

		/**
		 * @var InsertIndex
		 * Index used for knowing where to insert a layer.
		 */
		uint32_t InsertIndex = 0;

		LCLASS(LLayerStack);
	};


}
