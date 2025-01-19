#include "LKpch.h"
#include "MaterialAsset.h"

#include "LkEngine/Renderer/MaterialLibrary.h"


namespace LkEngine {

	namespace 
	{
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
		LOBJECT_REGISTER();
		//LASSET_REGISTER();

		LK_CORE_VERIFY(InMaterial, "Material reference is not valid");

		Handle = {};
		Material = LMaterial::Copy(InMaterial);
		LK_CORE_DEBUG_TAG("MaterialAsset", "Created material asset ({}) with material {}", Handle, InMaterial->GetName());

		/// FIXME
		LK_CORE_ASSERT(Material->GetTexture(""), "Material ({}) texture is nullptr!", Material->GetName());
	}

	LMaterialAsset::LMaterialAsset(const bool IsTransparent)
		: m_Transparent(IsTransparent)
	{
		LOBJECT_REGISTER();
		//LASSET_REGISTER();

		Handle = {};

		if (m_Transparent)
		{
			LK_CORE_VERIFY(LMaterialLibrary::BaseMaterial, "BaseMaterial is not valid");
			Handle = LMaterialLibrary::BaseMaterial->Handle;
			Material = LMaterialLibrary::Get().GetMaterial(BASE_MATERIAL)->GetMaterial();
			LK_CORE_DEBUG_TAG("MaterialAsset", "Created transparent material asset: {}", Handle);
		}
		else
		{
			LK_CORE_VERIFY(LMaterialLibrary::BaseMaterial, "BaseMaterial is not valid");
			Handle = LMaterialLibrary::BaseMaterial->Handle;
			Material = LMaterialLibrary::Get().GetMaterial(BASE_MATERIAL)->GetMaterial();
			LK_CORE_DEBUG_TAG("MaterialAsset", "Created non-transparent material asset: {}", Handle);
		}

		SetDefaults();
		//LK_CORE_INFO_TAG("MaterialAsset", "Created material asset '{}' with default settings", Handle);

		LK_CORE_ASSERT(Material);
	}

	LMaterialAsset::~LMaterialAsset()
	{
		LK_CORE_WARN_TAG("MaterialAsset", "Releasing resources for: {}", Handle);
	}

	glm::vec3& LMaterialAsset::GetAlbedoColor()
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		return Material->GetVector3(s_AlbedoColorUniform);
	}

	void LMaterialAsset::SetAlbedoColor(const glm::vec3& color)
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		Material->Set(s_AlbedoColorUniform, color);
	}

	float& LMaterialAsset::GetMetalness()
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		return Material->GetFloat(s_MetalnessUniform);
	}

	void LMaterialAsset::SetMetalness(const float Value)
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		Material->Set(s_MetalnessUniform, Value);
	}

	float& LMaterialAsset::GetRoughness()
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		return Material->GetFloat(s_RoughnessUniform);
	}

	void LMaterialAsset::SetRoughness(float value)
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		Material->Set(s_RoughnessUniform, value);
	}

	float& LMaterialAsset::GetEmission()
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		return Material->GetFloat(s_EmissionUniform);
	}

	void LMaterialAsset::SetEmission(float value)
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		Material->Set(s_EmissionUniform, value);
	}

	float& LMaterialAsset::GetTransparency()
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		return Material->GetFloat(s_TransparencyUniform);
	}

	void LMaterialAsset::SetTransparency(float transparency)
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		Material->Set(s_TransparencyUniform, transparency);
	}

	TObjectPtr<LTexture2D> LMaterialAsset::GetAlbedoMap()
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		return Material->TryGetTexture(s_AlbedoMapUniform);
	}

	void LMaterialAsset::SetAlbedoMap(TObjectPtr<LTexture2D> texture)
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		Material->Set(s_AlbedoMapUniform, texture);
	}

	void LMaterialAsset::ClearAlbedoMap()
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		//Material->Set(s_AlbedoMapUniform, Renderer::GetWhiteTexture());
	}

	TObjectPtr<LTexture2D> LMaterialAsset::GetNormalMap()
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		return Material->TryGetTexture(s_NormalMapUniform);
	}

	void LMaterialAsset::SetNormalMap(TObjectPtr<LTexture2D> texture)
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		Material->Set(s_NormalMapUniform, texture);
	}

	TObjectPtr<LTexture2D> LMaterialAsset::GetMetalnessMap()
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		return Material->TryGetTexture(s_MetalnessMapUniform);
	}

	void LMaterialAsset::SetMetalnessMap(TObjectPtr<LTexture2D> texture)
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		Material->Set(s_MetalnessMapUniform, texture);
	}

	void LMaterialAsset::ClearMetalnessMap()
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		//Material->Set(s_MetalnessMapUniform, Renderer::GetWhiteTexture());
	}

	TObjectPtr<LTexture2D> LMaterialAsset::GetRoughnessMap()
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		return Material->TryGetTexture(s_RoughnessMapUniform);
	}

	void LMaterialAsset::SetRoughnessMap(TObjectPtr<LTexture2D> texture)
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		Material->Set(s_RoughnessMapUniform, texture);
	}

	void LMaterialAsset::ClearRoughnessMap()
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
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
		LOBJECT_REGISTER();
		//LASSET_REGISTER();

		LK_CORE_DEBUG_TAG("MaterialTable", "Created new material table with {} materials", InMaterialCount);
	}

	LMaterialTable::LMaterialTable(TObjectPtr<LMaterialTable> Other)
		: MaterialCount(Other->MaterialCount)
	{
		LOBJECT_REGISTER();
		//LASSET_REGISTER();

		LK_CORE_INFO_TAG("MaterialTable", "Copying table with {} materials", Other->GetMaterialCount());

		const auto& MeshMaterials = Other->GetMaterials();
		for (auto& [Index, Handle] : MeshMaterials)
		{
			SetMaterial(Index, Handle);
			LK_CORE_DEBUG_TAG("MaterialTable", "Assigning material {} to handle: {}", Index, Handle);
		}
	}

	LMaterialTable::~LMaterialTable()
	{
		LK_CORE_WARN_TAG("MaterialTable", "Destroyed table with {} materials", Materials.size());
	}

	void LMaterialTable::SetMaterial(const uint32_t Index, const FAssetHandle AssetHandle)
	{
		LK_CORE_VERIFY(AssetHandle > 0, "SetMaterial called with invalid asset handle: {}", AssetHandle);
		Materials[Index] = AssetHandle;
		if (Index >= MaterialCount)
		{
			MaterialCount = Index + 1;
		}

		LK_CORE_INFO_TAG("MaterialTable", "Assigning material '{}' to index {} (count: {})", AssetHandle, Index, MaterialCount);
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
