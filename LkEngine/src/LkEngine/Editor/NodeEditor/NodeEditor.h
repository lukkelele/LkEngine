#pragma once

#include <imgui-node-editor/imgui_node_editor.h>

#include "NodeEditorContext.h"


namespace LkEngine {

    namespace NE = ax::NodeEditor; // Node Editor

    class NodeEditor
    {
    public:
        NodeEditor();
        ~NodeEditor();

        void Init();
        void OnRender();

    private:
        bool m_Initialized = false;
        NodeEditorContext* m_NodeEditorContext = nullptr;
    };

}