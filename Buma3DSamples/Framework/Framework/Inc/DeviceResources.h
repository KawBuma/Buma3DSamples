#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Buma3D.h"
#include "Util/Buma3DPtr.h"
#include "ResourceHeapProperties.h"

namespace buma
{

namespace debug
{
struct ILogger;
}

namespace shader
{
class ShaderLoader;
}

namespace res
{
class ResourceCreate;
}

class ResourceHeapProperties;
class ResourceHeapsAllocator;



enum INTERNAL_API_TYPE
{
    INTERNAL_API_TYPE_D3D12 = 1,
    INTERNAL_API_TYPE_VULKAN = 2
};

struct DEVICE_RESOURCE_DESC
{
    INTERNAL_API_TYPE                       type;
    std::string                             library_dir;
    bool                                    is_enabled_debug;
    std::shared_ptr<buma::debug::ILogger>   message_logger;
    buma3d::PFN_Buma3DDebugMessageCallback  DebugMessageCallback;
};

class DeviceResources : public std::enable_shared_from_this<DeviceResources>
{
public:
    DeviceResources();
    ~DeviceResources();

    bool Init(const DEVICE_RESOURCE_DESC& _desc);

public:
    const buma3d::util::Ptr<buma3d::IDeviceFactory>&                GetFactory()                                    const { return factory; }
    const buma3d::util::Ptr<buma3d::IDeviceAdapter>&                GetAdapter()                                    const { return adapter; }
    const buma3d::util::Ptr<buma3d::IDevice>&                       GetDevice()                                     const { return device; }
    const std::vector<buma3d::util::Ptr<buma3d::ICommandQueue>>&    GetCommandQueues(buma3d::COMMAND_TYPE _type)    const { return cmd_queues[_type]; }
    ResourceHeapsAllocator*                                         GetResourceHeapsAllocator()                     const { return resource_heaps_allocator.get(); }
    res::ResourceCreate*                                            GetResourceCreate()                             const { return resource_create.get(); }

    const std::unique_ptr<shader::ShaderLoader>&                    GetShaderLoader()                                         { return shader_laoder; }
    const buma3d::DEVICE_ADAPTER_LIMITS&                            GetDeviceAdapterLimits()                            const { return limits; }
    const std::shared_ptr<ResourceHeapProperties>&                  GetResourceHeapProperties()                         const { return resource_heap_props; }
    const std::vector<buma3d::COMMAND_QUEUE_PROPERTIES>&            GetQueueProperties()                                const { return queue_props; }
    //const std::vector<std::shared_ptr<buma::GpuTimerPool>>&         GetGpuTimerPool(const buma3d::COMMAND_TYPE _type)   const { return direct_gpu_timer_pools[_type]; }

    bool WaitForGpu();

protected:
    bool InitB3D(INTERNAL_API_TYPE _type, const char* _library_dir);
    bool PickAdapter();
    bool CreateDevice();
    bool GetCommandQueues();
    void UninitB3D();

private:
    DEVICE_RESOURCE_DESC                                    desc;
    struct B3D_PFN;
    std::unique_ptr<B3D_PFN>                                pfn;
    buma3d::util::Ptr<buma3d::IDeviceFactory>               factory;
    buma3d::util::Ptr<buma3d::IDeviceAdapter>               adapter;
    buma3d::util::Ptr<buma3d::IDevice>                      device;
    std::vector<buma3d::util::Ptr<buma3d::ICommandQueue>>   cmd_queues[buma3d::COMMAND_TYPE_NUM_TYPES];         // [COMMAND_TYPE]
    //std::vector<std::shared_ptr<buma::GpuTimerPool>>      gpu_timer_pools[buma3d::COMMAND_TYPE_NUM_TYPES];    // [COMMAND_TYPE]
    std::unique_ptr<shader::ShaderLoader>                   shader_laoder;
    std::unique_ptr<ResourceHeapsAllocator>                 resource_heaps_allocator;
    std::unique_ptr<res::ResourceCreate>                    resource_create;

    buma3d::DEVICE_ADAPTER_LIMITS                           limits;
    std::shared_ptr<ResourceHeapProperties>                 resource_heap_props;
    std::vector<buma3d::COMMAND_QUEUE_PROPERTIES>           queue_props;


};


}// namespace buma
