/******************************************************************
 * DelegateFwd
 * 
 * Forward declarations and macro definitions.
 *
 *  - LK_DECLARE_DELEGATE
 *  - LK_DECLARE_MULTICAST_DELEGATE
 *  - LK_DECLARE_DELEGATE_RET
 *  - LK_DECLARE_EVENT
 *
 *******************************************************************/
#pragma once

#define LK_DECLARE_DELEGATE(DelegateName, ...) \
	using DelegateName = ::LkEngine::Core::LDelegate<#DelegateName, void __VA_OPT__(,__VA_ARGS__)>

#define LK_DECLARE_DELEGATE_RET(DelegateName, ReturnValue, ...) \
	using DelegateName = ::LkEngine::Core::LDelegate<#DelegateName, ReturnValue __VA_OPT__(,__VA_ARGS__)>

#define LK_DECLARE_MULTICAST_DELEGATE(DelegateName, ...) \
	using DelegateName = ::LkEngine::Core::LMulticastDelegate<__VA_ARGS__>; \
	using DelegateName ## _DelegateType = ::LkEngine::Core::LMulticastDelegate<__VA_ARGS__>::TDelegate

#define LK_DECLARE_EVENT(EventName, TOwner, ...) \
	class EventName : public ::LkEngine::Core::LMulticastDelegate<__VA_ARGS__> \
	{ \
	private: \
		friend class TOwner; \
		using ::LkEngine::Core::LMulticastDelegate::Broadcast; \
		using ::LkEngine::Core::LMulticastDelegate::RemoveAll; \
		using ::LkEngine::Core::LMulticastDelegate::Remove; \
	};
