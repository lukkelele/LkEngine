#pragma once

#include "AssetEditor.h"

#include "LkEngine/Renderer/Material.h"


namespace LkEngine {

	class LMaterialEditor : public LAssetEditor
	{
	public:
		LMaterialEditor();
		~LMaterialEditor() = default;

		virtual void Tick(const float DeltaTime) override;

		virtual void SetAsset(const TObjectPtr<LAsset>& Asset) override
		{
			LK_CORE_DEBUG_TAG("MaterialEditor", "Set asset: {}", Asset->Handle);
		#if 0
			MaterialAsset = Asset.As<LMaterialAsset>();
		#else
			TObjectPtr<LMaterialAsset> MatAsset = Asset.As<LMaterialAsset>();
			MaterialAsset = MatAsset;
		#endif
			LK_CORE_VERIFY(MaterialAsset);
		}

		virtual void SerializeToYaml(YAML::Emitter& Out) const override;
		virtual void DeserializeFromYaml(const YAML::Node& EditorNode) override;

		virtual bool HasAsset() const override { return MaterialAsset != nullptr; }

	private:
		virtual void Render() override;
		virtual void OnOpen() override;
		virtual void OnClose() override;

	public:
		void UI_ShaderSelection();

	private:
		TObjectPtr<LMaterialAsset> MaterialAsset{};

		LCLASS(LMaterialEditor);
	};

}
