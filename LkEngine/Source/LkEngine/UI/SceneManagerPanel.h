#pragma once

#include "Panel.h"

#include "LkEngine/Scene/Scene.h"
#include "LkEngine/Scene/Entity.h"


namespace LkEngine {

	class SceneManagerPanel : public Panel
	{
	public:
		SceneManagerPanel();
		SceneManagerPanel(const Ref<Scene>& scene);

		void Init();

		void OnRender() override;
		void OnImGuiRender() override;

		void DrawEntityNode(LEntity entity);
		static void DrawComponents(LEntity entity);

		template<typename T, typename UIFunction>
		static void DrawComponent(const std::string& name, LEntity entity, UIFunction uiFunction);

		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);

		void UI_SelectedEntityProperties();

		void SetScene(const Ref<Scene>& scene);
		std::pair<float, float> GetMouseViewportSpace(bool primaryViewport);

		void UI_CameraSettings();

	private:
		Ref<Scene> m_Scene;

		Ref<Scene> m_ComponentCopyScene;
		LEntity m_ComponentCopyEntity;
	};

}