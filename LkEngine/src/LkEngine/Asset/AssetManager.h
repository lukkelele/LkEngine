#pragma once
#include "Asset.h"
#include "AssetRegistry.h"

#include "AssimpMeshImporter.h"


namespace LkEngine {

    class AssetManager : public RefCounted
    {
    public:
        AssetManager();
        ~AssetManager();

        void Init();

    private:
        AssetRegistry m_Registry;
    };

}