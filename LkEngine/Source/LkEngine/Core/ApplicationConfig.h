#pragma once

#include <stdint.h>

#include "LkEngine/Core/String.h"


namespace LkEngine {

    struct FApplicationSpecification
    {
        int Argc = 0;
        char** Argv{};

		std::string Title = "LkEngine";
        uint16_t Width = 1650;
        uint16_t Height = 1080;
        bool Fullscreen = false;
        bool VSync = true;
        bool Resizable = true;
        bool StartMaximized = false;
        bool ImGuiEnabled = true;

        std::string WorkingDirectory{};
        std::filesystem::path ConfigurationFile = "LkEngine.config";
		std::filesystem::path StartProject{};

		//glm::vec4 ClearColor = { 0.30f, 0.30f, 0.30f, 1.0f };
    };

}
