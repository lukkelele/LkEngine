//#ifndef IMGUI_DEFINE_MATH_OPERATORS
//#define IMGUI_DEFINE_MATH_OPERATORS
//#endif
#include "LkEngine/Core/Application.h"
#include "TestLayer.h"

#define SCREEN_WIDTH    1920
#define SCREEN_HEIGHT   1080

using namespace LkEngine;


class LkApplication : public LkEngine::Application
{
public:
    LkApplication(LkEngine::ApplicationProperties props = LkEngine::ApplicationProperties("Sandbox", SCREEN_WIDTH, SCREEN_HEIGHT))
        : LkEngine::Application(props)
    {
    }

    ~LkApplication() = default;
};


int main()
{
    LkApplication app;
    TestLayer testLayer;

    app.Init();
    app.PushLayer(&testLayer);
    app.Run();
    app.Exit();

    return 0;
}