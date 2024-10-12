#include "LKpch.h"
#include "MaterialAsset.h"


namespace LkEngine {

    static const std::string s_AlbedoColorUniform = "u_MaterialUniforms.AlbedoColor";
	static const std::string s_UseNormalMapUniform = "u_MaterialUniforms.UseNormalMap";
	static const std::string s_MetalnessUniform = "u_MaterialUniforms.Metalness";
	static const std::string s_RoughnessUniform = "u_MaterialUniforms.Roughness";
	static const std::string s_EmissionUniform = "u_MaterialUniforms.Emission";
	static const std::string s_TransparencyUniform = "u_MaterialUniforms.Transparency";
	
	static const std::string s_AlbedoMapUniform = "u_AlbedoTexture";
	static const std::string s_NormalMapUniform = "u_NormalTexture";
	static const std::string s_MetalnessMapUniform = "u_MetalnessTexture";
	static const std::string s_RoughnessMapUniform = "u_RoughnessTexture";

	LMaterialAsset::LMaterialAsset(const TObjectPtr<LMaterial>& material)
	{
		Handle = {};
		m_Material = LMaterial::Copy(material);
		LK_CORE_INFO_TAG("MaterialAsset", "Created material asset ({}) with material {}", 
						 Handle, material->GetName());

		LK_CORE_ASSERT(m_Material->GetTexture(""), "Material ({}) texture is nullptr!", m_Material->GetName());
	}

	LMaterialAsset::LMaterialAsset(const bool IsTransparent)
		: m_Transparent(IsTransparent)
	{
		Handle = {};
		SetDefaults();
		LK_CORE_INFO_TAG("MaterialAsset", "Created material asset with default settings");
	}

	glm::vec3& LMaterialAsset::GetAlbedoColor()
	{
		return m_Material->GetVector3(s_AlbedoColorUniform);
	}

	void LMaterialAsset::SetAlbedoColor(const glm::vec3& color)
	{
		m_Material->Set(s_AlbedoColorUniform, color);
	}

	float& LMaterialAsset::GetMetalness()
	{
		return m_Material->GetFloat(s_MetalnessUniform);
	}

	void LMaterialAsset::SetMetalness(float value)
	{
		LK_CORE_VERIFY(m_Material, "Material is nullptr!");
		m_Material->Set(s_MetalnessUniform, value);
	}

	float& LMaterialAsset::GetRoughness()
	{
		LK_CORE_VERIFY(m_Material, "Material is nullptr!");
		return m_Material->GetFloat(s_RoughnessUniform);
	}

	void LMaterialAsset::SetRoughness(float value)
	{
		LK_CORE_VERIFY(m_Material, "Material is nullptr!");
		m_Material->Set(s_RoughnessUniform, value);
	}

	float& LMaterialAsset::GetEmission()
	{
		LK_CORE_VERIFY(m_Material, "Material is nullptr!");
		return m_Material->GetFloat(s_EmissionUniform);
	}

	void LMaterialAsset::SetEmission(float value)
	{
		m_Material->Set(s_EmissionUniform, value);
	}

	float& LMaterialAsset::GetTransparency()
	{
		return m_Material->GetFloat(s_TransparencyUniform);
	}

	void LMaterialAsset::SetTransparency(float transparency)
	{
		m_Material->Set(s_TransparencyUniform, transparency);
	}

	TObjectPtr<LTexture2D> LMaterialAsset::GetAlbedoMap()
	{
		return m_Material->TryGetTexture(s_AlbedoMapUniform);
	}

	void LMaterialAsset::SetAlbedoMap(TObjectPtr<LTexture2D> texture)
	{
		m_Material->Set(s_AlbedoMapUniform, texture);
	}

	void LMaterialAsset::ClearAlbedoMap()
	{
		//m_Material->Set(s_AlbedoMapUniform, Renderer::GetWhiteTexture());
	}

	TObjectPtr<LTexture2D> LMaterialAsset::GetNormalMap()
	{
		return m_Material->TryGetTexture(s_NormalMapUniform);
	}

	void LMaterialAsset::SetNormalMap(TObjectPtr<LTexture2D> texture)
	{
		m_Material->Set(s_NormalMapUniform, texture);
	}

	TObjectPtr<LTexture2D> LMaterialAsset::GetMetalnessMap()
	{
		return m_Material->TryGetTexture(s_MetalnessMapUniform);
	}

	void LMaterialAsset::SetMetalnessMap(TObjectPtr<LTexture2D> texture)
	{
		m_Material->Set(s_MetalnessMapUniform, texture);
	}

	void LMaterialAsset::ClearMetalnessMap()
	{
		//m_Material->Set(s_MetalnessMapUniform, Renderer::GetWhiteTexture());
	}

	TObjectPtr<LTexture2D> LMaterialAsset::GetRoughnessMap()
	{
		return m_Material->TryGetTexture(s_RoughnessMapUniform);
	}

	void LMaterialAsset::SetRoughnessMap(TObjectPtr<LTexture2D> texture)
	{
		m_Material->Set(s_RoughnessMapUniform, texture);
	}

	void LMaterialAsset::ClearRoughnessMap()
	{
		//m_Material->Set(s_RoughnessMapUniform, Renderer::GetWhiteTexture());
	}

	void LMaterialAsset::SetDefaults()
	{
		if (m_Transparent)
		{
			// Set defaults
			SetAlbedoColor(glm::vec3(0.8f));

			// Maps
			//SetAlbedoMap(Renderer::GetWhiteTexture());
		}
		else
		{
			// Set defaults
			SetAlbedoColor(glm::vec3(0.8f));
			SetEmission(0.0f);
			//SetUseNormalMap(false);
			SetMetalness(0.0f);
			SetRoughness(0.4f);

			// Maps
			//SetAlbedoMap(Renderer::GetWhiteTexture());
			//SetNormalMap(Renderer::GetWhiteTexture());
			//SetMetalnessMap(Renderer::GetWhiteTexture());
			//SetRoughnessMap(Renderer::GetWhiteTexture());
		}
	}

    LMaterialTable::LMaterialTable(uint32_t materialCount)
		: m_MaterialCount(materialCount)
	{
	}

	LMaterialTable::LMaterialTable(TObjectPtr<LMaterialTable> Other)
		: m_MaterialCount(Other->m_MaterialCount)
	{
		const auto& meshMaterials = Other->GetMaterials();
		for (auto& [index, materialAsset] : meshMaterials)
		{
			//SetMaterial(index, Ref<MaterialAsset>::Create(materialAsset->GetMaterial()));
			SetMaterial(index, materialAsset);
			LK_CORE_DEBUG_TAG("MaterialTable", "Setting material {} to material FAssetHandle: {}", index, materialAsset);
		}
	}

	void LMaterialTable::SetMaterial(uint32_t index, FAssetHandle material)
	{
		m_Materials[index] = material;
		if (index >= m_MaterialCount)
		{
			m_MaterialCount = index + 1;
		}

		LK_CORE_DEBUG_TAG("MaterialTable", "Setting material {} to handle {} (m_MaterialCount={})", index, material, m_MaterialCount);
	}

	void LMaterialTable::InsertLast(FAssetHandle material)
	{
		m_Materials[m_MaterialCount++] = material;
	}

	void LMaterialTable::ClearMaterial(uint32_t index)
	{
		LK_CORE_ASSERT(HasMaterial(index));
		m_Materials.erase(index);

		if (index >= m_MaterialCount)
		{
			m_MaterialCount = index + 1;
		}
	}

	void LMaterialTable::Clear()
	{
		m_Materials.clear();
	}


}