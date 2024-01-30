#pragma once

#include "LkEngine/Renderer/Texture.h"

namespace LkEngine {

    struct MaterialSpecification
    {
        float Roughness = 0.50f;
        float Density = 1.0f;
        float Friction = 0.30f;

        MaterialSpecification() = default;
        MaterialSpecification(float roughness, float density, float friction) 
            : Roughness(roughness) 
            , Density(density)
            , Friction(friction)
        {}
    };

    // TODO: Will need to make separate implementation for each render API
    // e.g --> OpenGLMaterial 

    class Material : public RefCounted
    {
    public:
        Material(const MaterialSpecification& properties = MaterialSpecification());
        Material(const MaterialSpecification& properties, Ref<Texture> texture);
        Material(Ref<Texture> texture);
        ~Material() = default;

        void SetTexture(const Ref<Texture> texture);

        Ref<Texture> GetTexture() { return m_Texture; }
        std::string GetTextureName() { return m_Texture->GetName(); }

        void SetName(const std::string& name) { m_Name = name; }
        std::string GetName() const { return m_Name; }

        void SetRoughness(float roughness) { m_Properties.Roughness = roughness; }
        float GetRoughness() const { return m_Properties.Roughness; }

        const MaterialSpecification& GetSpecification() const { return m_Properties; }

        static Ref<Material> Create(const MaterialSpecification& properties = MaterialSpecification());

    private:
        std::string m_Name;
        MaterialSpecification m_Properties;
        Ref<Texture> m_Texture;
    };

}