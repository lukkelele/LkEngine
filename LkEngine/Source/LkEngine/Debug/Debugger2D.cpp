#include "LKpch.h"
#include "Debugger2D.h"

#include "LkEngine/Scene/Scene.h"

#ifdef LK_PHYSICS_API_BOX2D
    #include "LkEngine/Debug/Box2D/Box2DDebugger.h"
#endif


namespace LkEngine {

    TSharedPtr<Debugger2D> Debugger2D::Create()
    {
    #ifdef LK_PHYSICS_API_BOX2D
		#if 0 /// DISABLED
        return std::make_shared<Box2DDebugger>();
		#endif
        return nullptr;
    #else
        return nullptr;
    #endif
    }

}