#pragma once

#include "LkEngine/Renderer/UI/UILayer.h"

#include "LkEngine/Core/Math/Vector.h"

/** 
 * @brief Define LVector2 conversion for ImVec2.
 */
#define IM_VEC2_CLASS_EXTRA \
	ImVec2(const ::LkEngine::LVector2& InVector) { x = InVector.X; y = InVector.Y; } \
	operator ::LkEngine::LVector2() const { return ::LkEngine::LVector2(x, y); }

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <ImGuizmo/ImGuizmo.h>


namespace LkEngine {

    class LOpenGLImGuiLayer : public LUILayer
    {
    public:
        LOpenGLImGuiLayer();
        ~LOpenGLImGuiLayer() = default;

        virtual void Initialize() override;
        virtual void Destroy() override;

        virtual void Tick(const float DeltaTime) {}
        virtual void RenderUI() override {};

        virtual void OnAttach() override {}
        virtual void OnDetach() override {}

        virtual void BeginFrame() override;
        virtual void EndFrame() override;
        
        virtual void SetDarkTheme() override;

	private:
		virtual void InitializeFonts() override;

    private:
		std::filesystem::path LayoutConfig{};
        LCLASS(LOpenGLImGuiLayer);
    };

}
