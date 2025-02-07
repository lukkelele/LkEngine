#pragma once

#include "LkEngine/Core/Delegate/Delegate.h"


namespace LkEngine::Core {

	/**
	 * @brief Helper for checking if a delegate is a multicast delegate.
	 */
	template <typename T>
	struct IsMulticastDelegate : std::false_type {};

	template <typename... TArgs>
	struct IsMulticastDelegate<LMulticastDelegate<TArgs...>> : std::true_type {};

	template<typename T>
	inline constexpr bool IsMulticastDelegate_v = IsMulticastDelegate<T>::value;

}
