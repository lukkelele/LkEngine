#include "LkEngine/Core/Application.h"

#include "RuntimeLayer.h"
#include "LkEngine/Core/Time/Time.h"


/**
 * LRuntimeApplication
 * 
 *  Runtime application.
 */
class LRuntimeApplication : public LkEngine::LApplication
{
public:
    LRuntimeApplication(LkEngine::FApplicationSpecification Specification)
        : LkEngine::LApplication(Specification) 
		, ProjectPath(Specification.StartProject)
    {
		LOBJECT_REGISTER();
    }

	~LRuntimeApplication()
	{
	}

	virtual void Initialize() override
	{
		LApplication::Initialize();

		PushLayer(LkEngine::TObjectPtr<LkEngine::LRuntimeLayer>::Create(ProjectPath));
	}

private:
	std::filesystem::path& ProjectPath;
};


int main(int Argc, char* Argv[])
{
	LkEngine::Core::Setup(Argc, Argv);

    LkEngine::FApplicationSpecification ApplicationSpec = {
        .Title = "LkEditor",
        .Width = 1920,
        .Height = 1080,
    };

    ApplicationSpec.Argc = Argc;
    ApplicationSpec.Argv = Argv;

    LkEngine::TObjectPtr<LRuntimeApplication> Application = LkEngine::TObjectPtr<LRuntimeApplication>::Create(ApplicationSpec);
    Application->Initialize();
    Application->Run();
    Application->Shutdown();

    return 0;
}
