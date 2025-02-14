#pragma once

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include <imgui-node-editor/imgui_node_editor.h>


namespace LkEngine {

    namespace NE = ax::NodeEditor;

    class LNodeEditorContext : public LObject
    {
    public:
        LNodeEditorContext();
        ~LNodeEditorContext();

        void SetAsCurrentEditorContext();

        FORCEINLINE NE::EditorContext* GetContext() { return EditorContext; }

        static void SetEditorContext(LNodeEditorContext* InContext);

    private:
        NE::EditorContext* EditorContext = nullptr;

		LCLASS(LNodeEditorContext);
    };

}
