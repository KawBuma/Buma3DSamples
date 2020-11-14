#pragma once
//#include <MyImgui.h>

// プラットフォーム: アプリケーションを起動する。
// アプリケーション: プラットフォームに対して必要な情報を与え、ウィンドウ等を準備してもらう。
// ウィンドウ      : プラットフォーム固有の画面の生成と処理(ウィンドウプロシージャなど)を実行する。 
// フレームワーク  : これらを統括しユーザーに提供する。

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

enum INTERNAL_API_TYPE
{
    INTERNAL_API_TYPE_D3D12 = 1,
    INTERNAL_API_TYPE_VULKAN = 2
};

struct DEVICE_RESOURCE_DESC
{
    INTERNAL_API_TYPE                       type;
    std::string                             library_dir;
    bool                                    is_enable_debug;
    std::shared_ptr<buma::debug::ILogger>   message_logger;
    buma3d::PFN_Buma3DDebugMessageCallback  DebugMessageCallback;
};

class DeviceResources
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

    const std::unique_ptr<shader::ShaderLoader>& GetShaderLoader() { return shader_laoder; }

    const buma3d::RESOURCE_HEAP_PROPERTIES* FindHeapIndex(buma3d::RESOURCE_HEAP_PROPERTY_FLAGS _flags) const;
    const std::vector<buma3d::RESOURCE_HEAP_PROPERTIES>& GetResourceHeapProperties() const { return resource_heap_props; }
    const std::vector<buma3d::COMMAND_QUEUE_PROPERTIES>& GetQueueProperties()        const { return queue_props; }

    // const std::vector<std::shared_ptr<buma::GpuTimerPool>>&      GetGpuTimerPool(const buma3d::COMMAND_TYPE _type)   const { return direct_gpu_timer_pools[_type]; }
    // std::shared_ptr<buma::MyImgui>                               GetMyImGui()                                        const { return my_imugi; }

    bool WaitForGpu();

protected:
    bool InitB3D(INTERNAL_API_TYPE _type, const char* _library_dir);
    bool PickAdapter();
    bool CreateDevice();
    bool GetCommandQueues();
    bool CreateMyImGui();
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
    //std::shared_ptr<buma::MyImGui>                        my_imugi;
    std::unique_ptr<shader::ShaderLoader>                   shader_laoder;

    std::vector<buma3d::RESOURCE_HEAP_PROPERTIES>           resource_heap_props;
    std::vector<buma3d::COMMAND_QUEUE_PROPERTIES>           queue_props;

};


}// namespace buma
