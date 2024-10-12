#pragma once

#include <memory>
#include <functional>

//
// TODO
//

namespace LkEngine {

	/**
	 * Unique Pointer. 
	 */
	template<typename T>
	using TUniquePtr = std::unique_ptr<T>;
	template<typename T, typename ...TArgs>
	constexpr TUniquePtr<T> MakeUnique(TArgs&& ...Args)
	{
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
