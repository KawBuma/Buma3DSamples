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
    LoggerWindows(bool _is_enable);
    virtual ~LoggerWindows();

    void Log(LOG_TYPE _type, const char* _msg) override;

private:
    bool is_enable;
    std::shared_ptr<spdlog::logger> async_file;

};


}// namespace debug
}// namespace buma
