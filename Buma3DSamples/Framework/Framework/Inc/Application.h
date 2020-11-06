#pragma once

namespace buma
{

class ApplicationBase
{
public:
    ApplicationBase();
    ApplicationBase(const ApplicationBase&) = delete;
    virtual ~ApplicationBase();

    buma3d::util::Ptr<buma3d::IDeviceFactory>   GetDeviceFactory() const { return factory; }
    buma3d::util::Ptr<buma3d::IDeviceAdapter>   GetDeviceAdapter() const { return adapter; }
    buma3d::util::Ptr<buma3d::IDevice>          GetDevice       () const { return device ; }

    virtual void Prepare(PlatformBase& _platform) = 0;

    virtual void Init() = 0;
    virtual void Tick() = 0;

protected:
    std::shared_ptr<DeviceResources>            dr;
    buma3d::util::Ptr<buma3d::IDeviceFactory>   factory;
    buma3d::util::Ptr<buma3d::IDeviceAdapter>   adapter;
    buma3d::util::Ptr<buma3d::IDevice>          device;

};


}// namespace buma
