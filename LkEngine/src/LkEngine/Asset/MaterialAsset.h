#pragma once

#include "LkEngine/Asset/Asset.h"

#include "LkEngine/Renderer/Texture.h"
#include "LkEngine/Renderer/Material.h"


namespace LkEngine {

    class MaterialAsset : public Asset
    {
    public:
		//MaterialAsset(Ref<Material> material);
		MaterialAsset(const Ref<Material>& material);
        MaterialAsset(bool transparent = false);
		~MaterialAsset();

		glm::vec3& GetAlbedoColor();
		void SetAlbedoColor(const glm::vec3& color);

		float& GetMetalness();
		void SetMetalness(float value);

		float& GetRoughness();
		void SetRoughness(float value);

		float& GetEmission();
		void SetEmission(float value);

		static AssetType GetStaticType() { return AssetType::Material; }
		virtual AssetType GetAssetType() const override { return GetStaticType(); }

		Ref<Material> GetMaterial() const { return m_Material; }
		void SetMaterial(Ref<Material> material) { m_Material = material; }

        void SetTransparency(float transparency);
        float& GetTransparency();
		bool IsTransparent() const { return m_Transparent; }

        Ref<Texture2D> GetAlbedoMap();
        void SetAlbedoMap(Ref<Texture2D> texture);
        void ClearAlbedoMap();

        Ref<Texture2D> GetNormalMap();
        void SetNormalMap(Ref<Texture2D> texture);

        Ref<Texture2D> GetMetalnessMap();
        void SetMetalnessMap(Ref<Texture2D> texture);
        void ClearMetalnessMap();

        Ref<Texture2D> GetRoughnessMap();
        void SetRoughnessMap(Ref<Texture2D> texture);
        void ClearRoughnessMap();

	private:
		void SetDefaults();

	private:
		Ref<Material> m_Material = nullptr;
		bool m_Transparent = false;

		friend class MaterialEditor;
    };



    class MaterialTable : public RefCounted
    {
    public:
        MaterialTable(uint32_t materialCount = 1);
        MaterialTable(Ref<MaterialTable> other);
        ~MaterialTable() = default;

        void SetMaterial(uint32_t index, AssetHandle material);
        void ClearMaterial(uint32_t index);

        void InsertLast(AssetHandle material);

        bool HasMaterial(uint32_t materialIndex) const 
        { 
            return m_Materials.find(materialIndex) != m_Materials.end(); 
        }

        AssetHandle GetMaterialHandle(uint32_t materialIndex) const
        {
            LK_CORE_ASSERT(HasMaterial(materialIndex));
            return m_Materials.at(materialIndex);
        }

        std::map<uint32_t, AssetHandle>& GetMaterials() { return m_Materials; }
        const std::map<uint32_t, AssetHandle>& GetMaterials() const { return m_Materials; }

        uint32_t GetMaterialCount() const { return m_MaterialCount; }
        void SetMaterialCount(uint32_t materialCount) { m_MaterialCount = materialCount; }

        void Clear();

    private:
        std::map<uint32_t, AssetHandle> m_Materials;
        uint32_t m_MaterialCount;
    };



}