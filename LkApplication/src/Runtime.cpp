//#ifndef IMGUI_DEFINE_MATH_OPERATORS
//#define IMGUI_DEFINE_MATH_OPERATORS
//#endif
#include "LkEngine/Application.h"
#include "TestLayer.h"

using namespace LkEngine;

class LkApplication : public LkEngine::Application
{
public:
    LkApplication(LkEngine::ApplicationProperties props = LkEngine::ApplicationProperties("Sandbox", 1920, 1080))
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