#pragma once

#include "LkEngine/UI/Panel.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"


namespace LkEngine {

	class LSceneManagerPanel : public IPanel
	{
	public:
		LSceneManagerPanel(TObjectPtr<LScene> InScene = nullptr);
		~LSceneManagerPanel() = default;

		virtual void Initialize() override;

		virtual void OnRender() override;
		virtual void OnRenderUI(bool& IsOpen) override;

		virtual void SerializeToYaml(YAML::Emitter& Out) const override;
		virtual void DeserializeFromYaml(const YAML::Node& Data) override;

		void DrawEntityNode(LEntity entity);
		static void DrawComponents(LEntity entity);

		/**
		 * DrawComponent
		 */
		template<typename T, typename TUIFunction>
		static void DrawComponent(const std::string& ComponentName, LEntity Entity, TUIFunction UIFunction);

		template<typename T>
		void DisplayAddComponentEntry(const std::string& InComponentEntryName);

		void UI_SelectedEntityProperties();

		void SetScene(const TObjectPtr<LScene>& scene);
		std::pair<float, float> GetMouseViewportSpace(bool primaryViewport);

		void UI_CameraSettings();

	private:
		bool bIsWindow = true;
		TObjectPtr<LScene> Scene{};

		TObjectPtr<LScene> ComponentCopyScene{};
		LEntity ComponentCopyEntity;

		LCLASS(LSceneManagerPanel);
	};

}
