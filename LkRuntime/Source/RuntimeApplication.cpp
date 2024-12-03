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
	LkEngine::Global::SetRuntimeArguments(Argc, Argv);

	using namespace LkEngine;
    LkEngine::FApplicationSpecification ApplicationSpec = {
        .Title = "LkEditor",
        .Width = 1920,
        .Height = 1080,
    };

    ApplicationSpec.Argc = Argc;
    ApplicationSpec.Argv = Argv;

	LkEngine::LLog& Logger = LkEngine::LLog::Get();
	Logger.Initialize();
	LK_CORE_INFO("Log Directory: {}", LogDirectory.string());

    LRuntimeApplication Application(ApplicationSpec);
    Application.Initialize();
    Application.Run();
    Application.Shutdown();

    return 0;
}
