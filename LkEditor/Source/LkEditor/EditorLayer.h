/**
 * @file 
 * @brief LkEditor
 */
#pragma once

#include <LkEngine/Core/LObject/Object.h>
#include <LkEngine/Core/LObject/ObjectPtr.h>
#include <LkEngine/Core/Thread.h>
#include <LkEngine/Core/Layer.h>
#include <LkEngine/Core/Delegate/Delegate.h>
#include <LkEngine/Core/Viewport.h>
#include <LkEngine/Core/SelectionContext.h>
#include <LkEngine/Core/Math/Math.h>
#include <LkEngine/Core/Math/Ray.h>
#include <LkEngine/Core/Event/EventQueue.h>

#include <LkEngine/Editor/EditorGlobals.h>
#include <LkEngine/Editor/EditorContext.h>
#include <LkEngine/Editor/EditorSettings.h>
#include <LkEngine/Editor/PanelManager.h>

#include <LkEngine/Scene/Entity.h>
#include <LkEngine/Scene/Components.h>
#include <LkEngine/Scene/SceneSelectionData.h>

#include <LkEngine/Renderer/Framebuffer.h>
#include <LkEngine/Renderer/UI/UICore.h>
#include <LkEngine/Renderer/UI/UILayer.h>

#include <LkEngine/Project/Project.h>

namespace LkEngine {

	class LWindow;
	class LNodeEditorPanel;
	class LScene;
	class LSceneManagerPanel;
	class LSubmesh;
	class LRenderer2D;
	class LViewport;

	/**
	 * @ingroup Editor
	 * @{
	 */

	/**
	 * @class LEditorLayer
	 *
	 * The LkEngine editor layer (LkEditor).
	 */
	class LEditorLayer : public LLayer
	{
	public:
		LEditorLayer();
		~LEditorLayer() = default;

		virtual void Initialize() override;

		virtual void Tick(const float InDeltaTime) override;
		virtual void RenderUI() override;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnEvent(LEvent& Event) override;

	private:
		void InitializePanelManager();

		void RenderViewport();
		void Render2D();
		void DrawObjectGizmo(LEntity Entity);

		void OpenProject();
		void OpenProject(const std::filesystem::path& ProjectPath);
		void CloseProject(const EProjectLoadAction LoadAction = EProjectLoadAction::Unload);
		void SaveProject();
		void CreateProject(const std::filesystem::path& ProjectPath);
		void OnProjectChanged(const TObjectPtr<LProject>& InProject);
		void EmptyProject();
		void StarterProject();

		bool OpenScene();
		bool OpenScene(const std::filesystem::path& Filepath, const bool CheckForAutosaves);
		void SetScene(TObjectPtr<LScene> InScene);
		void NewScene(const std::string& SceneName = "Untitled");
		void SaveScene();
		void SaveSceneAs();
		void AutoSaveScene();

	public:
		FORCEINLINE static LEditorLayer& Get() { return *Instance; }
		FORCEINLINE TObjectPtr<LScene> GetEditorScene() { return EditorScene; }
		FORCEINLINE LEditorCamera& GetEditorCamera() { return EditorCamera; }

		void OnViewportSizeUpdated(const uint16_t NewWidth, const uint16_t NewHeight);
		void UpdateWindowTitle(const std::string& SceneName);

		void OnKeyPressed(const FKeyData& KeyData);
		void OnKeyReleased(const FKeyData& KeyData);

		void OnMouseEnabled(const bool Enabled);
		void OnMouseButtonPressed(const FMouseButtonData& ButtonData);
		void OnMouseButtonReleased(const FMouseButtonData& ButtonData);
		void OnMouseScrolled(const EMouseScrollDirection ScrollDir);
		void OnMouseCursorModeChanged(const ECursorMode CursorMode);
		void OnEngineShutdown();

		void OnScenePlay();
		void OnSceneStop();

		void UI_PrepareTopBar();
		void UI_PrepareLeftSidebar() const;
		void UI_PrepareRightSidebar() const;
		void UI_PrepareEditorViewport();
		void UI_SyncEditorWindowSizes(const LVector2& InViewportSize);
		void UI_RenderExternalWindows();
		void UI_HandleDragAndDrop();

		void UI_MainMenuBar();
		void UI_ToolBar();
		void UI_AboutPopup();
		void UI_LoadAutoSavePopup();
		void UI_NewProjectPopup();
		void UI_NewScenePopup();

		void UI_ShowViewportAndWindowDetails();
		void UI_ClearColorModificationMenu();
		void UI_ViewportTexture();
		void UI_WindowStatistics();
	
		void UI_OpenGLExtensions();
		void UI_RenderSettingsWindow(); /* TODO: Re-evaluate, might move this to LToolsPanel. */
		void UI_ProjectInfo();
		void UI_Debug_EditorViewportInfo();

		/** 
		 * @brief Perform raycast on a scene. 
		 * @returns Number of entities hit by the ray.
		 */
		uint16_t RaycastScene(TObjectPtr<LScene>& TargetScene, std::vector<FSceneSelectionData>& SceneSelectionData);

		/**
		 * @brief Cast ray from an editor camera. 
		 */
		void CastRay(FRayCast& RayCast, const LEditorCamera& Camera, const float MousePosX, const float MousePosY);

		std::pair<float, float> GetMouseViewportSpace(const bool IsEditorViewport);

		void OnCameraActivityChanged(LCamera* CameraRef, const bool CameraActive);

		/**
		 * @brief Set window focus.
		 * Used after switching between mouse modes (Pan/Rotate/Zoom) or 
		 * any other activities that can modify the focus.
		 */
		void UI_SetWindowFocus(const ImGuiID WindowID);

		/** 
		 * @brief Cache the ID of the currently focused window. 
		 */
		void UI_CacheWindowFocus();

	public:
		/** Delegate for changes in the selection inside a scene. */
		FOnSceneSelectionUpdated OnSceneSelectionUpdated;
	private:
		FEditorSettings& EditorSettings;
		LEventQueue EventQueue;
		TObjectPtr<LPanelManager> PanelManager;

		TObjectPtr<LProject> Project{};
		TObjectPtr<LScene> EditorScene{};
		TObjectPtr<LScene> RuntimeScene{};
		TObjectPtr<LScene> CurrentScene{};
		TObjectPtr<LSceneRenderer> SceneRenderer{};
		std::string SceneFilePath{};

		TObjectPtr<LWindow> Window;
		TObjectPtr<LRenderer2D> Renderer2D;

		std::array<LVector2, 2> PrimaryViewportBounds;
		TObjectPtr<LViewport> EditorViewport;
		TObjectPtr<LSceneRenderer> ViewportRenderer{};
		LEditorCamera EditorCamera;

		int Gizmo = static_cast<int>(ImGuizmo::OPERATION::TRANSLATE);

		ESceneState SceneState = ESceneState::Edit;

		struct FAutoSaveData
		{
			std::string FilePath{};
			std::string FilePathAuto{};
		} AutoSaveData;

		inline static LEditorLayer* Instance = nullptr;
	};

	/** @} */

}
