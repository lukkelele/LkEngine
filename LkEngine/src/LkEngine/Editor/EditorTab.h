#pragma once

#include "LkEngine/Core/Base.h"

#include "NodeEditor/NodeEditor.h"


namespace LkEngine {

    enum class EditorTabType
    {
        Viewport,
        BlankEditor,
        NodeEditor,
    };

    struct Tab
    {
        std::string Name;
        EditorTabType Type;
        
        virtual void OnRender() {}
        virtual void OnImGuiRender() {}

        Tab() = default;
        Tab(std::string_view name, const EditorTabType type) : Name(std::string(name)) , Type(type) {}
        virtual ~Tab() = default;
    };

    struct NodeEditorTab : public Tab
    {
        NodeEditor* NodeEditorRef = nullptr;

        void OnRender() override {}

        void OnImGuiRender() override
        {
            NodeEditorRef->OnRender();
        }

        NodeEditorTab(std::string_view name) 
            : Tab(name, EditorTabType::NodeEditor) 
        {
            NodeEditorRef = new NodeEditor(name);
        }
        NodeEditorTab(std::string_view name, const EditorTabType tabType) : NodeEditorTab(name) {}
        ~NodeEditorTab()
        {
            NodeEditorRef->Destroy();
        }

    };

}