/****************************************************************
 * Logging Macros
 *
 ****************************************************************/
#pragma once

/** 
 * Raw print functions.
 */
#define LK_PRINT(...)     ::LkEngine::LLog::Print(__VA_ARGS__)
#define LK_PRINTLN(...)   ::LkEngine::LLog::PrintLn(__VA_ARGS__)

/** 
 * Core logging.
 */
#define LK_CORE_TRACE(...)   ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Trace, __VA_ARGS__)
#define LK_CORE_DEBUG(...)   ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Debug, __VA_ARGS__)
#define LK_CORE_INFO(...)    ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Info, __VA_ARGS__)
#define LK_CORE_WARN(...)    ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Warning, __VA_ARGS__)
#define LK_CORE_ERROR(...)   ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Error, __VA_ARGS__)
#define LK_CORE_FATAL(...)   ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Fatal, __VA_ARGS__)

#define LK_CORE_TRACE_TAG(Tag, ...)  ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Trace, Tag, __VA_ARGS__)
#define LK_CORE_DEBUG_TAG(Tag, ...)  ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Debug, Tag, __VA_ARGS__)
#define LK_CORE_INFO_TAG(Tag, ...)   ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Info, Tag, __VA_ARGS__)
#define LK_CORE_WARN_TAG(Tag, ...)   ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Warning, Tag, __VA_ARGS__)
#define LK_CORE_ERROR_TAG(Tag, ...)  ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Error, Tag, __VA_ARGS__)
#define LK_CORE_FATAL_TAG(Tag, ...)  ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Core, ::LkEngine::ELogLevel::Fatal, Tag, __VA_ARGS__)

/** 
 * Client logging.
 */
#define LK_TRACE(...)   ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Trace, __VA_ARGS__)
#define LK_DEBUG(...)   ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Info, __VA_ARGS__)
#define LK_INFO(...)    ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Info, __VA_ARGS__)
#define LK_WARN(...)    ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Warning, __VA_ARGS__)
#define LK_ERROR(...)   ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Error, __VA_ARGS__)
#define LK_FATAL(...)   ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Fatal, __VA_ARGS__)

#define LK_TRACE_TAG(Tag, ...)   ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Trace, Tag, __VA_ARGS__)
#define LK_DEBUG_TAG(Tag, ...)   ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Debug, Tag, __VA_ARGS__)
#define LK_INFO_TAG(Tag, ...)    ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Info, Tag, __VA_ARGS__)
#define LK_WARN_TAG(Tag, ...)    ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Warning, Tag, __VA_ARGS__)
#define LK_ERROR_TAG(Tag, ...)   ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Error, Tag, __VA_ARGS__)
#define LK_FATAL_TAG(Tag, ...)   ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Client, ::LkEngine::ELogLevel::Fatal, Tag, __VA_ARGS__)

/** 
 * UI logging.
 */
#define LK_UI_TRACE(...)  ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Trace, __VA_ARGS__)
#define LK_UI_DEBUG(...)  ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Debug, __VA_ARGS__)
#define LK_UI_INFO(...)   ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Info, __VA_ARGS__)
#define LK_UI_WARN(...)   ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Warning, __VA_ARGS__)
#define LK_UI_ERROR(...)  ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Error, __VA_ARGS__)
#define LK_UI_FATAL(...)  ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Fatal, __VA_ARGS__)

#define LK_UI_TRACE_TAG(Tag, ...) ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Trace, Tag, __VA_ARGS__)
#define LK_UI_DEBUG_TAG(Tag, ...) ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Debug, Tag, __VA_ARGS__)
#define LK_UI_INFO_TAG(Tag, ...)  ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Info, Tag, __VA_ARGS__)
#define LK_UI_WARN_TAG(Tag, ...)  ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Warning, Tag, __VA_ARGS__)
#define LK_UI_ERROR_TAG(Tag, ...) ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Error, Tag, __VA_ARGS__)
#define LK_UI_FATAL_TAG(Tag, ...) ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::UI, ::LkEngine::ELogLevel::Fatal, Tag, __VA_ARGS__)

/** 
 * Asset logging.
 */
#define LK_ASSET_TRACE(...) ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Trace, __VA_ARGS__)
#define LK_ASSET_DEBUG(...) ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Debug, __VA_ARGS__)
#define LK_ASSET_INFO(...)  ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Info, __VA_ARGS__)
#define LK_ASSET_WARN(...)  ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Warning, __VA_ARGS__)
#define LK_ASSET_ERROR(...) ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Error, __VA_ARGS__)
#define LK_ASSET_FATAL(...) ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Fatal, __VA_ARGS__)

#define LK_ASSET_TRACE_TAG(Tag, ...) ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Trace, Tag, __VA_ARGS__)
#define LK_ASSET_DEBUG_TAG(Tag, ...) ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Debug, Tag, __VA_ARGS__)
#define LK_ASSET_INFO_TAG(Tag, ...)  ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Info, Tag, __VA_ARGS__)
#define LK_ASSET_WARN_TAG(Tag, ...)  ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Warning, Tag, __VA_ARGS__)
#define LK_ASSET_ERROR_TAG(Tag, ...) ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Error, Tag, __VA_ARGS__)
#define LK_ASSET_FATAL_TAG(Tag, ...) ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::Asset, ::LkEngine::ELogLevel::Fatal, Tag, __VA_ARGS__)

/** 
 * Test logging. 
 */
#define LK_TEST_TRACE(...)	  ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Trace, __VA_ARGS__)
#define LK_TEST_DEBUG(...)	  ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Debug, __VA_ARGS__)
#define LK_TEST_INFO(...)	  ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Info, __VA_ARGS__)
#define LK_TEST_WARN(...)	  ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Warning, __VA_ARGS__)
#define LK_TEST_ERROR(...)	  ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Error, __VA_ARGS__)
#define LK_TEST_FATAL(...)	  ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Fatal, __VA_ARGS__)

#define LK_TEST_TRACE_TAG(Tag, ...)  ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Trace, Tag, __VA_ARGS__)
#define LK_TEST_DEBUG_TAG(Tag, ...)  ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Debug, Tag, __VA_ARGS__)
#define LK_TEST_INFO_TAG(Tag, ...)   ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Info, Tag, __VA_ARGS__)
#define LK_TEST_WARN_TAG(Tag, ...)   ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Warning, Tag, __VA_ARGS__)
#define LK_TEST_ERROR_TAG(Tag, ...)  ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Error, Tag, __VA_ARGS__)
#define LK_TEST_FATAL_TAG(Tag, ...)  ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Fatal, Tag, __VA_ARGS__)

#define LK_TEST_SUCCESS(...)           ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Info,  __VA_ARGS__)
#define LK_TEST_FAIL(...)	           ::LkEngine::LLog::PrintMessage(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Error, __VA_ARGS__)
#define LK_TEST_SUCCESS_TAG(Tag, ...)  ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Info,  Tag, __VA_ARGS__)
#define LK_TEST_FAIL_TAG(Tag, ...)	   ::LkEngine::LLog::PrintMessageWithTag(::LkEngine::ELoggerType::TestRunner, ::LkEngine::ELogLevel::Error, Tag, __VA_ARGS__)
