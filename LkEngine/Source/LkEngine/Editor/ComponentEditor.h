#pragma once

#include "LkEngine/UI/Panel.h"

#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/Renderer/Material.h"


namespace LkEngine {

    class LComponentEditor : public IPanel
    {
    public:
        LComponentEditor();
        ~LComponentEditor() = default;

        virtual void Initialize() override;

        virtual void OnRender() override {}
        virtual void OnRenderUI() override;

    private:
        void RenderMaterialEditor(LMaterial& Material);
        void RenderTextureEditor(const TObjectPtr<LTexture>& Texture);
        void RenderTextureEditor(const TObjectPtr<LTexture2D>& Texture);

    private:
        LCLASS(LComponentEditor);
    };

}