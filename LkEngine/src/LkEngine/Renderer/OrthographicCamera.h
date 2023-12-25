#pragma once
#if 0
#include "LkEngine/Renderer/Camera.h"


namespace LkEngine {


    class OrthographicCamera : public Camera
    {
    public:
        OrthographicCamera(float left, float right, float bottom, float top);
        ~OrthographicCamera() = default;

        void Update(float ts);
        void UpdateProjection();
        void UpdateView();
        void SetProjection(float left, float right, float bottom, float top);
        float GetRotation() const { return m_Rotation; }

    private:
        glm::vec3 m_Pos{}; // FIXME: REMOVE, JUST TO COMPILE
        float m_Rotation = 0.0f;
    };

}

#endif