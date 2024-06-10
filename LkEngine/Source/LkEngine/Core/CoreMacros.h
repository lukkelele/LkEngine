#pragma once


#define LK_UNUSED(_VAR)         ((void)(_VAR))
#define LK_ARRAYSIZE(_ARR)      (static_cast<int>((sizeof(_ARR) / sizeof(*(_ARR))))) 
#define BIT_FIELD(x)            (1 << x)
#define STRINIGFY(x)            #x


/// @TODO: Do Platform implementations in separate headers for Windows/Linux
/** Windows Platform */
#if defined(LK_PLATFORM_WINDOWS)
	#define STDCALL __stdcall
	#define VARARGS __cdecl

	/* Force Inline */
	#ifdef FORCEINLINE
	#undef FORCEINLINE
	#define FORCEINLINE __forceinline
	#endif
	//#define FORCEINLINE __forceinline

	#define LK_ITOA(c, buf, size, p)  _itoa_s(c, buf, size, p)
#endif

/** Linux Platform */
#if defined(LK_PLATFORM_LINUX)
	#define STDCALL __stdcall
	#define VARARGS __cdecl
	#define FORCEINLINE __forceinline
	#define LK_ITOA(c, buf, size, p)  void() // FIXME
#endif



namespace LkEngine {
}