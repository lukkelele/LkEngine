#pragma once

#include <algorithm>
#include <functional>
#include <memory>

#include "LkEngine/Core/LObject/ObjectBase.h"


namespace LkEngine {

	/**
	 * Unique Pointer. 
	 */
#if LK_CUSTOM_UNIQUE_PTR
#	include "UniquePtr.h"
#else
	template<typename T>
	using TUniquePtr = std::unique_ptr<T>;

	template<typename T, typename ...TArgs>
	constexpr TUniquePtr<T> MakeUnique(TArgs&& ...Args)
	{
		return std::make_unique<T>(std::forward<TArgs>(Args)...);
	}
#endif

	/**
	 * Shared Pointer. 
	 */
#if LK_CUSTOM_SHARED_PTR
#	include "SharedPtr.h"
#else
	template<typename T>
	using TSharedPtr = std::shared_ptr<T>;

	template<typename T, typename ...TArgs>
	constexpr TSharedPtr<T> MakeShared(TArgs&& ...Args)
	{
		return std::make_shared<T>(std::forward<TArgs>(Args)...);
	}
#endif

	/**
	 * Weak Pointer. 
	 */
#if LK_CUSTOM_WEAK_PTR
#	include "WeakPtr.h"
#else
	template<typename T>
	using TWeakPtr = std::weak_ptr<T>;
#endif

}
