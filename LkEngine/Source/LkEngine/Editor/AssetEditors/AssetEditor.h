#pragma once

#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "LkEngine/Renderer/UI/Panel.h"


namespace LkEngine {

	class LScene;

	/**
	 * LAssetEditor
	 * 
	 *  Base class for asset editors.
	 */
	class LAssetEditor : public LObject
	{
	public:
		LAssetEditor(std::string_view EditorName);
		LAssetEditor() = delete;
		virtual ~LAssetEditor() = default;

		virtual void Tick(const float DeltaTime) {};
		void RenderUI();

		virtual void SetAsset(const TObjectPtr<LAsset>& Asset) = 0;
		virtual void SetSceneContext(const TObjectPtr<LScene>& InScene) {}

		void SetOpen(const bool IsOpen);
		void SetTitle(std::string_view NewTitle);

		FORCEINLINE bool IsOpen() const { return bIsOpen; }
		const std::string& GetID() const { return ID; }
		const std::string& GetTitle() const { return Title; }
		virtual bool HasAsset() const = 0;

		virtual void SerializeToYaml(YAML::Emitter& Out) const = 0;
		virtual void DeserializeFromYaml(const YAML::Node& Data) = 0;

	private:
		virtual void Render() = 0;
		virtual void OnOpen() = 0;
		virtual void OnClose() = 0;

	protected:
		std::string ID{};
		std::string Title{};
		bool bIsOpen = false;

	private:
		LCLASS(LAssetEditor);
	};

}
