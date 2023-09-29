#pragma once


namespace LkEngine {

    class Application
    {
    public:
        Application();
        ~Application();

        static Application* Get() { return m_Instance; }


    private:
        static Application* m_Instance;

    };

}