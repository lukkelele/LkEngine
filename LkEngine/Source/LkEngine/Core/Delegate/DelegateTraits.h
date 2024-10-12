#pragma once

#include "LkEngine/Core/Delegate/Delegate.h"


/**
 * Delegate type trait helpers.
 */
namespace LkEngine 
{
	template <typename T>
	struct IsMulticastDelegate : std::false_type {};

	template <typename... TArgs>
	struct IsMulticastDelegate<LMulticastDelegate<TArgs...>> : std::true_type {};

	template<typename T>
	inline constexpr bool IsMulticastDelegate_v = IsMulticastDelegate<T>::value;
}
