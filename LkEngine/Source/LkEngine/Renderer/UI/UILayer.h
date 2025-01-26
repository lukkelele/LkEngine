#pragma once

#include "LkEngine/Core/Core.h"
#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"
#include "LkEngine/Core/Layer.h"

#include "LkEngine/Renderer/UI/ImGui/LImGui.h"

#include "LkEngine/Editor/EditorCore.h"


namespace LkEngine {

    class LWindow;

    enum class EUserInterfaceAPI : uint8_t
    {
        Unknown = 0,
        ImGui
    };

    /**
     * LUILayer
     * 
     *  UI Layer.
     */
    class LUILayer : public LLayer
    {
    public:
        virtual ~LUILayer() = default;

        virtual void Initialize() = 0;
        virtual void Destroy() = 0;

        virtual void OnAttach() = 0;
        virtual void OnDetach() = 0;

        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;

        virtual void SetDarkTheme() = 0; /* Remove */

        static TObjectPtr<LUILayer> Create();

	protected:
		virtual void InitializeFonts() = 0;

    private:
        LCLASS(LUILayer);
    };


}
