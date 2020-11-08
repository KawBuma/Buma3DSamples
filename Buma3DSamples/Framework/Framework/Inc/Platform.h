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

struct PLATFORM_DESC
{
    PLATFORM_TYPE   type;
    const void*     data;
};

class PlatformBase
{
public:
    PlatformBase();
    PlatformBase(const PlatformBase&) = delete;
    virtual ~PlatformBase();

    virtual void AttachApplication(std::shared_ptr<ApplicationBase> _app);
    virtual int MainLoop() = 0;

    std::shared_ptr<ApplicationBase> GetApplication()     { return app; }
    std::shared_ptr<DeviceResources> GetDeviceResources() { return device_resources; }
    std::shared_ptr<WindowBase>      GetWindow()          { return window; }

    virtual bool Init(const PLATFORM_DESC& _desc) = 0;
    virtual bool Term() = 0;

protected:
    virtual bool PrepareDeviceResources() = 0;
    virtual bool PrepareWindow() = 0;

protected:
    std::shared_ptr<ApplicationBase>    app;
    std::shared_ptr<DeviceResources>    device_resources;
    std::shared_ptr<WindowBase>         window;


};


}// namespace buma
