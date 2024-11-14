#include "LKpch.h"
#include "MaterialLibrary.h"
#include "TextureLibrary.h"

#include "Renderer.h"

#include "LkEngine/Asset/AssetManager.h"


namespace LkEngine {

    LMaterialLibrary::LMaterialLibrary()
    {
		LCLASS_REGISTER();
    }

    LMaterialLibrary& LMaterialLibrary::Get()
    {
        static LMaterialLibrary MaterialLibrary;
        return MaterialLibrary;
    }

    void LMaterialLibrary::Initialize()
    {
		LK_CORE_DEBUG_TAG("MaterialLibrary", "Creating base material");
        TObjectPtr<LMaterial> BaseMaterial = LMaterial::Create(
			LRenderer::GetShaderLibrary()->Get("Renderer_Model"), /*  Shader.         */
            BASE_MATERIAL                                         /*  Material Name.  */
        );
        m_Collection.emplace(BaseMaterial->GetName(), BaseMaterial);

        LK_CORE_DEBUG_TAG("MaterialLibrary", "Creating basic materials");
        CreateBasicMaterials();
    }

    void LMaterialLibrary::Add(const TObjectPtr<LMaterial> Material)
    {
        /* Material does not exist in the collection. */
        //if (!m_Collection.contains(Material->GetHandle()))
        if (!m_Collection.contains(Material->GetName()))
        {
            m_Collection.emplace(Material->GetName(), Material);
        }
    }

    TObjectPtr<LMaterial> LMaterialLibrary::GetMaterial(std::string_view InMaterialName)
    {
        if (auto Iter = m_Collection.find(std::string(InMaterialName)); Iter != m_Collection.end())
        {
            return Iter->second;
        }

        LK_CORE_WARN_TAG("MaterialLibrary", "Failed to get material \"{}\"", InMaterialName.data());
        return nullptr;
    }

    void LMaterialLibrary::CreateBasicMaterials()
    {
        // Asphalt
        {
            TObjectPtr<LMaterial> asphalt = LMaterial::Create(
                LRenderer::GetShaderLibrary()->Get("Renderer_Model"), 
                "Basic_Asphalt"
            );
            TObjectPtr<LMaterialAsset> materialAsset(asphalt);
            m_Collection.emplace(asphalt->GetName(), asphalt);
            m_LoadedMaterialAssets.emplace(materialAsset->Handle, materialAsset);

            LK_CORE_INFO_TAG("MaterialLibrary", "Created material 'Basic_Asphalt'");
        }

        // Rubber
        {
            TObjectPtr<LMaterial> rubber = LMaterial::Create(
                LRenderer::GetShaderLibrary()->Get("Renderer_Model"), 
                "Basic_Rubber"
            );
            TObjectPtr<LMaterialAsset> materialAsset(rubber);
            m_Collection.emplace(rubber->GetName(), rubber);
            m_LoadedMaterialAssets.emplace(materialAsset->Handle, materialAsset);
            LK_CORE_INFO_TAG("MaterialLibrary", "Created material 'Basic_Rubber'");
        }
    }

    TObjectPtr<LMaterial> LMaterialLibrary::GetBaseMaterial()
    {
		LK_CORE_ASSERT(m_Collection.contains(BASE_MATERIAL));
        return m_Collection.find(BASE_MATERIAL)->second;
    }

}