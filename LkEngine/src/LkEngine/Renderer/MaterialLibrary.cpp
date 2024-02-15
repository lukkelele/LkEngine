#include "LKpch.h"
#include "MaterialLibrary.h"
#include "TextureLibrary.h"

#include "Renderer.h"

#include "LkEngine/Asset/AssetManager.h"


namespace LkEngine {

    MaterialLibrary::MaterialLibrary()
    {
        Init();
    }

    void MaterialLibrary::Init()
    {
        Ref<Material> BaseMaterial = Material::Create(Renderer::GetShaderLibrary()->Get("Renderer_Model"), "BaseMaterial");
        m_Collection.emplace(BaseMaterial->GetName(), BaseMaterial);

        LK_CORE_DEBUG_TAG("MaterialLibrary", "Creating basic materials");
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
        LK_CORE_VERIFY(it != m_Collection.end());
        if (it != m_Collection.end())
            return it->second;
        return nullptr;
    }

    void MaterialLibrary::CreateBasicMaterials()
    {
        // Asphalt
        {
            Ref<Material> asphalt = Material::Create(Renderer::GetShaderLibrary()->Get("Renderer_Model"), "Basic_Asphalt");
            Ref<MaterialAsset> materialAsset(asphalt);
            m_Collection.emplace(asphalt->GetName(), asphalt);
            m_LoadedMaterialAssets.emplace(materialAsset->Handle, materialAsset);
            LK_CORE_INFO_TAG("MaterialLibrary", "Created material 'Basic_Asphalt'");
        }

        // Rubber
        {
            Ref<Material> rubber = Material::Create(Renderer::GetShaderLibrary()->Get("Renderer_Model"), "Basic_Rubber");
            Ref<MaterialAsset> materialAsset(rubber);
            m_Collection.emplace(rubber->GetName(), rubber);
            m_LoadedMaterialAssets.emplace(materialAsset->Handle, materialAsset);
            LK_CORE_INFO_TAG("MaterialLibrary", "Created material 'Basic_Rubber'");
        }
    }

    Ref<Material> MaterialLibrary::GetBaseMaterial()
    {
        return m_Collection.find("BaseMaterial")->second;
    }

}