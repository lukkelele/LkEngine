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
        static s_ptr<Input> Create(Application* app);

        static void OnUpdate();
        static void SetScene(Scene& scene) { m_Scene = &scene; }

    private:
        inline static Input* m_Instance = nullptr;
        inline static Scene* m_Scene = nullptr;
    };

}