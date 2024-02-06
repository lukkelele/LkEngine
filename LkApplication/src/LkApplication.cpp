#include "LkEngine/Core/Application.h"

//#include "LkEngine/Platform/OpenGL/LkOpenGL.h"
//#include "LkEngine/Platform/Vulkan/LkVulkan.h"

#include "SuperMario/GameLayer.h"


constexpr uint32_t SCREEN_WIDTH = 1920;
constexpr uint32_t SCREEN_HEIGHT = 1080;

using namespace LkEngine;


class LkApplication : public LkEngine::Application
{
public:
    LkApplication(LkEngine::ApplicationSpecification props = LkEngine::ApplicationSpecification("Sandbox", SCREEN_WIDTH, SCREEN_HEIGHT))
        : LkEngine::Application(props) 
    {
    }

    ~LkApplication()
    {
    }
};


int main()
{
    LkApplication app;
    app.Init();

    GameLayer superMarioGameLayer;
    app.PushLayer(&superMarioGameLayer);

    app.Run();
    app.Shutdown();

    return 0;
}