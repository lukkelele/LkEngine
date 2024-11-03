#include "LKpch.h"
#include "UUID.h"

#include <random>


namespace LkEngine {

	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_Engine(s_RandomDevice());
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

	UUID::UUID()
		: m_UUID(s_UniformDistribution(s_Engine))
	{
		/* Never allow UUID to be 0. */
		while (m_UUID == 0)
		{
			m_UUID = s_UniformDistribution(s_Engine);
		}
	}

	UUID::UUID(const uint64_t InUUID)
		: m_UUID(InUUID)
	{
	}

}