#include "NodeEditorContext.h"

namespace LkEngine {

    LNodeEditorContext::LNodeEditorContext()
    {
		LOBJECT_REGISTER();

		LK_CORE_DEBUG_TAG("NodeEditorContext", "Creating new context");
		NE::Config NodeConfig;
		//NodeConfig.SettingsFile = "LkNodeEditor.json";
		NodeConfig.SettingsFile = nullptr;
        EditorContext = NE::CreateEditor(&NodeConfig);
        NE::SetCurrentEditor(EditorContext);

		LK_CORE_WARN_TAG("NodeEditorContext", "Nodes not saved");
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
