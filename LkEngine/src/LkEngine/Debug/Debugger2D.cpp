#include "LKpch.h"
#include "Debugger2D.h"

#include "LkEngine/Scene/Scene.h"

#ifdef LK_PHYSICS_API_BOX2D
    #include "LkEngine/Debug/Box2DDebugger.h"
#endif


namespace LkEngine {

    s_ptr<Debugger2D> Debugger2D::Create()
    {
    #ifdef LK_PHYSICS_API_BOX2D
        return std::make_shared<Box2DDebugger>();
    #else
        return nullptr;
    #endif
    }

}