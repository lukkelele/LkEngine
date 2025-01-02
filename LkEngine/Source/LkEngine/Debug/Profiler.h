#pragma once

/* Always enable profiling in non-dist builds. */
/* FIXME: Re-enable this. */
#ifndef LK_PROFILING
//#define LK_PROFILING !LK_DIST
#define LK_PROFILING 0
#endif

#if LK_PROFILING 
#include <tracy/Tracy.hpp>
#endif

#if LK_PROFILING
#	define LK_SCOPE_PERF(Name)	   FScopePerfTimer LK_SCOPED_PerfTimer__LINE__(Name, LApplication::Get().GetPerformanceProfiler());
#	define LK_SCOPE_TIMER(Name)	   FScopedTimer LK_SCOPED_Timer__LINE__(Name);

#	define LK_PROFILE_MARK_FRAME   FrameMark
#	define LK_PROFILE_FUNC(...)	   ZoneScoped##__VA_OPT__(N(__VA_ARGS__))
#	define LK_PROFILE_THREAD(...)  tracy::SetThreadName(__VA_ARGS__)
#else
#	define LK_SCOPE_PERF(Name)     (void)(Name)
#	define LK_SCOPE_TIMER(Name)    (void)(Name)

#	define LK_PROFILE_MARK_FRAME   (void)(0)
#	define LK_PROFILE_FUNC(...)    
#	define LK_PROFILE_THREAD(...)
#endif

