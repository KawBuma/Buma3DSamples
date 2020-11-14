#include "pch.h"
#include "Logger.h"
#include "LoggerWindows.h"
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include "spdlog/sinks/basic_file_sink.h"
#include <filesystem>

namespace buma
{
namespace debug
{

LoggerWindows::LoggerWindows(bool _is_enable)
    : is_enable{ _is_enable }
{
    if (is_enable)
    {
        if (!std::filesystem::exists("./Log"))
            std::filesystem::create_directory("./Log");
        async_file = spdlog::basic_logger_mt<spdlog::default_factory>("buma::LoggerWindows", "./Log/Log.txt", /*truncate*/true);
    }
}

LoggerWindows::~LoggerWindows()
{
    async_file.reset();
}

void LoggerWindows::Log(LOG_TYPE _type, const char* _msg)
{
    if (!async_file)
        return;

    const char* type = "[unknown] ";
    switch (_type)
    {
    case buma::debug::LOG_TYPE_INFO     : async_file->info    (_msg); type = "[info] ";     break;
    case buma::debug::LOG_TYPE_DEBUG    : async_file->trace   (_msg); type = "[trace] ";    break;
    case buma::debug::LOG_TYPE_WARNING  : async_file->warn    (_msg); type = "[warn] ";     break;
    case buma::debug::LOG_TYPE_ERROR    : async_file->error   (_msg); type = "[error] ";    break;
    case buma::debug::LOG_TYPE_CRITICAL : async_file->critical(_msg); type = "[critical] "; break;
        
    default:
        break;
    }
    std::cout << type << _msg << std::endl;
}


}// namespace debug
}// namespace buma
