#pragma once

#include "Apex/Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Apex {

	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;

	};

}

//Core Log Macros
// #define APEX_CORE_TRACE(...)		::Apex::Log::GetCoreLogger()->trace(__VA_ARGS__)
// #define APEX_CORE_DEBUG(...)		::Apex::Log::GetCoreLogger()->debug(__VA_ARGS__)
// #define APEX_CORE_INFO(...)			::Apex::Log::GetCoreLogger()->info(__VA_ARGS__)
// #define APEX_CORE_WARN(...)			::Apex::Log::GetCoreLogger()->warn(__VA_ARGS__)
// #define APEX_CORE_ERROR(...)		::Apex::Log::GetCoreLogger()->error(__VA_ARGS__)
// #define APEX_CORE_CRITICAL(...)		::Apex::Log::GetCoreLogger()->critical(__VA_ARGS__); __DEBUG_BREAK__;

#define APEX_CORE_TRACE(...)		SPDLOG_LOGGER_TRACE(Apex::Log::GetCoreLogger(), __VA_ARGS__)
#define APEX_CORE_DEBUG(...)		SPDLOG_LOGGER_DEBUG(Apex::Log::GetCoreLogger(), __VA_ARGS__)
#define APEX_CORE_INFO(...)			SPDLOG_LOGGER_INFO(Apex::Log::GetCoreLogger(), __VA_ARGS__)
#define APEX_CORE_WARN(...)			SPDLOG_LOGGER_WARN(Apex::Log::GetCoreLogger(), __VA_ARGS__)
#define APEX_CORE_ERROR(...)		SPDLOG_LOGGER_ERROR(Apex::Log::GetCoreLogger(), __VA_ARGS__)
#define APEX_CORE_CRITICAL(...)		SPDLOG_LOGGER_CRITICAL(Apex::Log::GetCoreLogger(), __VA_ARGS__); __DEBUG_BREAK__;


//Client Log Macros
// #define APEX_LOG_TRACE(...)				::Apex::Log::GetClientLogger()->trace(__VA_ARGS__)
// #define APEX_LOG_DEBUG(...)				::Apex::Log::GetClientLogger()->debug(__VA_ARGS__)
// #define APEX_LOG_INFO(...)				::Apex::Log::GetClientLogger()->info(__VA_ARGS__)
// #define APEX_LOG_WARN(...)				::Apex::Log::GetClientLogger()->warn(__VA_ARGS__)
// #define APEX_LOG_ERROR(...)				::Apex::Log::GetClientLogger()->error(__VA_ARGS__)
// #define APEX_LOG_CRITICAL(...)			::Apex::Log::GetClientLogger()->critical(__VA_ARGS__); __DEBUG_BREAK__;

#define APEX_LOG_TRACE(...)				SPD_LOGGER_TRACE(Apex::Log::GetClientLogger(), __VA_ARGS__)
#define APEX_LOG_DEBUG(...)				SPD_LOGGER_DEBUG(Apex::Log::GetClientLogger(), __VA_ARGS__)
#define APEX_LOG_INFO(...)				SPD_LOGGER_INFO(Apex::Log::GetClientLogger(), __VA_ARGS__)
#define APEX_LOG_WARN(...)				SPD_LOGGER_WARN(Apex::Log::GetClientLogger(), __VA_ARGS__)
#define APEX_LOG_ERROR(...)				SPD_LOGGER_ERROR(Apex::Log::GetClientLogger(), __VA_ARGS__)
#define APEX_LOG_CRITICAL(...)			SPD_LOGGER_CRITICAL(Apex::Log::GetClientLogger(), __VA_ARGS__); __DEBUG_BREAK__;
