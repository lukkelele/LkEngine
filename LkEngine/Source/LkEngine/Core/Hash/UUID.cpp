#include "LKpch.h"
#include "UUID.h"

#include <random>


namespace LkEngine {

	static std::random_device RandomDevice;
	static std::mt19937_64 RandomEngine(RandomDevice());
	static std::uniform_int_distribution<LUUID::SizeType> UniformDistribution;

	LUUID::LUUID()
		: UUID(UniformDistribution(RandomEngine))
	{
		/* Never allow an UUID to be 0. */
		while (UUID == 0)
		{
			UUID = UniformDistribution(RandomEngine);
		}
	}

	LUUID::LUUID(const SizeType InUUID)
		: UUID(InUUID)
	{
	}

}
