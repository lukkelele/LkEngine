#include "LkEngine/Core/Application.h"

#include "TestLayer.h"

#include "SuperMario/GameLayer.h"


constexpr uint32_t SCREEN_WIDTH = 1400;
constexpr uint32_t SCREEN_HEIGHT = 1024;

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
    TestLayer testLayer;

    GameLayer superMarioGameLayer;

    app.Init();

    app.PushLayer(&superMarioGameLayer);

    app.Run();
    app.Exit();

    return 0;
}