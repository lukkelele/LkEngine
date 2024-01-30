#pragma once

#include "LkEngine/Core/Base.h"

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

    class Tab
    {
    public:
        std::string Name = "";
        EditorTabType Type = EditorTabType::None;
        bool Closed = false;
        int Index;
        
        virtual void OnRender() = 0;
        virtual void OnImGuiRender() = 0;

        EditorTabType GetTabType() const { return Type; }

        virtual ~Tab() = default;
    };


    class ViewportTab : public Tab
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


    class NodeEditorTab : public Tab
    {
    public:
        NodeEditorTab(std::string_view name);
        NodeEditorTab(std::string_view name, const EditorTabType tabType);
        ~NodeEditorTab();

        void OnRender() override;
        void OnImGuiRender() override;

    public:
        NodeEditor* NodeEditorRef = nullptr;
    };


    class MaterialEditorTab: public Tab
    {
    public:
        MaterialEditorTab(std::string_view name);
        MaterialEditorTab(std::string_view name, const EditorTabType tabType);
        ~MaterialEditorTab();

        void OnRender() override;
        void OnImGuiRender() override;

    public:
        NodeEditor* NodeEditorRef = nullptr;
        Ref<Material> MaterialRef = nullptr;
    };


}