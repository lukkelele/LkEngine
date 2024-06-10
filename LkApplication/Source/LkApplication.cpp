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


int main()
{
    LkEngine::ApplicationSpecification ApplicationSpec("LkEditor", 1920, 1080);

    LkApplication Application(ApplicationSpec);
    Application.Initialize();
    Application.Run();
    Application.Shutdown();

    return 0;
}