#include "LKpch.h"
#include "MaterialLibrary.h"
#include "TextureLibrary.h"


namespace LkEngine {

    MaterialLibrary::MaterialLibrary()
    {
        m_Instance = this;
    }

    void MaterialLibrary::Init()
    {
        CreateBasicMaterials();
    }

    void MaterialLibrary::Add(s_ptr<Material>& material)
    {
        // TODO: Check if material exists
        m_Collection.emplace(material->GetName(), material);
    }

    s_ptr<Material> MaterialLibrary::GetMaterial(std::string_view materialName)
    {
        auto it = m_Collection.find(std::string(materialName));
        if (it != m_Collection.end())
            return it->second;
        return nullptr;
    }

    void MaterialLibrary::CreateBasicMaterials()
    {
        // Asphalt

        // Rubber
        MaterialProperties RubberProperties;
        RubberProperties.Density = 0.90f;
        RubberProperties.Friction = 1.0f;
        auto rubber = std::make_shared<Material>(RubberProperties);
        m_Collection.emplace("Rubber", rubber);
    }

}