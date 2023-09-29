#include <iostream>

#include "LkEngine/Core/Window.h"


int main()
{
    std::cout << "Hello world" << std::endl;

    int WindowWidth = 1920;
    int WindowHeight = 1080;
    const char* WindowTitle = "LkEngine Window";

    auto& window = LkEngine::Window::Create(WindowTitle, WindowWidth, WindowHeight);
    window->Init("#version 330");
    GLFWwindow* glfwWindow = window->GetGLFWWindow();
    ASSERT(glfwWindow);
    
    while (!glfwWindowShouldClose(glfwWindow))
    {
        window->BeginFrame();


        window->EndFrame();
    }



    return 0;
}