#pragma once

#include <stdint.h>

#include "LkEngine/Core/String.h"


namespace LkEngine {

    /// @FIXME: DONT SET VERSION HERE
    struct ApplicationSpecification
    {
        int Argc = 0;
        char** Argv{};

		LString Title = "LkEngine v0.2";
        uint16_t Width = 1650;
        uint16_t Height = 1080;
        bool Fullscreen = false;
        bool VSync = true;
        bool Resizable = true;
        bool StartMaximized = false;
        bool ImGuiEnabled = true;

        LString WorkingDirectory{};
        std::filesystem::path ConfigurationFile = "LkEngine.config";
		std::filesystem::path StartProject{};
    };

}
