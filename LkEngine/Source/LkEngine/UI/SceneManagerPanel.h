#pragma once

#include "Panel.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"


namespace LkEngine {

	class LSceneManagerPanel : public IPanel
	{
	public:
		LSceneManagerPanel(TObjectPtr<LScene> InScene = nullptr);
		//LSceneManagerPanel() = delete; /* FIXME: EVALUATE */
		~LSceneManagerPanel() = default;

		virtual void Initialize() override;

		virtual void OnRender() override;
		virtual void OnRenderUI() override;

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
		TObjectPtr<LScene> Scene{};

		/* TODO: Figure out a replacement for this approach. */
		TObjectPtr<LScene> ComponentCopyScene{};

		/// FIXME: REWORK THIS
		LEntity ComponentCopyEntity;

		LCLASS(LSceneManagerPanel);
	};

}
