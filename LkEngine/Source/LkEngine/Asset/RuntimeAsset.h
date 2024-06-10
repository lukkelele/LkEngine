#pragma once

#include "LkEngine/Asset/Asset.h"


namespace LkEngine {

    class RuntimeAsset : Asset
    {
    public:
        virtual void Setup() = 0;
        virtual void Destroy() = 0;
        virtual void OnUpdate(float timestep = 1.0f) {}
        virtual void OnImGuiRender() {}
    };

}