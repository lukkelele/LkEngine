#include "LkEngine/Core/Application.h"

#include "RuntimeLayer.h"


/**
 * LRuntimeApplication
 * 
 *  Runtime application.
 */
class LRuntimeApplication : public LkEngine::LApplication
{
public:
    LRuntimeApplication(LkEngine::ApplicationSpecification Specification)
        : LkEngine::LApplication(Specification) 
		, ProjectPath(Specification.StartProject)
    {
    }

    ~LRuntimeApplication() = default;

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
    LkEngine::ApplicationSpecification ApplicationSpec = {
        .Title = "LkEditor",
        .Width = 1920,
        .Height = 1080,
    };

    ApplicationSpec.Argc = Argc;
    ApplicationSpec.Argv = Argv;

    LRuntimeApplication Application(ApplicationSpec);
    Application.Initialize();
    Application.Run();
    Application.Shutdown();

    return 0;
}