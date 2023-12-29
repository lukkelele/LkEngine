#pragma once

#include "LkEngine/Renderer/Texture.h"

namespace LkEngine {

    struct MaterialProperties
    {
        float Roughness = 0.50f;
        float Density = 1.0f;

        MaterialProperties() = default;
        MaterialProperties(float roughness, float density) 
            : Roughness(roughness) 
            , Density(density)
        {}
    };

    class Material
    {
    public:
        Material(const MaterialProperties& properties = MaterialProperties());
        Material(const MaterialProperties& properties, s_ptr<Texture> texture);
        Material(s_ptr<Texture> texture);
        ~Material() = default;

        const MaterialProperties& GetSpecification() const { return m_Properties; }
        s_ptr<Texture> GetTexture() { return m_Texture; }
        void SetTexture(s_ptr<Texture> texture);
        float GetRoughness() const { return m_Properties.Roughness; }

    private:
        MaterialProperties m_Properties;
        s_ptr<Texture> m_Texture = nullptr;
    };

}