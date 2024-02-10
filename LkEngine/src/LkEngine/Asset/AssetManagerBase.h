#pragma once

#include <unordered_set>

#include "Asset.h"
#include "AssetRegistry.h"

#include "AssimpMeshImporter.h"


namespace LkEngine {

    class AssetManagerBase : public RefCounted
    {
    public:
        virtual ~AssetManagerBase() = default;
    };

}