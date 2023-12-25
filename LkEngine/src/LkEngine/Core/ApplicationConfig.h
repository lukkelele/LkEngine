#pragma once

#include <stdint.h>
#include <string>


namespace LkEngine {

    struct ApplicationSpecification
    {
		std::string Title;
        uint16_t Width;
        uint16_t Height;
        bool Fullscreen = false;
        bool VSync = true;
		std::string WorkingDirectory;
        bool Resizable = true;
        bool StartMaximized = false;
        bool ImGuiEnabled = true;

        ApplicationSpecification(const std::string& title = "LkEngine",
                                 uint16_t width = 1600,
                                 uint16_t height = 1024,
                                 bool fullscreen = false,
                                 bool vsync = true,
                                 const std::string& workingDirectory = "",
                                 bool startMaximized = false,
                                 bool resizeable = true,
                                 bool imguiEnabled = true)
			: Title(title)
            , Width(width)
            , Height(height)
            , Fullscreen(fullscreen)
            , VSync(vsync)
            , WorkingDirectory(workingDirectory)
            , Resizable(resizeable)
            , StartMaximized(startMaximized)
            , ImGuiEnabled(imguiEnabled)
        {
        }
    };

}