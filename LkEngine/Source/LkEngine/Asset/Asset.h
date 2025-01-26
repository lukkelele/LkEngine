#pragma once

#include <filesystem>

#include "LkEngine/Core/Core.h"

#include "LkEngine/Core/LObject/Object.h"
#include "LkEngine/Core/LObject/ObjectPtr.h"

#include "LkEngine/Asset/AssetTypes.h"


namespace LkEngine {

	using FAssetHandle = LUUID;

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
		FORCEINLINE virtual EAssetType GetAssetType() const { return EAssetType::None; }

		FORCEINLINE bool HasFlag(const EAssetFlag Flag) const 
		{ 
			return (AssetFlags & Flag);
		}

		FORCEINLINE void SetFlag(const EAssetFlag Flag, bool Value = true)
		{
			if (Value)
			{
				AssetFlags |= Flag;
			}
			else
			{
				AssetFlags &= ~Flag;
			}
		}

		FORCEINLINE virtual bool IsValid() const
		{
			return ((AssetFlags & EAssetFlag::Missing) | (AssetFlags & EAssetFlag::Invalid));
		}

		virtual bool operator==(const LAsset& Other) const
		{
			return (Handle == Other.Handle);
		}
		
		virtual bool operator!=(const LAsset& Other) const
		{
			return !(*this == Other);
		}

    public:
        FAssetHandle Handle = 0;

		/**
		 * The underlying type needs to be used for the member because 
		 * of the enum class. The operations use the enum class as usual,
		 * the primitive type is only used for the flag storage.
		 */
		std::underlying_type_t<EAssetFlag> AssetFlags = static_cast<std::underlying_type_t<EAssetFlag>>(EAssetFlag::None);

	private:
		LCLASS(LAsset)
    };

#if 0
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
#endif

	struct FAssetMetadata
	{
		FAssetHandle Handle = 0;
		EAssetType Type = EAssetType::None;
		std::filesystem::path FilePath{};
		bool bIsDataLoaded = false;
		bool bIsMemoryAsset = false;

		FORCEINLINE bool IsValid() const 
		{ 
			return ((Handle > 0) && !bIsMemoryAsset);
		}
		
		FORCEINLINE std::string ToString(const bool CompactFormat = true) const
		{
			if (CompactFormat)
			{
				return std::format("Asset={:12} Type={} File={} Loaded={} MemoryOnly={}",
								   Handle, Enum::ToString(Type), 
								   FilePath.string(), 
								   (bIsDataLoaded ? "Yes" : "No"), 
								   (bIsMemoryAsset ? "Yes" : "No"));
			}

			return std::format("[Asset: {}]\n * Type: {}\n * Filepath: {}\n * Loaded: {}\n * Memory Asset: {}",
							   Handle, Enum::ToString(Type), 
							   FilePath.string(), 
							   (bIsDataLoaded ? "Yes" : "No"), 
							   (bIsMemoryAsset ? "Yes" : "No"));
		}
	};

}
