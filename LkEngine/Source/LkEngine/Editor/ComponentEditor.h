#pragma once

#include "LkEngine/UI/Panel.h"

#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/Renderer/Material.h"



namespace LkEngine {

    class LComponentEditor : public IPanel
    {
    public:
        LComponentEditor() = default;
        ~LComponentEditor() = default;

        void Init() override;
        void OnRender() override;
        void OnImGuiRender() override;

    private:
        void RenderMaterialEditor(LMaterial& Material);
        void RenderTextureEditor(const TObjectPtr<LTexture> texture);
        void RenderTextureEditor(const TObjectPtr<LTexture2D> texture);

    private:
        friend class LEditorLayer; /// REMOVE
    };

}