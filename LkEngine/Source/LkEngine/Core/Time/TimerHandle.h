#pragma once

#include "LkEngine/Core/CoreMacros.h"
#include "LkEngine/Core/CoreTypes.h"


namespace LkEngine {

	/**
	 * FTimerHandle
	 */
	class FTimerHandle
	{
	public:
		FTimerHandle() : ID(Counter++) {}
		FTimerHandle(const uint64_t InID) : ID(InID) {}

		FORCEINLINE uint64_t GetID() const { return ID; }
		FORCEINLINE bool IsValid() const { return (ID != 0); }

		FORCEINLINE bool operator==(const FTimerHandle& Other) const 
		{ 
			return (ID == Other.ID); 
		}

		FORCEINLINE void Invalidate() { ID = 0; }

	private:
		/** Generates a unique ID for each timer. */
		static FTimerHandle CreateHandle() { return FTimerHandle{ ++Counter }; }

		uint64_t ID = 0;
		inline static uint64_t Counter = 0;

		friend class LTimerManager;
	};

}


/* Provide hash implementation for FTimerHandle. */
namespace std 
{
    template<>
    struct hash<LkEngine::FTimerHandle>
    {
        std::size_t operator()(const LkEngine::FTimerHandle& TimerHandle) const noexcept
        {
            return std::hash<uint64_t>()(TimerHandle.GetID());
        }
    };
}
