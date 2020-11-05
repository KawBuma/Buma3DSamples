#pragma once

namespace buma
{

class PlatformBase
{
public:
    PlatformBase();
    PlatformBase(const PlatformBase&) = delete;
    virtual ~PlatformBase();

    virtual void AttachApplication(std::shared_ptr<ApplicationBase> _app);
    virtual int MainLoop() = 0;

    std::shared_ptr<ApplicationBase> GetApplication()     const { return app; }
    std::shared_ptr<DeviceResources> GetDeviceResources() const { return device_resources; }
    std::shared_ptr<WindowBase>      GetWindow()          const { return window; }

protected:
    virtual bool Prepare() = 0;
    virtual bool PrepareDeviceResources() = 0;
    virtual bool PrepareWindow() = 0;

protected:
    std::shared_ptr<ApplicationBase>    app;
    std::shared_ptr<DeviceResources>    device_resources;
    std::shared_ptr<WindowBase>         window;


};


}// namespace buma
