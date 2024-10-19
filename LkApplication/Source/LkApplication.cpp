#include "LkEngine/Core/Application.h"


/**
 * LkApplication
 * 
 *  Runtime application.
 */
class LkApplication : public LkEngine::LApplication
{
public:
    LkApplication(LkEngine::ApplicationSpecification Specification)
        : LkEngine::LApplication(Specification) 
    {
    }

    ~LkApplication() = default;
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

    LkApplication Application(ApplicationSpec);
    Application.Initialize();
    Application.Run();
    Application.Shutdown();

    return 0;
}