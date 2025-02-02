#pragma once

#include "LkEngine/Renderer/UI/Panel.h"
#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/Renderer/Material.h"


namespace LkEngine {

    class LComponentEditor : public IPanel
    {
    public:
        LComponentEditor();
        ~LComponentEditor() = default;

        virtual void Initialize() override;

        virtual void Render() override {}
        virtual void RenderUI(bool& IsOpen) override;

    private:
        void RenderMaterialEditor(LMaterial& Material);
        void RenderTextureEditor(const TObjectPtr<LTexture>& Texture);
        void RenderTextureEditor(const TObjectPtr<LTexture2D>& Texture);

    private:
        LCLASS(LComponentEditor);
    };

}
