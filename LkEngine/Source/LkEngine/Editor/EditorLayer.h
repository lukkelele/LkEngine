/******************************************************************
 * EditorLayer
 *
 * Main editor.
 *******************************************************************/
#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "LkEngine/Core/Thread.h"
#include "LkEngine/Core/Layer.h"
#include "LkEngine/Core/Delegate/Delegate.h"
#include "LkEngine/Core/Viewport.h"
#include "LkEngine/Core/SelectionContext.h"
#include "LkEngine/Core/Math/Math.h"
#include "LkEngine/Core/Math/Ray.h"

#include "LkEngine/Editor/EditorGlobals.h"
#include "LkEngine/Editor/EditorSettings.h"
#include "LkEngine/Editor/PanelManager.h"

#include "LkEngine/Scene/Entity.h"
#include "LkEngine/Scene/Components.h"
#include "LkEngine/Scene/SceneSelectionData.h"

#include "LkEngine/Renderer/Framebuffer.h"
#include "LkEngine/Renderer/UI/UICore.h"
#include "LkEngine/Renderer/UI/UILayer.h"


namespace LkEngine {

	class LComponentEditor;
	class LEditorTabManager;
	class LNodeEditor;
	class LScene;
	class LSceneManagerPanel;
	class LSubmesh;
	class LRenderer2D;
	class LProject;
	class LViewport;

	/* FIXME: */
	enum class EEditorWindowType
	{
		None = 0,
		Viewport,
		NodeEditorLayer,
	};

	/**
	 * LEditorLayer
	 *
	 *  Editor layer.
	 */
	class LEditorLayer : public LLayer
	{
	public:
		LEditorLayer();
		~LEditorLayer() = default;

		virtual void Initialize() override;

		virtual void OnUpdate(const float InDeltaTime) override;
		virtual void OnRenderUI() override;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void RenderViewport();
		void Render2D();

		FORCEINLINE TObjectPtr<LScene> GetEditorScene() { return EditorScene; }
		FORCEINLINE TObjectPtr<LEditorCamera> GetEditorCamera() { return EditorCamera; }

		FORCEINLINE static LEditorLayer& Get() { return *Instance; }

		bool OpenScene();
		bool OpenScene(const std::filesystem::path& Filepath, const bool CheckForAutosaves);
		void SetScene(TObjectPtr<LScene> InScene);
		void NewScene(const std::string& SceneName = "Untitled");
		void EmptyProject();
		void StarterProject();
		void SaveProjectAs();

	private:
		void InitializePanelManager();

		void OnViewportSizeUpdated(const uint16_t NewWidth, const uint16_t NewHeight);
		void UpdateWindowTitle(const std::string& SceneName);

		void OnKeyPressed(const FKeyData& KeyData);
		void OnKeyReleased(const FKeyData& KeyData);

		void OnMouseEnabled(const bool Enabled);
		void OnMouseButtonPressed(const FMouseButtonData& ButtonData);
		void OnMouseButtonReleased(const FMouseButtonData& ButtonData);
		void OnMouseScrolled(const EMouseScrollDirection ScrollDir);
		void OnMouseCursorModeChanged(const ECursorMode CursorMode);

		void OnScenePlay();
		void OnSceneStop();

		void SaveScene();
		void SaveSceneAs();
		void AutoSaveScene();

		void UI_PrepareTopBar();
		void UI_PrepareLeftSidebar() const;
		void UI_PrepareRightSidebar() const;
		void UI_PrepareEditorViewport();

		void DrawObjectGizmo(LEntity Entity);
		void UI_RenderExternalWindows();

		void UI_MainMenuBar();
		void UI_ToolBar();
		void UI_AboutPopup();
		void UI_LoadAutoSavePopup();

		void UI_SyncEditorWindowSizes(const LVector2& InViewportSize);

		void UI_ShowViewportAndWindowDetails();
		void UI_ClearColorModificationMenu();
		void UI_ViewportTexture();
		void UI_WindowStatistics();
		void UI_TabManager();

		void UI_HandleDragAndDrop();
		
		void UI_OpenGLExtensions();
		void UI_RenderSettingsWindow(); /* TODO: Re-evaluate */

		/**
		 * Perform raycast on scene, returns the number of hit entities.
		 */
		uint16_t RaycastScene(TObjectPtr<LScene>& TargetScene, std::vector<FSceneSelectionData>& SceneSelectionData);

		void CastRay(FRayCast& RayCast, const LEditorCamera& Camera, const float MousePosX, const float MousePosY);

		TObjectPtr<LFramebuffer>& GetViewportFramebuffer() 
		{ 
			LK_CORE_ASSERT(ViewportFramebuffer);
			return ViewportFramebuffer; 
		}

		LEntity CreateCube(); /* TODO: REMOVE */

		std::pair<float, float> GetMouseViewportSpace(const bool IsEditorViewport);

		/**
		 * Set window focus.
		 * Used after switching between mouse modes (Pan/Rotate/Zoom) that can remove the focus.
		 */
		void UI_SetWindowFocus(const ImGuiID WindowID);

		/**
		 * Cache the ID of the currently focused window.
		 */
		void UI_CacheWindowFocus();

	public:
		/** Delegate for changes in the selection inside a scene. */
		FOnSceneSelectionUpdated OnSceneSelectionUpdated;

		glm::vec2 ViewportScalers = { 1.0f, 1.0f };
	private:
		FEditorSettings& EditorSettings;
		LEditorTabManager& TabManager;
		TObjectPtr<LPanelManager> PanelManager;

		TObjectPtr<LProject> Project{};
		TObjectPtr<LScene> EditorScene{};
		TObjectPtr<LScene> RuntimeScene{};
		TObjectPtr<LScene> CurrentScene{};
		TObjectPtr<LSceneRenderer> SceneRenderer{};
		std::string SceneFilePath{};

		TObjectPtr<LWindow> Window{};
		TObjectPtr<LRenderer2D> Renderer2D{};

		LVector2 PrimaryViewportBounds[2];
		TObjectPtr<LViewport> EditorViewport;
		TObjectPtr<LFramebuffer> ViewportFramebuffer;
		TObjectPtr<LSceneRenderer> ViewportRenderer{};
		TObjectPtr<LEditorCamera> EditorCamera; /* TODO: Remove pointer, just make raw member. */

		int Gizmo = static_cast<int>(ImGuizmo::OPERATION::TRANSLATE);

		FEventCallback EventCallback;

		ESceneState SceneState = ESceneState::Edit;

		struct FAutoSaveData
		{
			std::string FilePath{};
			std::string FilePathAuto{};
		} AutoSaveData;

		friend class LRenderer; /* Allow access to ViewportFramebuffer, temporarily. */

		inline static LEditorLayer* Instance = nullptr;
	};

}
