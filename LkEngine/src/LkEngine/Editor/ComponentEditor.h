#pragma once

#include "LkEngine/UI/Panel.h"

#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/Renderer/Material.h"



namespace LkEngine {

    class ComponentEditor : public Panel
    {
    public:
        ComponentEditor();
        ~ComponentEditor();

        void Init() override;
        void OnRender() override;
        void OnImGuiRender() override;

    private:
        void RenderMaterialEditor(Material& material);
        void RenderTextureEditor(const Ref<Texture> texture);
        void RenderTextureEditor(const Ref<Texture2D> texture);

    private:
        friend class EditorLayer;
    };

}