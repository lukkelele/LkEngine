#include "LKpch.h"
#include "LkEngine/Renderer/Material.h"


namespace LkEngine {

	//-------------------------------------------------------------------------------
	// Material
	//-------------------------------------------------------------------------------
    Material::Material(const MaterialSpecification& properties)
        : m_Properties(properties)
    {
    }

    Material::Material(const MaterialSpecification& properties, Ref<Texture> texture)
        : m_Properties(properties)
        , m_Texture(texture)
    {
    }

    Material::Material(Ref<Texture> texture)
        : m_Properties(MaterialSpecification())
        , m_Texture(texture)
    {
    }

    void Material::SetTexture(Ref<Texture> texture)
    {
        m_Texture = texture;
        // ... do work
    }

    Ref<Material> Material::Create(const MaterialSpecification& properties)
    {
        return Ref<Material>::Create(properties);
    }



}