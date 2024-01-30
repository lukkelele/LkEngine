#pragma once

#include "Material.h"


namespace LkEngine {

    //using MaterialCollection = std::unordered_map<std::string, Ref<Material>>;
    using MaterialCollection = std::unordered_map<std::string, Ref<Material>>;

    class MaterialLibrary : public RefCounted
    {
    public:
        MaterialLibrary();
        ~MaterialLibrary() = default;

        static Ref<MaterialLibrary> Get() { return m_Instance; }

        void Init();
        void Add(const Ref<Material> material);

        Ref<Material> GetMaterial(std::string_view materialName);
        MaterialCollection& GetMaterials() { return m_Collection; }

    private:
        void CreateBasicMaterials();

    private:
        MaterialCollection m_Collection;

        inline static Ref<MaterialLibrary> m_Instance = nullptr;
    };

}