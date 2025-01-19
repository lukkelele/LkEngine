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

		virtual bool operator==(const LAsset& Other) const
		{
			return (Handle == Other.Handle);
		}
		
		virtual bool operator!=(const LAsset& Other) const
		{
			return !(*this == Other);
		}

		bool IsFlagSet(const EAssetFlag InFlag) const 
		{ 
			return (Flags & InFlag);
		}

		void SetFlag(const EAssetFlag InFlag, bool InValue = true)
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

		virtual bool IsValid() const
		{
			return ((Flags & EAssetFlag::Missing) | (Flags & EAssetFlag::Invalid));
		}

    public:
        FAssetHandle Handle = 0;

		/**
		 * The underlying type needs to be used for the member because 
		 * of the enum class. The operations use the enum class as usual,
		 * the primitive type is only used for the flag storage.
		 */
		std::underlying_type_t<EAssetFlag> Flags = static_cast<std::underlying_type_t<EAssetFlag>>(EAssetFlag::None);

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

		FORCEINLINE bool IsValid() const 
		{ 
			return ((Handle > 0) && !bIsMemoryAsset);
		}
		
		FORCEINLINE std::string ToString(const bool CompactFormat = true) const
		{
			if (CompactFormat)
			{
				return std::format("Asset={:8} Type={} File={} Loaded={} MemoryOnly={}",
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
