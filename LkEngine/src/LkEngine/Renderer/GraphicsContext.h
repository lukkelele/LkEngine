#pragma once

#include <memory>
#include <string>

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>


namespace LkEngine {

    // Forward declaration
    class Window;

    enum class SourceBlendFunction
    {
        Zero = 0,
        One,
        Color,
        Alpha,
        One_Minus_DestinationAlpha
    };

    enum class DestinationBlendFunction 
    {
        Zero = 0,
        One,
        Alpha,
        Color,
        One_Minus_SourceAlpha,
    };

    struct BlendFunction
    {
        SourceBlendFunction Source;
        DestinationBlendFunction Destination;
        BlendFunction() 
            : Source(SourceBlendFunction::Alpha), Destination(DestinationBlendFunction::One_Minus_SourceAlpha) {}
        BlendFunction(const SourceBlendFunction& source, const DestinationBlendFunction& destination)
            : Source(source), Destination(destination) {}
    };

    class GraphicsContext
    {
    public:
        enum class Profile { Core = 0, Compability = 1 };
    public:
        virtual ~GraphicsContext() = default;

        static GraphicsContext* Get() { return m_Instance; }
        static s_ptr<GraphicsContext> Create(Window* window, const std::string& shaderVersion);
        static void SetProfile(const Profile& profile);
        static void SetVersion(int majorVersion, int minorVersion);

        virtual void Init(const SourceBlendFunction& srcFunc, const DestinationBlendFunction& dstFunc) = 0;
        virtual void Destroy() = 0;
        virtual GLFWwindow* GetGlfwWindow() = 0;
        virtual void BeginImGuiFrame() = 0;
        virtual void EndImGuiFrame() = 0;
        virtual void InitImGui(const std::string& glslVersion) = 0;
        virtual void SetDepthEnabled(bool enabled) = 0;
        virtual void SetBlendFunction(const SourceBlendFunction& srcFunc, const DestinationBlendFunction& dstFunc) = 0;
        virtual void SetSourceBlendFunction(const SourceBlendFunction& srcFunc) = 0;
        virtual void SetDestinationBlendFunction(const DestinationBlendFunction& dstFunc) = 0;
        virtual void UpdateResolution(uint16_t width, uint16_t height) = 0;
        virtual std::string GetSourceBlendFunctionName() = 0;
        virtual std::string GetDestinationBlendFunctionName() = 0;
        virtual std::string GetSourceBlendFunctionName(const SourceBlendFunction& srcFunc) = 0;
        virtual std::string GetDestinationBlendFunctionName(const DestinationBlendFunction& dstFunc) = 0;
        virtual void SetDarkTheme();

    protected:
        static void HandleViewportEvents();
    
    protected:
        inline static GraphicsContext* m_Instance = nullptr;

    };

}
