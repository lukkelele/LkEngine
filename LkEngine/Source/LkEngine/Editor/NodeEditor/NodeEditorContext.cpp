#include "LKpch.h"
#include "NodeEditorContext.h"


namespace LkEngine {

    LNodeEditorContext::LNodeEditorContext()
    {
    }

    LNodeEditorContext::~LNodeEditorContext()
    {
    }

    void LNodeEditorContext::Initialize()
    {
        // FIXME:
        EditorContext = NE::CreateEditor();
        NE::SetCurrentEditor(EditorContext);
    }

    void LNodeEditorContext::Destroy()
    {
    }

    void LNodeEditorContext::SetAsCurrentEditorContext()
    {
        // FIXME:
        NE::SetCurrentEditor(EditorContext);
    }

    void LNodeEditorContext::SetEditorContext(LNodeEditorContext* InEditorContext)
    {
        // FIXME:
        NE::SetCurrentEditor(InEditorContext->GetContext());
    }

}
