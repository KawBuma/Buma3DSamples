#pragma once

namespace buma
{
namespace debug
{

enum LOG_TYPE
{
    LOG_TYPE_INFO,
    LOG_TYPE_DEBUG,
    LOG_TYPE_WARNING,
    LOG_TYPE_ERROR,
    LOG_TYPE_CRITICAL,
};

struct ILogger
{
public:
    virtual ~ILogger() {}

    virtual void Log(LOG_TYPE _type, const char* _msg) = 0;
    void LogInfo     (const char* _msg) { Log(LOG_TYPE_INFO    , _msg); }
    void LogDebug    (const char* _msg) { Log(LOG_TYPE_DEBUG   , _msg); }
    void LogWarn     (const char* _msg) { Log(LOG_TYPE_WARNING , _msg); }
    void LogError    (const char* _msg) { Log(LOG_TYPE_ERROR   , _msg); }
    void LogCritical (const char* _msg) { Log(LOG_TYPE_CRITICAL, _msg); }

};


}// namespace debug
}// namespace buma
