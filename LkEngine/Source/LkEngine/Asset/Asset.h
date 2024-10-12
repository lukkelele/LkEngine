#pragma once

#include <filesystem>

#include "LkEngine/Core/Core.h"

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/LObjectPtr.h"

#include "LkEngine/Asset/AssetTypes.h"


namespace LkEngine {

    using FAssetHandle = UUID;

	/// TODO: Documentation
    /**
     * LAsset
	 *
	 *  Base asset class.
     */
    class LAsset : public LObject
    {
    public:
        virtual ~LAsset() = default;

		FORCEINLINE static EAssetType GetStaticType() { return EAssetType::None; }

		/**
		 * GetAssetType
		 *
		 *  Implemented by LASSET.
		 */
		FORCEINLINE virtual EAssetType GetAssetType() const { return EAssetType::None; }

		virtual bool operator==(const LAsset& Other) const
		{
			return Handle == Other.Handle;
		}
		
		virtual bool operator!=(const LAsset& Other) const
		{
			return !(*this == Other);
		}

		FORCEINLINE bool IsFlagSet(EAssetFlag InFlag) const 
		{ 
			return (InFlag & Flags);
		}

		void SetFlag(AssetFlag::Type InFlag, bool InValue = true)
		{
			if (InValue)
			{
				Flags |= InFlag;
			}
			else
			{
				Flags &= ~InFlag;
			}
		}

    public:
        FAssetHandle Handle = 0;
		uint16_t Flags = (uint16_t)AssetFlag::None;

	private:
		LCLASS(LAsset)
    };

	/**
	 * LAudioAsset
	 */
	class LAudioAsset : public LAsset
	{
	public:
		LAudioAsset() = default;
		~LAudioAsset() = default;

		FORCEINLINE static EAssetType GetStaticType() { return EAssetType::Audio; }

	private:
		LASSET(LAudioAsset)
	};

	struct FAssetMetadata
	{
		FAssetHandle Handle = 0;
		EAssetType Type;
		std::filesystem::path FilePath;
		bool IsDataLoaded = false;
		bool IsMemoryAsset = false;

		FORCEINLINE bool IsValid() const 
		{ 
			return (Handle != 0);
		}
	};

}