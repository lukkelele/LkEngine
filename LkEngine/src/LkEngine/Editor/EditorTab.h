#pragma once

#include "LkEngine/Core/Base.h"

#include "NodeEditor/NodeEditor.h"


namespace LkEngine {

    enum class EditorTabType
    {
        None = 0,
        Viewport,
        BlankEditor,
        NodeEditor,
    };

    class Tab
    {
    public:
        std::string Name = "";
        EditorTabType Type = EditorTabType::None;
        
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
        NodeEditorTab(std::string_view name) 
        {
            Name = std::string(name);
            Type = EditorTabType::NodeEditor;
            NodeEditorRef = new NodeEditor(name);
        }
        NodeEditorTab(std::string_view name, const EditorTabType tabType) : NodeEditorTab(name) {}
        ~NodeEditorTab()
        {
            NodeEditorRef->Destroy();
        }

        void OnRender() override {}
        void OnImGuiRender() override
        {
            NodeEditorRef->OnRender();
        }

    public:
        NodeEditor* NodeEditorRef = nullptr;

    };

}