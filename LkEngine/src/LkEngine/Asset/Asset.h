#pragma once

#include "LkEngine/Core/Base.h"
#include "LkEngine/Core/UUID.h"
#include "LkEngine/Asset/AssetTypes.h"


namespace LkEngine {

    using AssetHandle = UUID;

    class Asset
    {
    public:
        virtual ~Asset() = default;

		static AssetType GetStaticType() { return AssetType::None; }
		virtual AssetType GetAssetType() const { return AssetType::None; }

		virtual bool operator==(const Asset& other) const
		{
			return Handle == other.Handle;
		}
		
		virtual bool operator!=(const Asset& other) const
		{
			return !(*this == other);
		}

		bool IsFlagSet(AssetFlag flag) const { return (uint16_t)flag & Flags; }
		void SetFlag(AssetFlag flag, bool value = true)
		{
			if (value)
				Flags |= (uint16_t)flag;
			else
				Flags &= ~(uint16_t)flag;
		}

    public:
        AssetHandle Handle = 0;
		uint16_t Flags = (uint16_t)AssetFlag::None;
    };

}