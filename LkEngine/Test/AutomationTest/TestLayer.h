#pragma once

#include "LkEngine/LkEngine.h"


namespace LkEngine {

	class LTestLayer : public LLayer
	{
	public:
		LTestLayer(const std::filesystem::path& ProjectPath);
		LTestLayer() = delete;
		virtual ~LTestLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(const float Timestep) override;

	private:
		LCLASS(LTestLayer);
	};

}
