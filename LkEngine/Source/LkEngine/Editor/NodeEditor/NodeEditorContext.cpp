#include "LKpch.h"
#include "NodeEditorContext.h"


namespace LkEngine {

    LNodeEditorContext::LNodeEditorContext()
    {
		LOBJECT_REGISTER();

		LK_CORE_INFO_TAG("NodeEditorContext", "Creating new context");
        EditorContext = NE::CreateEditor();
        NE::SetCurrentEditor(EditorContext);
    }

    LNodeEditorContext::~LNodeEditorContext()
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
