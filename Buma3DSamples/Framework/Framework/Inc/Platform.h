#pragma once

namespace buma
{

class WindowBase;
class ApplicationBase;

enum PLATFORM_TYPE
{
    PLATFORM_TYPE_WINDOWS
};

struct PLATFORM_DATA_WINDOWS
{
    const void*     hInstance;
    const void*     hPrevInstance;
    const wchar_t*  lpCmdLine;
    int             nCmdShow;
};

struct WINDOW_DESC
{
    uint32_t    width;
    uint32_t    height;
    const char* name;
};

struct PLATFORM_DESC
{
    PLATFORM_TYPE   type;
    const void*     data;
    WINDOW_DESC     window_desc;
};

class PlatformBase
{
public:
    PlatformBase();
    PlatformBase(const PlatformBase&) = delete;
    virtual ~PlatformBase();

    virtual void AttachApplication(std::shared_ptr<ApplicationBase> _app);
    virtual int MainLoop() = 0;

    std::shared_ptr<ApplicationBase>                    GetApplication()     const { return app; }
    std::shared_ptr<DeviceResources>                    GetDeviceResources() const { return device_resources; }
    std::shared_ptr<WindowBase>                         GetWindow()          const { return window; }
    const std::vector<std::unique_ptr<std::string>>&    GetCommandLines()    const { return cmd_lines; }
    virtual const StepTimer*                            GetStepTimer()       const { return &timer; }
    bool                                                IsPrepared()         const { return is_prepared; }

    virtual bool Init(const PLATFORM_DESC& _desc) = 0;
    virtual bool Term() = 0;

    virtual debug::ILogger* GetLogger() = 0;


protected:
    virtual bool ParseCommandLines(const PLATFORM_DESC& _desc) = 0;
    virtual bool PrepareDeviceResources() = 0;
    virtual bool PrepareWindow(const WINDOW_DESC& _desc) = 0;

protected:
    std::vector<std::unique_ptr<std::string>>   cmd_lines;
    std::shared_ptr<ApplicationBase>            app;
    std::shared_ptr<DeviceResources>            device_resources;
    std::shared_ptr<WindowBase>                 window;
    StepTimer                                   timer;
    bool                                        is_prepared;

};


}// namespace buma
