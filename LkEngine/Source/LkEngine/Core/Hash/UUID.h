#pragma once

namespace LkEngine {

	class UUID
	{
	public:
		using SizeType = uint64_t;

		UUID();
		UUID(const SizeType InUUID);
		UUID(const UUID&) = default;

		operator uint64_t() const { return m_UUID; }

	private:
		uint64_t m_UUID = 0;
	};

}

namespace std {
	template <typename T> struct hash;

	template<>
	struct hash<LkEngine::UUID>
	{
		std::size_t operator()(const LkEngine::UUID& uuid) const
		{
			return (uint64_t)uuid;
		}
	};
}

/* Logging formatter. */
template<> 
struct std::formatter<LkEngine::UUID> : std::formatter<std::string>
{
    template<typename FormatContext>
    auto format(const LkEngine::UUID& Uuid, FormatContext& Context) const
    {
		return LK_FMT_LIB::format_to(Context.out(), "{}", static_cast<::LkEngine::UUID::SizeType>(Uuid));
    }
};
