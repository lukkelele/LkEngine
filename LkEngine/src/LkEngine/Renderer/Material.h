#pragma once

#include "LkEngine/Renderer/Texture.h"

namespace LkEngine {

    struct MaterialProperties
    {
        float Roughness = 0.50f;
        float Density = 1.0f;
        float Friction = 0.30f;

        MaterialProperties() = default;
        MaterialProperties(float roughness, float density, float friction) 
            : Roughness(roughness) 
            , Density(density)
            , Friction(friction)
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

        std::string GetName() const { return m_Name; }
        void SetName(const std::string& name) { m_Name = name; }
        float GetRoughness() const { return m_Properties.Roughness; }
        float& GetRoughness() { return m_Properties.Roughness; }

    private:
        std::string m_Name;
        MaterialProperties m_Properties;
        s_ptr<Texture> m_Texture = nullptr;
    };

}