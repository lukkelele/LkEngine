#pragma once

#include "LkEngine/Core/Base.h"


namespace LkEngine {

    class Application;

    class Input
    {
    public:
        Input(Application* app);
        ~Input() = default;

        static s_ptr<Input> Get() { return s_Instance; }

        void Init();
        void OnUpdate();

    private:
        s_ptr<Application> m_App = nullptr;
        static s_ptr<Input> s_Instance;
    };

}