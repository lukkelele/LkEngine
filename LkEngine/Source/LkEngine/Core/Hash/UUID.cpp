#include "LKpch.h"
#include "UUID.h"

#include <random>


namespace LkEngine {

	static std::random_device RandomDevice;
	static std::mt19937_64 RandomEngine(RandomDevice());
	static std::uniform_int_distribution<uint64_t> UniformDistribution;

	UUID::UUID()
		: m_UUID(UniformDistribution(RandomEngine))
	{
		/* Never allow UUID to be 0. */
		while (m_UUID == 0)
		{
			m_UUID = UniformDistribution(RandomEngine);
		}
	}

	UUID::UUID(const uint64_t InUUID)
		: m_UUID(InUUID)
	{
	}

}
