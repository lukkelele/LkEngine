#pragma once

#include "LkEngine/LkEngine.h"


namespace LkEngine {

	class LRuntimeLayer : public LLayer
	{
	public:
		LRuntimeLayer(const std::filesystem::path& ProjectPath);
		LRuntimeLayer() = delete;
		virtual ~LRuntimeLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void Tick(const float Timestep) override;

		void OpenProject();

		void OpenScene(const std::string& InFilepath);
		void LoadScene(const LUUID SceneHandle);

	private:
		void OnScenePlay();
		void OnSceneStop();

		void LoadSceneAssets();

	private:
		LCLASS(LRuntimeLayer);
	};

}
