#include "pch.h"
#include "Logger.h"
#include "LoggerWindows.h"
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include "spdlog/sinks/basic_file_sink.h"

namespace buma
{
namespace debug
{

LoggerWindows::LoggerWindows()
{
    async_file = spdlog::basic_logger_mt<spdlog::async_factory>("buma::debug::LoggerWindows::async_file", "./Log/Log.txt");
}

LoggerWindows::~LoggerWindows()
{
    async_file.reset();
}

void LoggerWindows::Log(LOG_TYPE _type, const char* _msg)
{
    switch (_type)
    {
    case buma::debug::LOG_TYPE_INFO     : async_file->info    (_msg); break;
    case buma::debug::LOG_TYPE_DEBUG    : async_file->trace   (_msg); break;
    case buma::debug::LOG_TYPE_WARNING  : async_file->warn    (_msg); break;
    case buma::debug::LOG_TYPE_ERROR    : async_file->error   (_msg); break;
    case buma::debug::LOG_TYPE_CRITICAL : async_file->critical(_msg); break;
        
    default:
        break;
    }
    std::cout << _msg << std::endl;
}


}// namespace debug
}// namespace buma
