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

        static Input* Get() { return m_Instance; }
        static s_ptr<Input> Create(Application* app) { return std::make_shared<Input>(app); }

        static void OnUpdate();
        static void HandleScene(Scene& scene);
        static void SetScene(Scene& scene) { m_Scene = &scene; }

    private:
        s_ptr<Application> m_App = nullptr;
        inline static Input* m_Instance = nullptr;
        inline static Scene* m_Scene = nullptr;
    };

}