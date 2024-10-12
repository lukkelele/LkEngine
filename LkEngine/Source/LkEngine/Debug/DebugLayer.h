#pragma once

#include "LkEngine/Core/Layer.h"
#include "LkEngine/Renderer/Color.h"

#include <glm/glm.hpp>


namespace LkEngine {

    class LScene;
    class LEntity;

    class DebugLayer : public LLayer
    {
    public:
        DebugLayer();
        ~DebugLayer() = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnUpdate(float ts) override;
        virtual void OnImGuiRender() override;

    public:
        inline static int s_DebugEntities = 0;
    };

}