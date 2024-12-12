#pragma once

#include <unordered_set>

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "Asset.h"
#include "AssetRegistry.h"

#include "AssimpMeshImporter.h"


namespace LkEngine {

    class LAssetManagerBase : public LObject
    {
    public:
        virtual ~LAssetManagerBase() = default;

		virtual void Initialize() = 0;
		virtual void Destroy() = 0;

    private:
        LCLASS(LAssetManagerBase)
    };

}
