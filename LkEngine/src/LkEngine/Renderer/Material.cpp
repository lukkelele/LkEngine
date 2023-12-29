#include "LKpch.h"
#include "LkEngine/Renderer/Material.h"


namespace LkEngine {

    Material::Material(const MaterialProperties& properties)
        : m_Properties(properties)
    {
    }

    Material::Material(const MaterialProperties& properties, s_ptr<Texture> texture)
        : m_Properties(properties)
        , m_Texture(texture)
    {
    }

    Material::Material(s_ptr<Texture> texture)
        : m_Properties(MaterialProperties())
        , m_Texture(texture)
    {
    }

    void Material::SetTexture(s_ptr<Texture> texture)
    {
        m_Texture = texture;
        // ... do work
    }

}