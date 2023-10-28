#include "LKpch.h"
#include "LkEngine/Input/Input.h"
#include "LkEngine/Core/Application.h"


namespace LkEngine {

    s_ptr<Input> Input::s_Instance = nullptr;

    Input::Input(Application* app)
        : m_App(create_s_ptr<Application>(*app))
        
    {
        s_Instance = std::shared_ptr<Input>(this);
        LK_ASSERT(m_App);
        LK_ASSERT(s_Instance);
    }

    void Input::Init()
    {
        LOG_DEBUG("Input initializing");
    }
    
    void Input::OnUpdate()
    {

    }

}