#pragma once

#include <memory>
#include <functional>

#include "LkEngine/Core/LObject/LObjectBase.h"


namespace LkEngine {

	/**
	 * Unique Pointer. 
	 */
	template<typename T>
	using TUniquePtr = std::unique_ptr<T>;

	template<typename T, typename ...TArgs>
	constexpr TUniquePtr<T> MakeUnique(TArgs&& ...Args)
	{
	#if 0
		if constexpr (sizeof...(Args) == 1)
		{
			/* Unpack the argument and check if it is of type EInitFlag. */
			if constexpr (std::is_same_v<std::decay_t<decltype(std::get<0>(std::forward_as_tuple(Args...)))>, EInitFlag>)
			{
				/* DO CONCEPT HERE */
				//static_assert(std::is_base_of_v<LObject, std::decay_t<T>>, "T is not a derivation of LObject");
				TUniquePtr<T> NewInstance = std::make_unique<T>();
				NewInstance->Initialize();

				return NewInstance;
			}
		}
	#endif

		return std::make_unique<T>(std::forward<TArgs>(Args)...);
	}

	/**
	 * Shared Pointer. 
	 */
	template<typename T>
	using TSharedPtr = std::shared_ptr<T>;

	template<typename T, typename ...TArgs>
	constexpr TSharedPtr<T> MakeShared(TArgs&& ...Args)
	{
		return std::make_shared<T>(std::forward<TArgs>(Args)...);
	}


	/**
	 * Weak Pointer. 
	 */
	template<typename T>
	using TWeakPtr = std::weak_ptr<T>;

}
