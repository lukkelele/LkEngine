#pragma once

#include "AssetEditor.h"
#include "LkEngine/Renderer/UI/Panel.h"

#include "LkEngine/Serialization/Serializer.h"


namespace LkEngine {

	class LScene;

	/**
	 * LAssetEditorManager
	 */
	class LAssetEditorManager
	{
	public:
		LAssetEditorManager();
		~LAssetEditorManager() = default;

		static void Tick(const float DeltaTime = 0.0f);

		static void RenderUI();
		static void OpenEditor(const TObjectPtr<LAsset>& Asset);

		static void SetSceneContext(const TObjectPtr<LScene>& InScene);
		static void SetEditorOpen(const EAssetType AssetType, const bool Open);

		template<typename T>
		static void RegisterEditor(const EAssetType AssetType)
		{
			static_assert(std::is_base_of_v<LAssetEditor, T>, "Asset editor has to derive from LAssetEditorPanel");
			LK_CORE_DEBUG_TAG("AssetEditorManager", "Registering editor for: {}", Enum::ToString(AssetType));
			LK_CORE_ASSERT(!AssetEditorMap.contains(AssetType));
			AssetEditorMap[AssetType] = std::make_unique<T>();
		}

		static void RegisterEditors();
		static void UnregisterEditors();

		static void Serialize();
		static void Deserialize();

	private:
		static std::unordered_map<EAssetType, std::unique_ptr<LAssetEditor>> AssetEditorMap;
		static TObjectPtr<LScene> SceneContext;

		friend class LEditorLayer;
	};

}
