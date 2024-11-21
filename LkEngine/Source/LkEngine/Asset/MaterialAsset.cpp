#include "LKpch.h"
#include "MaterialAsset.h"


namespace LkEngine {

	namespace {
		/* clang-format off */
		const std::string s_AlbedoColorUniform	= "u_MaterialUniforms.AlbedoColor";
		const std::string s_UseNormalMapUniform = "u_MaterialUniforms.UseNormalMap";
		const std::string s_MetalnessUniform	= "u_MaterialUniforms.Metalness";
		const std::string s_RoughnessUniform	= "u_MaterialUniforms.Roughness";
		const std::string s_EmissionUniform		= "u_MaterialUniforms.Emission";
		const std::string s_TransparencyUniform = "u_MaterialUniforms.Transparency";
		
		const std::string s_AlbedoMapUniform	= "u_AlbedoTexture";
		const std::string s_NormalMapUniform	= "u_NormalTexture";
		const std::string s_MetalnessMapUniform = "u_MetalnessTexture";
		const std::string s_RoughnessMapUniform = "u_RoughnessTexture";
		/* clang-format on */
	}

	LMaterialAsset::LMaterialAsset(const TObjectPtr<LMaterial>& InMaterial)
	{
		LASSET_REGISTER();

		Handle = {};
		Material = LMaterial::Copy(InMaterial);
		LK_CORE_INFO_TAG("MaterialAsset", "Created material asset ({}) with material {}", Handle, InMaterial->GetName());

		LK_CORE_ASSERT(Material->GetTexture(""), "Material ({}) texture is nullptr!", Material->GetName());
	}

	LMaterialAsset::LMaterialAsset(const bool IsTransparent)
		: m_Transparent(IsTransparent)
	{
		LASSET_REGISTER();

		Handle = {};
		SetDefaults();
		LK_CORE_INFO_TAG("MaterialAsset", "Created material asset with default settings");
	}

	glm::vec3& LMaterialAsset::GetAlbedoColor()
	{
		return Material->GetVector3(s_AlbedoColorUniform);
	}

	void LMaterialAsset::SetAlbedoColor(const glm::vec3& color)
	{
		Material->Set(s_AlbedoColorUniform, color);
	}

	float& LMaterialAsset::GetMetalness()
	{
		return Material->GetFloat(s_MetalnessUniform);
	}

	void LMaterialAsset::SetMetalness(float value)
	{
		LK_CORE_VERIFY(Material, "Material is nullptr!");
		Material->Set(s_MetalnessUniform, value);
	}

	float& LMaterialAsset::GetRoughness()
	{
		LK_CORE_VERIFY(Material, "Material is nullptr!");
		return Material->GetFloat(s_RoughnessUniform);
	}

	void LMaterialAsset::SetRoughness(float value)
	{
		LK_CORE_VERIFY(Material, "Material is nullptr!");
		Material->Set(s_RoughnessUniform, value);
	}

	float& LMaterialAsset::GetEmission()
	{
		LK_CORE_VERIFY(Material, "Material is nullptr!");
		return Material->GetFloat(s_EmissionUniform);
	}

	void LMaterialAsset::SetEmission(float value)
	{
		Material->Set(s_EmissionUniform, value);
	}

	float& LMaterialAsset::GetTransparency()
	{
		return Material->GetFloat(s_TransparencyUniform);
	}

	void LMaterialAsset::SetTransparency(float transparency)
	{
		Material->Set(s_TransparencyUniform, transparency);
	}

	TObjectPtr<LTexture2D> LMaterialAsset::GetAlbedoMap()
	{
		return Material->TryGetTexture(s_AlbedoMapUniform);
	}

	void LMaterialAsset::SetAlbedoMap(TObjectPtr<LTexture2D> texture)
	{
		Material->Set(s_AlbedoMapUniform, texture);
	}

