#include "LKpch.h"
#include "MaterialLibrary.h"
#include "TextureLibrary.h"


namespace LkEngine {

    MaterialLibrary::MaterialLibrary()
    {
        m_Instance = Ref<MaterialLibrary>(this);
    }

    void MaterialLibrary::Init()
    {
        CreateBasicMaterials();
    }

    void MaterialLibrary::Add(const Ref<Material> material)
    {
        // TODO: Check if material exists
        m_Collection.emplace(material->GetName(), material);
    }

    Ref<Material> MaterialLibrary::GetMaterial(std::string_view materialName)
    {
        auto it = m_Collection.find(std::string(materialName));
        if (it != m_Collection.end())
            return it->second;
        return nullptr;
    }

    void MaterialLibrary::CreateBasicMaterials()
    {
        // Asphalt
        MaterialSpecification asphaltSpec;
        asphaltSpec.Roughness = 1.0f;
        asphaltSpec.Friction = 1.0f;
        asphaltSpec.Density = 1.0f;
        auto asphalt = Ref<Material>::Create(asphaltSpec);
        asphalt->SetName("BasicMaterial_Asphalt");
        m_Collection.emplace(asphalt->GetName(), asphalt);

        // Rubber
        MaterialSpecification rubberSpec;
        rubberSpec.Density = 0.90f;
        rubberSpec.Friction = 1.0f;
        auto rubber = Ref<Material>::Create(rubberSpec);
        rubber->SetName("BasicMaterial_Rubber");
        m_Collection.emplace(rubber->GetName(), rubber);
    }

}