#include "LKpch.h"
#include "MaterialAsset.h"

#include "LkEngine/Renderer/Material.h"
#include "LkEngine/Asset/EditorAssetManager.h"


namespace LkEngine {

	namespace 
	{
		const std::string AlbedoColorUniform  = "u_MaterialUniforms.AlbedoColor";
		const std::string UseNormalMapUniform = "u_MaterialUniforms.UseNormalMap";
		const std::string MetalnessUniform	  = "u_MaterialUniforms.Metalness";
		const std::string RoughnessUniform	  = "u_MaterialUniforms.Roughness";
		const std::string EmissionUniform     = "u_MaterialUniforms.Emission";
		const std::string TransparencyUniform = "u_MaterialUniforms.Transparency";
		
		const std::string AlbedoMapUniform	  = "u_AlbedoTexture";
		const std::string NormalMapUniform	  = "u_NormalTexture";
		const std::string MetalnessMapUniform = "u_MetalnessTexture";
		const std::string RoughnessMapUniform = "u_RoughnessTexture";
	}

	LMaterialAsset::LMaterialAsset(const TObjectPtr<LMaterial>& InMaterial)
	{
		LOBJECT_REGISTER();
		LK_CORE_VERIFY(InMaterial);

		Handle = {};
		Material = LMaterial::Copy(InMaterial);
		LK_CORE_TRACE_TAG("MaterialAsset", "Created material asset ({}) with material {}", Handle, InMaterial->GetName());

		LK_CORE_ASSERT(Material->GetTexture());
	}

	LMaterialAsset::LMaterialAsset(const bool IsTransparent)
		: bTransparent(IsTransparent)
	{
		LOBJECT_REGISTER();

		Handle = {};

		/* TODO: Need to fix this. */
		if (bTransparent)
		{
			LK_CORE_ASSERT(LEditorAssetManager::BaseMaterial);
			Handle = LEditorAssetManager::BaseMaterial->Handle;
			Material = LEditorAssetManager::BaseMaterial->GetMaterial();
		}
		else
		{
			LK_CORE_ASSERT(LEditorAssetManager::BaseMaterial);
			Handle = LEditorAssetManager::BaseMaterial->Handle;
			Material = LEditorAssetManager::BaseMaterial->GetMaterial();
		}

		SetDefaults();

		LK_CORE_TRACE_TAG("MaterialAsset", "Created {} material asset: {}", (bTransparent ? "transparent" : "non-transparent"), Handle);
		LK_CORE_ASSERT(Material);
	}

	glm::vec3& LMaterialAsset::GetAlbedoColor()
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		return Material->GetVector3(AlbedoColorUniform);
	}

	void LMaterialAsset::SetAlbedoColor(const glm::vec3& color)
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		Material->Set(AlbedoColorUniform, color);
	}

	float& LMaterialAsset::GetMetalness()
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		return Material->GetFloat(MetalnessUniform);
	}

	void LMaterialAsset::SetMetalness(const float Value)
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		Material->Set(MetalnessUniform, Value);
	}

	float& LMaterialAsset::GetRoughness()
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		return Material->GetFloat(RoughnessUniform);
	}

	void LMaterialAsset::SetRoughness(float value)
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		Material->Set(RoughnessUniform, value);
	}

	float& LMaterialAsset::GetEmission()
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		return Material->GetFloat(EmissionUniform);
	}

	void LMaterialAsset::SetEmission(float value)
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		Material->Set(EmissionUniform, value);
	}

	float& LMaterialAsset::GetTransparency()
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		return Material->GetFloat(TransparencyUniform);
	}

	void LMaterialAsset::SetTransparency(float transparency)
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		Material->Set(TransparencyUniform, transparency);
	}

	TObjectPtr<LTexture2D> LMaterialAsset::GetAlbedoMap()
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		return Material->TryGetTexture(AlbedoMapUniform);
	}

	void LMaterialAsset::SetAlbedoMap(TObjectPtr<LTexture2D> texture)
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		Material->Set(AlbedoMapUniform, texture);
	}

	void LMaterialAsset::ClearAlbedoMap()
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		//Material->Set(AlbedoMapUniform, Renderer::GetWhiteTexture());
	}

	TObjectPtr<LTexture2D> LMaterialAsset::GetNormalMap()
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		return Material->TryGetTexture(NormalMapUniform);
	}

	void LMaterialAsset::SetNormalMap(TObjectPtr<LTexture2D> texture)
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		Material->Set(NormalMapUniform, texture);
	}

	TObjectPtr<LTexture2D> LMaterialAsset::GetMetalnessMap()
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		return Material->TryGetTexture(MetalnessMapUniform);
	}

	void LMaterialAsset::SetMetalnessMap(TObjectPtr<LTexture2D> texture)
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		Material->Set(MetalnessMapUniform, texture);
	}

	void LMaterialAsset::ClearMetalnessMap()
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		//Material->Set(MetalnessMapUniform, Renderer::GetWhiteTexture());
	}

	TObjectPtr<LTexture2D> LMaterialAsset::GetRoughnessMap()
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		return Material->TryGetTexture(RoughnessMapUniform);
	}

	void LMaterialAsset::SetRoughnessMap(TObjectPtr<LTexture2D> texture)
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		Material->Set(RoughnessMapUniform, texture);
	}

	void LMaterialAsset::ClearRoughnessMap()
	{
		LK_CORE_ASSERT(Material, "{}", LK_FUNCSIG);
		//Material->Set(RoughnessMapUniform, Renderer::GetWhiteTexture());
	}

	void LMaterialAsset::SetDefaults()
	{
		if (bTransparent)
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
		LK_CORE_TRACE_TAG("MaterialTable", "Created new material table with {} materials", InMaterialCount);
	}

	LMaterialTable::LMaterialTable(TObjectPtr<LMaterialTable> Other)
		: MaterialCount(Other->MaterialCount)
	{
		LOBJECT_REGISTER();
		LK_CORE_TRACE_TAG("MaterialTable", "Copying table with {} materials", Other->GetMaterialCount());

		const auto& MeshMaterials = Other->GetMaterials();
		for (auto& [Index, Handle] : MeshMaterials)
		{
			SetMaterial(Index, Handle);
		}
	}

	LMaterialTable::~LMaterialTable()
	{
		LK_CORE_DEBUG_TAG("MaterialTable", "Destroyed table with {} materials", Materials.size());
	}

	void LMaterialTable::SetMaterial(const uint32_t Index, const FAssetHandle AssetHandle)
	{
		LK_CORE_VERIFY(AssetHandle > 0, "SetMaterial called with invalid asset handle: {}", AssetHandle);
		Materials[Index] = AssetHandle;
		if (Index >= MaterialCount)
		{
			MaterialCount = Index + 1;
		}

		LK_CORE_TRACE_TAG("MaterialTable", "Assigning {} to index {} (count: {})", AssetHandle, Index, MaterialCount);
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
