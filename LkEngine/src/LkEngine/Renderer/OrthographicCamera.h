#pragma once
#include "LkEngine/Renderer/Camera.h"


namespace LkEngine {


    class OrthographicCamera : public Camera
    {
    public:
        OrthographicCamera(float left, float right, float bottom, float top);
        ~OrthographicCamera() = default;

        void OnUpdate(float ts);
        void UpdateProjection();
        void UpdateView();
        void SetProjection(float left, float right, float bottom, float top);
        float GetRotation() const { return m_Rotation; }

    private:
        float m_Rotation = 0.0f;

    };

}