#pragma once

#include <filesystem>

#include "LkEngine/Core/Core.h"

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "LkEngine/Asset/AssetTypes.h"


namespace LkEngine {

    using FAssetHandle = LUUID;

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

		/// FIXME: Use LK_ENUM_CLASS_FLAGS
		FORCEINLINE bool IsFlagSet(const EAssetFlag InFlag) const 
		{ 
			return (Flags & static_cast<uint16_t>(InFlag));
		}

		/// FIXME: Use LK_ENUM_CLASS_FLAGS
		FORCEINLINE void SetFlag(const EAssetFlag InFlag, bool InValue = true)
		{
			if (InValue)
			{
				//Flags |= InFlag;
				Flags |= static_cast<uint16_t>(InFlag);
			}
			else
			{
				//Flags &= ~InFlag;
				Flags &= ~(static_cast<uint16_t>(InFlag));
			}
		}

		FORCEINLINE virtual bool IsValid() const
		{
			return LObject::IsValid();
		}

    public:
        FAssetHandle Handle = 0;
		uint16_t Flags = static_cast<uint16_t>(EAssetFlag::None);

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
		EAssetType Type = EAssetType::None;
		std::filesystem::path FilePath{};
		bool bIsDataLoaded = false;
		bool bIsMemoryAsset = false;

		FORCEINLINE bool IsValid() const { return (Handle > 0); }
		
		FORCEINLINE std::string ToString() const
		{
			return std::format("[Asset Metadata: {}]\n * Type: {}\n * Filepath: {}\n * Is Loaded: {}\n * Memory Asset: {}",
							   Handle, Enum::ToString(Type), 
							   FilePath.string(), 
							   (bIsDataLoaded ? "Yes" : "No"), 
							   (bIsMemoryAsset ? "Yes" : "No"));
		}
	};

}
