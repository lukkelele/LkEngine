#pragma once

#include "Panel.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"


namespace LkEngine {

	class LSceneManagerPanel : public IPanel
	{
	public:
		LSceneManagerPanel();
		LSceneManagerPanel(const TObjectPtr<LScene>& InScene);
		~LSceneManagerPanel() = default;

		virtual void Initialize() override;

		virtual void OnRender() override;
		virtual void OnRenderUI() override;

		void DrawEntityNode(LEntity entity);
		static void DrawComponents(LEntity entity);

		/**
		 * DrawComponent
		 */
		template<typename T, typename UIFunction>
		static void DrawComponent(const std::string& name, 
								  LEntity entity, 
								  UIFunction uiFunction);

		template<typename T>
		void DisplayAddComponentEntry(const std::string& InComponentEntryName);

		void UI_SelectedEntityProperties();

		void SetScene(const TObjectPtr<LScene>& scene);
		std::pair<float, float> GetMouseViewportSpace(bool primaryViewport);

		void UI_CameraSettings();

	private:
		TObjectPtr<LScene> m_Scene{};
		TObjectPtr<LScene> m_ComponentCopyScene{};

		/// FIXME: REWORK THIS
		LEntity m_ComponentCopyEntity;
	};

}