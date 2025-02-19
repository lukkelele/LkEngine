#include <LkEngine/Core/Application.h>

#include "LkEditor/EditorLayer.h"


/**
 * LkEditor
 * 
 *  The editor for LkEngine.
 */
class LkEditor : public LkEngine::LApplication
{
public:
    LkEditor(LkEngine::FApplicationSpecification Specification)
        : LkEngine::LApplication(Specification) 
    {
		LOBJECT_REGISTER();
		LK_CORE_DEBUG_TAG("LkEditor", "Creating instance");
    }

	~LkEditor()
	{
	}

	virtual void Initialize() override
	{
		LK_CORE_INFO_TAG("LkEditor", "Initializing");
		LApplication::Initialize();
		LK_CORE_DEBUG_TAG("LkEditor", "Pushing editor layer");
		PushLayer(LkEngine::TObjectPtr<LkEngine::LEditorLayer>::Create());
	}
};


int main(int Argc, char* Argv[])
{
	LkEngine::Core::Setup(Argc, Argv);

    LkEngine::FApplicationSpecification ApplicationSpec = {
		.Argc = Argc,
		.Argv = Argv,
        .Title = "LkEditor",
    };

	LK_PRINTLN("Creating LkEditor");
    LkEngine::TObjectPtr<LkEditor> Editor = LkEngine::TObjectPtr<LkEditor>::Create(ApplicationSpec);
    Editor->Initialize();
    Editor->Run();
    Editor->Shutdown();

    return 0;
}
