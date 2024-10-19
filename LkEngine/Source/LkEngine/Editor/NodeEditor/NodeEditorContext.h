#pragma once

#include <imgui-node-editor/imgui_node_editor.h>


namespace LkEngine {

    namespace NE = ax::NodeEditor;

    class LNodeEditorContext
    {
    public:
        LNodeEditorContext();
        ~LNodeEditorContext();

        void Initialize();
        void Destroy();

        void SetAsCurrentEditorContext();

        FORCEINLINE NE::EditorContext* GetContext() 
        { 
            return EditorContext; 
        }

        static void SetEditorContext(LNodeEditorContext* InContext);

    private:
        NE::EditorContext* EditorContext = nullptr;
    };

}