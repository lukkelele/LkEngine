#include "LkEngine/Core/Application.h"

#include "SuperMario/GameLayer.h"


constexpr uint32_t SCREEN_WIDTH = 1200;
constexpr uint32_t SCREEN_HEIGHT = 800;

using namespace LkEngine;


class LkApplication : public LkEngine::Application
{
public:
    LkApplication(LkEngine::ApplicationSpecification props = LkEngine::ApplicationSpecification("Sandbox", SCREEN_WIDTH, SCREEN_HEIGHT))
        : LkEngine::Application(props)
    {
    }

    ~LkApplication() = default;
};


int main()
{
    LkApplication app;
    app.Init();

    GameLayer superMarioGameLayer;
    app.PushLayer(&superMarioGameLayer);

    app.Run();
    app.Exit();

    return 0;
}