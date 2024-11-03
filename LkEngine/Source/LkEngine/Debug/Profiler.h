#pragma once

/* Always enable profiling in non-dist builds. */
#ifndef LK_PROFILING
#define LK_PROFILING (!LK_DIST)
#endif

#if LK_PROFILING 
#include <tracy/Tracy.hpp>
#endif

#if defined(LK_PROFILING)
#define LK_SCOPE_PERF(Name) \
		FScopePerfTimer LK_SCOPED_PerfTimer__LINE__(Name, LApplication::Get().GetPerformanceProfiler());
#define LK_SCOPE_TIMER(Name) \
		FScopedTimer LK_SCOPED_Timer__LINE__(Name);
#else
#	define LK_SCOPE_PERF(Name)    (void)(Name)
#	define LK_SCOPE_TIMER(Name)   (void)(Name)
#endif

