#pragma once

#include <stdint.h>
#include <string>


namespace LkEngine {

    struct ApplicationSpecification
    {
		std::string Title = "LkEngine";
        uint16_t Width = 1650;
        uint16_t Height = 1080;
        bool Fullscreen = false;
        bool VSync = true;
        bool Resizable = true;
        bool StartMaximized = false;
        bool ImGuiEnabled = true;

		std::string WorkingDirectory;

        ApplicationSpecification() = default;

        ApplicationSpecification(const std::string& title, uint16_t width, uint16_t height,
                                 bool fullscreen = true, bool vsync = true)
            : Title(title)
            , Width(width)
            , Height(height)
            , Fullscreen(fullscreen)
            , VSync(vsync)
        {
        }

        ApplicationSpecification(const std::string& title, uint16_t width, uint16_t height,
                                 bool fullscreen, bool vsync, const std::string& workingDirectory,
                                 bool startMaximized, bool resizeable, bool imguiEnabled)
			: Title(title) , Width(width) , Height(height) , Fullscreen(fullscreen)
            , VSync(vsync) , WorkingDirectory(workingDirectory) , Resizable(resizeable)
            , StartMaximized(startMaximized) , ImGuiEnabled(imguiEnabled) 
        {
        }

    };

}