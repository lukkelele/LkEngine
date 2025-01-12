#include "LKpch.h"
#include "MaterialLibrary.h"
#include "TextureLibrary.h"

#include "Renderer.h"

#include "LkEngine/Asset/AssetManager.h"


namespace LkEngine {

	TObjectPtr<LMaterialAsset> LMaterialLibrary::BaseMaterial{};

    LMaterialLibrary::LMaterialLibrary()
    {
		LOBJECT_REGISTER();
    }

    LMaterialLibrary& LMaterialLibrary::Get()
    {
        static LMaterialLibrary MaterialLibrary;
        return MaterialLibrary;
    }

    void LMaterialLibrary::Initialize()
    {
		LK_CORE_DEBUG_TAG("MaterialLibrary", "Creating base material");
        TObjectPtr<LMaterial> BaseMat = LMaterial::Create(
			LRenderer::GetShaderLibrary()->Get("Renderer_Model"), /*  Shader.        */
            BASE_MATERIAL                                         /*  Material Name. */
        );

		TObjectPtr<LMaterialAsset> BaseMatAsset = Add(BaseMat);
		BaseMaterial = BaseMatAsset;

        CreateBasicMaterials();
    }

    TObjectPtr<LMaterialAsset> LMaterialLibrary::Add(const TObjectPtr<LMaterial> Material)
    {
		LK_VERIFY(Material, "Material is not valid");
		LK_CORE_TRACE_TAG("MaterialTable", "Adding {} to material library", Material->GetName());
		TObjectPtr<LMaterialAsset> MatAsset = TObjectPtr<LMaterialAsset>::Create(Material);
		m_LoadedMaterialAssets.emplace(MatAsset->Handle, MatAsset);
		m_Collection.emplace(Material->GetName(), MatAsset->Handle);

		return MatAsset;
    }

    TObjectPtr<LMaterialAsset> LMaterialLibrary::GetMaterial(const LUUID Handle)
	{
        if (auto Iter = m_LoadedMaterialAssets.find(Handle); Iter != m_LoadedMaterialAssets.end())
        {
            return Iter->second;
        }

        LK_CORE_ERROR_TAG("MaterialLibrary", "Failed to get material with handle \"{}\"", Handle);
        return nullptr;
	}

	TObjectPtr<LMaterialAsset> LMaterialLibrary::GetMaterial(std::string_view InMaterialName)
    {
        if (auto Iter = m_Collection.find(std::string(InMaterialName)); Iter != m_Collection.end())
        {
			return m_LoadedMaterialAssets.at(Iter->second);
        }

        return nullptr;
    }

    std::string LMaterialLibrary::DumpLoadedAssets()
	{
		std::string String;
		for (const auto& [MatAssetHandle, MatAsset] : m_LoadedMaterialAssets)
		{
			String += LK_FORMAT_STRING("Material Asset {}: {}\n", MatAssetHandle, MatAsset->GetMaterial()->GetName());
		}

		return String;
	}

	void LMaterialLibrary::CreateBasicMaterials()
    {
        /* Material: Asphalt. */
        {
            TObjectPtr<LMaterial> Asphalt = LMaterial::Create(
                LRenderer::GetShaderLibrary()->Get("Renderer_Model"), 
                "Basic_Asphalt"
            );
            TObjectPtr<LMaterialAsset> MaterialAsset(Asphalt);
            m_Collection.emplace(Asphalt->GetName(), Asphalt);
            m_LoadedMaterialAssets.emplace(MaterialAsset->Handle, MaterialAsset);
            LK_CORE_TRACE_TAG("MaterialLibrary", "Created material: 'Basic_Asphalt'");
        }

        /* Material: Rubber. */
        {
            TObjectPtr<LMaterial> Rubber = LMaterial::Create(
                LRenderer::GetShaderLibrary()->Get("Renderer_Model"), 
                "Basic_Rubber"
            );
            TObjectPtr<LMaterialAsset> MaterialAsset(Rubber);
            m_Collection.emplace(Rubber->GetName(), Rubber);
            m_LoadedMaterialAssets.emplace(MaterialAsset->Handle, MaterialAsset);
            LK_CORE_TRACE_TAG("MaterialLibrary", "Created material: 'Basic_Rubber'");
        }
    }

}
