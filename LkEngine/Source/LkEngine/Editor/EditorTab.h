#pragma once

#include "LkEngine/Core/Core.h"

#include "LkEngine/Renderer/Material.h"

#include "NodeEditor/NodeEditor.h"


namespace LkEngine {

    enum class EditorTabType
    {
        None = 0,
        Viewport,
        BlankEditor,
        NodeEditor,
        MaterialEditor,
        TextureEditor
    };

    class LTab
    {
    public:
        virtual ~LTab() = default;

        std::string Name = "";
        EditorTabType Type = EditorTabType::None;
        bool Closed = false;
        int Index;
        
        virtual void OnRender() = 0;
        virtual void OnImGuiRender() = 0;

        EditorTabType GetTabType() const { return Type; }

    };


    class ViewportTab : public LTab
    {
    public:
        ViewportTab(std::string_view name) 
        {
            Name = std::string(name);
            Type = EditorTabType::Viewport;
        }
        ViewportTab(std::string_view name, const EditorTabType tabType)
            : ViewportTab(name) {}
        ~ViewportTab() = default;

        void OnRender() override {}
        void OnImGuiRender() override {}
    };


    class NodeEditorTab : public LTab
    {
    public:
        NodeEditorTab(std::string_view name);
        ~NodeEditorTab();

        void OnRender() override;
        void OnImGuiRender() override;

    public:
        TObjectPtr<LNodeEditor> NodeEditor{};
    };

    /**
     * LMaterialEditorTab
     */
    class MaterialEditorTab: public LTab
    {
    public:
        MaterialEditorTab(std::string_view InName);
        ~MaterialEditorTab();

        void OnRender() override;
        void OnImGuiRender() override;

    public:
        TObjectPtr<LNodeEditor> NodeEditor{};
        TObjectPtr<LMaterial> MaterialRef = nullptr;
    };


}