#pragma once

#include <imgui-node-editor/imgui_node_editor.h>

#include "NodeEditorContext.h"


namespace LkEngine {

    namespace NE = ax::NodeEditor; 

    class NodeEditor
    {
    public:
        NodeEditor(std::string_view name);
        ~NodeEditor();

        void OnRender();
        void OnImGuiRender();
        void Destroy();

    private:
        void Save();

    private:
        std::string m_Name;
        NodeEditorContext* m_NodeEditorContext = nullptr;
    };

}