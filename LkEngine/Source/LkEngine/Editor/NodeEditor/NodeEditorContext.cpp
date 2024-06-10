#include "LKpch.h"
#include "NodeEditorContext.h"


namespace LkEngine {

    NodeEditorContext::NodeEditorContext()
    {
    }

    NodeEditorContext::~NodeEditorContext()
    {
    }

    void NodeEditorContext::SetEditorContext(NodeEditorContext* context)
    {
        NE::SetCurrentEditor(context->GetContext());
    }

    void NodeEditorContext::Init()
    {
        m_EditorContext = NE::CreateEditor();
        NE::SetCurrentEditor(m_EditorContext);
    }

    void NodeEditorContext::SetAsCurrentEditorContext()
    {
        NE::SetCurrentEditor(m_EditorContext);
    }

}
