#pragma once

namespace spdlog
{

class logger;

}// namespace spdlog

namespace buma
{
namespace debug
{

class LoggerWindows : public ILogger
{
public:
    LoggerWindows();
    virtual ~LoggerWindows();

    void Log(LOG_TYPE _type, const char* _msg) override;

private:
    std::shared_ptr<spdlog::logger> async_file;

};


}// namespace debug
}// namespace buma
