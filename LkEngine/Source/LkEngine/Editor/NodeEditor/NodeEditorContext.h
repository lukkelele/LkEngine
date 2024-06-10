#pragma once

#include <imgui-node-editor/imgui_node_editor.h>


namespace LkEngine {

    namespace NE = ax::NodeEditor;

    class NodeEditorContext
    {
    public:
        NodeEditorContext();
        ~NodeEditorContext();

        static void SetEditorContext(NodeEditorContext* context);

        void Init();
        void SetAsCurrentEditorContext();

        NE::EditorContext* GetContext() { return m_EditorContext; }

    private:
        NE::EditorContext* m_EditorContext = nullptr;

    };

}