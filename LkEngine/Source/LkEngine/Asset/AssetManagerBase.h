#pragma once

#include <unordered_set>

#include "Asset.h"
#include "AssetRegistry.h"

#include "AssimpMeshImporter.h"


namespace LkEngine {

    class LAssetManagerBase : public LObject
    {
    public:
        virtual ~LAssetManagerBase() = default;

    private:
        LCLASS(LAssetManagerBase)
    };

}