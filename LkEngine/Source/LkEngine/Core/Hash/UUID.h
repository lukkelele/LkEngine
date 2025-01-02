#pragma once

namespace LkEngine {

	/**
	 * LUUID
	 *
	 *	Unique identifier.
	 */
	struct LUUID
	{
		using SizeType = uint64_t;

		LUUID();
		LUUID(const SizeType InUUID);
		LUUID(const LUUID&) = default;

	#if 0
		bool operator==(const LUUID& Other) const { return (UUID == Other.UUID); }
		bool operator!=(const LUUID& Other) const { return !(*this == Other); }
	#endif

		operator uint64_t() const { return UUID; }

	private:
		uint64_t UUID = 0;
	};

	static_assert(std::disjunction_v<
					std::is_same<LUUID::SizeType, uint64_t>,
					std::is_same<LUUID::SizeType, uint32_t>
				  >, "LUUID::SizeType is not valid");
}

namespace std 
{
	template<typename T> struct hash;

	template<>
	struct hash<LkEngine::LUUID>
	{
		std::size_t operator()(const LkEngine::LUUID& uuid) const
		{
			return (uint64_t)uuid;
		}
	};
}

/* Logging formatter. */
template<> 
struct std::formatter<LkEngine::LUUID> : std::formatter<std::string>
{
    template<typename FormatContext>
    auto format(const LkEngine::LUUID& Uuid, FormatContext& Context) const
    {
		return LK_FMT_LIB::format_to(Context.out(), "{}", static_cast<::LkEngine::LUUID::SizeType>(Uuid));
    }
};
