#pragma once

#include "Material.h"


namespace LkEngine {

    //using MaterialCollection = std::unordered_map<std::string, s_ptr<Material>>;
    using MaterialCollection = std::unordered_map<std::string, s_ptr<Material>>;

    class MaterialLibrary
    {
    public:
        MaterialLibrary();
        ~MaterialLibrary() = default;

        static MaterialLibrary* Get() { return m_Instance; }

        void Init();
        void Add(s_ptr<Material>& material);
        s_ptr<Material> GetMaterial(std::string_view materialName);
        MaterialCollection& GetMaterials() { return m_Collection; }

    private:
        void CreateBasicMaterials();

    private:
        MaterialCollection m_Collection;

        inline static MaterialLibrary* m_Instance = nullptr;
    };

}