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

    virtual void Init() = 0;
    virtual void Update(const StepTimer& _timer);

private:
    buma3d::util::Ptr<buma3d::IDeviceFactory>   factory;
    buma3d::util::Ptr<buma3d::IDeviceAdapter>   adapter;
    buma3d::util::Ptr<buma3d::IDevice>          device;

};


}// namespace buma