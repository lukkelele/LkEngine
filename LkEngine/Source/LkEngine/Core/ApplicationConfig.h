#pragma once

#include <stdint.h>

#include "LkEngine/Core/String.h"


namespace LkEngine {

    struct ApplicationSpecification
    {
		//std::string Title = "LkEngine v2.0.1";
        /// @TODO: Get version
		LString Title = "LkEngine v2.0.1";
        uint16_t Width = 1650;
        uint16_t Height = 1080;
        bool Fullscreen = false;
        bool VSync = true;
        bool Resizable = true;
        bool StartMaximized = false;
        bool ImGuiEnabled = true;

        //std::string WorkingDirectory{};
        LString WorkingDirectory{};

        ApplicationSpecification() = default;

        ApplicationSpecification(const LString& InTitle, uint16_t InWidth, uint16_t InHeight,
                                 bool InFullscreen = true, bool InVSync= true)
            : Title(InTitle)
            , Width(InWidth)
            , Height(InHeight)
            , Fullscreen(InFullscreen)
            , VSync(InVSync)
        {
        }

        ApplicationSpecification(const std::string& InTitle, 
                                 const uint16_t InWidth, const uint16_t InHeight,
                                 const bool InFullscreen, 
                                 const bool InVSync, 
                                 const std::string& InWorkingDirectory,
                                 const bool InStartMaximized, 
                                 const bool InResizeable, 
                                 const bool InImguiEnabled)
			: Title(InTitle)
            , Width(InWidth)
            , Height(InHeight)
            , Fullscreen(InFullscreen)
            , VSync(InVSync)
            , WorkingDirectory(InWorkingDirectory)
            , Resizable(InResizeable)
            , StartMaximized(InStartMaximized)
            , ImGuiEnabled(InImguiEnabled) 
        {
        }

    };

}