#include "LKpch.h"
#include "NodeEditorContext.h"


namespace LkEngine {

    NodeEditorContext::NodeEditorContext()
    {
    }

    NodeEditorContext::~NodeEditorContext()
    {
    }

    void NodeEditorContext::Init()
    {
        m_EditorContext = NE::CreateEditor();
    }

}
