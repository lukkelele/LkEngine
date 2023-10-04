#include "LkEngine/Core/Application.h"


class LkApplication : public LkEngine::Application
{
public:
    LkApplication(LkEngine::ApplicationProperties props = LkEngine::ApplicationProperties("Sandbox", 1920, 1080))
    {
    }

    ~LkApplication()
    {
    }

};


int main()
{
    printf("Running sandbox");
    const char* WindowTitle = "LkEngine Window";
    int WindowWidth = 1920;
    int WindowHeight = 1080;

    LkEngine::ApplicationProperties props(
        WindowTitle, 
        WindowWidth, 
        WindowHeight
    );
    auto app = std::make_shared<LkEngine::Application>(props);
    app->Init();
    GLFWwindow* glfwWindow = *app->GetGlfwWindow();
    ASSERT(glfwWindow);

    while (!glfwWindowShouldClose(glfwWindow))
    {
        app->OnUpdate();
    }
    app->Exit();

    return 0;
}