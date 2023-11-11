#pragma once

#include "LkEngine/Core/Base.h"


namespace LkEngine {

    // Forward declaration
    class Application;
    class Scene;

    class Input
    {
    public:
        Input(Application* app);
        ~Input() = default;

        static Input* Get() { return s_Instance; }

        static void Init();
        static void HandleScene(Scene& scene);

    private:
        s_ptr<Application> m_App = nullptr;
        inline static Input* s_Instance = nullptr;
    };

}