	void LMaterialAsset::ClearAlbedoMap()
	{
		//Material->Set(s_AlbedoMapUniform, Renderer::GetWhiteTexture());
	}

	TObjectPtr<LTexture2D> LMaterialAsset::GetNormalMap()
	{
		return Material->TryGetTexture(s_NormalMapUniform);
	}

	void LMaterialAsset::SetNormalMap(TObjectPtr<LTexture2D> texture)
	{
		Material->Set(s_NormalMapUniform, texture);
	}

	TObjectPtr<LTexture2D> LMaterialAsset::GetMetalnessMap()
	{
		return Material->TryGetTexture(s_MetalnessMapUniform);
	}

	void LMaterialAsset::SetMetalnessMap(TObjectPtr<LTexture2D> texture)
	{
		Material->Set(s_MetalnessMapUniform, texture);
	}

	void LMaterialAsset::ClearMetalnessMap()
	{
		//Material->Set(s_MetalnessMapUniform, Renderer::GetWhiteTexture());
	}

	TObjectPtr<LTexture2D> LMaterialAsset::GetRoughnessMap()
	{
		return Material->TryGetTexture(s_RoughnessMapUniform);
	}

	void LMaterialAsset::SetRoughnessMap(TObjectPtr<LTexture2D> texture)
	{
		Material->Set(s_RoughnessMapUniform, texture);
	}

	void LMaterialAsset::ClearRoughnessMap()
	{
		//Material->Set(s_RoughnessMapUniform, Renderer::GetWhiteTexture());
	}

	void LMaterialAsset::SetDefaults()
	{
		if (m_Transparent)
		{
			/* Set defaults. */
			SetAlbedoColor(glm::vec3(0.8f));

			/* Maps. */
			//SetAlbedoMap(Renderer::GetWhiteTexture());
		}
		else
		{
			/* Set defaults. */
			SetAlbedoColor(glm::vec3(0.8f));
			SetEmission(0.0f);
			//SetUseNormalMap(false);
			SetMetalness(0.0f);
			SetRoughness(0.4f);

			/* Maps. */
		#if 0
			SetAlbedoMap(Renderer::GetWhiteTexture());
			SetNormalMap(Renderer::GetWhiteTexture());
			SetMetalnessMap(Renderer::GetWhiteTexture());
			SetRoughnessMap(Renderer::GetWhiteTexture());
		#endif
		}
	}

    LMaterialTable::LMaterialTable(const uint32_t InMaterialCount)
		: MaterialCount(InMaterialCount)
	{
		LASSET_REGISTER();
	}

	LMaterialTable::LMaterialTable(TObjectPtr<LMaterialTable> Other)
		: MaterialCount(Other->MaterialCount)
	{
		LASSET_REGISTER();

		const auto& MeshMaterials = Other->GetMaterials();
		for (auto& [Index, Handle] : MeshMaterials)
		{
			SetMaterial(Index, Handle);
			LK_CORE_DEBUG_TAG("MaterialTable", "Assigning material {} to handle: {}", Index, Handle);
		}
	}

	void LMaterialTable::SetMaterial(const uint32_t Index, const FAssetHandle AssetHandle)
	{
		Materials[Index] = AssetHandle;
		if (Index >= MaterialCount)
		{
			MaterialCount = Index + 1;
		}

		LK_CORE_DEBUG_TAG("MaterialTable", "Assigning material {} to handle {} (count: {})", 
						  Index, AssetHandle, MaterialCount);
	}

	void LMaterialTable::InsertLast(const FAssetHandle AssetHandle)
	{
		Materials[MaterialCount++] = AssetHandle;
	}

	void LMaterialTable::ClearMaterial(const uint32_t Index)
	{
		LK_CORE_ASSERT(HasMaterial(Index));
		Materials.erase(Index);

		if (Index >= MaterialCount)
		{
			MaterialCount = Index + 1;
		}
	}

	void LMaterialTable::Clear()
	{
		Materials.clear();
	}

}
