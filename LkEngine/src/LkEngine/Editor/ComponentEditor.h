#pragma once

#include "LkEngine/UI/UICore.h"

#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/Renderer/Material.h"


namespace LkEngine {

    class ComponentEditor
    {
    public:
        ComponentEditor();
        ~ComponentEditor();

        void OnImGuiRender();

    private:
        void RenderMaterialEditor(Material& material);
        void RenderTextureEditor(Texture& texture);

    private:
        friend class Editor;
    };

}