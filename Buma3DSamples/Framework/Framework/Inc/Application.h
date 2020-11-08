#pragma once

namespace buma
{

class PlatformBase;

class ApplicationBase
{
public:
    ApplicationBase();
    ApplicationBase(const ApplicationBase&) = delete;
    virtual ~ApplicationBase();

    std::shared_ptr<DeviceResources> GetDeviceResources() const { return dr; }

    virtual void Prepare(PlatformBase& _platform) = 0;

    virtual void Init() = 0;
    virtual void Tick() = 0;
    virtual void Term() = 0;

protected:
    std::shared_ptr<DeviceResources> dr;

};


}// namespace buma
