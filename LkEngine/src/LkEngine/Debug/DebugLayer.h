#pragma once

#include "LkEngine/Core/Layer.h"


namespace LkEngine {

    class DebugLayer : public Layer
    {
    public:
        DebugLayer();
        ~DebugLayer() = default;

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(float ts) override;
        void OnImGuiRender() override;

    };

}