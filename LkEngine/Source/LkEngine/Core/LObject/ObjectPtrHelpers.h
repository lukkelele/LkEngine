/******************************************************************
 * ObjectPtrHelpers.h
 *
 *******************************************************************/

#pragma once

#define LK_OBJECTPTR_CUSTOM_EQUALITY_OP 1

#define LK_DEBUG_LOG_LIVE_REFERENCES 0  /* Log whenever a reference is added/removed. */
#define LK_DEBUG_LOG_OBJECTPTR       0  /* Log TObjectPtr constructors and destructor calls. */
#define LK_DEBUG_OBJECTPTR_PARANOID  0  /* Validate the LObject pointer on all TObjectPtr access calls. */

#if LK_DEBUG_OBJECTPTR_PARANOID
#	define LK_PTR_ASSERT(...) LK_CORE_ASSERT(__VA_ARGS__)
#else
#	define LK_PTR_ASSERT(...)
#endif

#if LK_DEBUG_LOG_OBJECTPTR
#	define LK_DEBUG_PTR_LOG(...) \
			if (ObjectPtr) \
			{ \
				if (const LClass* ClassInfo = ObjectPtr->GetClass(); ClassInfo != nullptr) \
				{ \
					LK_PRINTLN("[TObjectPtr::{}] {}", ClassInfo->GetName(), __VA_ARGS__); \
				} \
				else \
				{ \
					LK_PRINTLN("[TObjectPtr::LObject] {}", __VA_ARGS__); \
				} \
			} \
			else \
			{ \
				LK_PRINTLN("[TObjectPtr] {}", __VA_ARGS__); \
			} 
#else
#	define LK_DEBUG_PTR_LOG(...) 
#endif
