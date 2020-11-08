#pragma once
//#include <MyImgui.h>

// プラットフォーム: アプリケーションを起動する。
// アプリケーション: プラットフォームに対して必要な情報を与え、ウィンドウ等を準備してもらう。
// ウィンドウ      : プラットフォーム固有の画面の生成と処理(ウィンドウプロシージャなど)を実行する。 
// フレームワーク  : これらを統括しユーザーに提供する。

namespace buma
{

namespace shader
{
class ShaderLoader;
}

enum INTERNAL_API_TYPE
{
    INTERNAL_API_TYPE_D3D12 = 1,
    INTERNAL_API_TYPE_VULKAN = 2
};

class ConsoleSession;

class DeviceResources
{
public:
    DeviceResources();
    ~DeviceResources();

    bool Init(INTERNAL_API_TYPE _type);

public:
    const buma3d::util::Ptr<buma3d::IDeviceFactory>&                GetFactory()                                    const { return factory; }
    const buma3d::util::Ptr<buma3d::IDeviceAdapter>&                GetAdapter()                                    const { return adapter; }
    const buma3d::util::Ptr<buma3d::IDevice>&                       GetDevice()                                     const { return device; }
    const std::vector<buma3d::util::Ptr<buma3d::ICommandQueue>>&    GetCommandQueues(buma3d::COMMAND_TYPE _type)    const { return cmd_queues[_type]; }

    const std::unique_ptr<shader::ShaderLoader>& GetShaderLoader() { return shader_laoder; }

    // const std::vector<std::shared_ptr<buma::GpuTimerPool>>&      GetGpuTimerPool(const buma3d::COMMAND_TYPE _type)   const { return direct_gpu_timer_pools[_type]; }
    // std::shared_ptr<buma::MyImgui>                               GetMyImGui()                                        const { return my_imugi; }

    bool WaitForGpu();

protected:
    static void B3D_APIENTRY B3DMessageCallback(buma3d::DEBUG_MESSAGE_SEVERITY _sev, buma3d::DEBUG_MESSAGE_CATEGORY_FLAG _category, const buma3d::Char8T* const _msg, void* _user_data);

protected:
    bool InitB3D(INTERNAL_API_TYPE _type);
    bool PickAdapter();
    bool CreateDevice();
    bool GetCommandQueues();
    bool CreateMyImGui();
    void UninitB3D();

private:
    struct B3D_PFN
    {
        buma3d::PFN_Buma3DInitialize                Buma3DInitialize;
        buma3d::PFN_Buma3DGetInternalHeaderVersion  Buma3DGetInternalHeaderVersion;
        buma3d::PFN_Buma3DCreateDeviceFactory       Buma3DCreateDeviceFactory;
        buma3d::PFN_Buma3DUninitialize              Buma3DUninitialize;
    };

private:
    HMODULE                                                 b3d_module;
    B3D_PFN                                                 pfn;
    INTERNAL_API_TYPE                                       type;
    buma3d::util::Ptr<buma3d::IDeviceFactory>               factory;
    buma3d::util::Ptr<buma3d::IDeviceAdapter>               adapter;
    buma3d::util::Ptr<buma3d::IDevice>                      device;
    std::vector<buma3d::util::Ptr<buma3d::ICommandQueue>>   cmd_queues[buma3d::COMMAND_TYPE_NUM_TYPES];         // [COMMAND_TYPE]
    //std::vector<std::shared_ptr<buma::GpuTimerPool>>      gpu_timer_pools[buma3d::COMMAND_TYPE_NUM_TYPES];    // [COMMAND_TYPE]
    //std::shared_ptr<buma::MyImGui>                        my_imugi;
    std::shared_ptr<ConsoleSession>                         console_session;
    std::unique_ptr<shader::ShaderLoader>                   shader_laoder;

    std::vector<buma3d::COMMAND_QUEUE_PROPERTIES> queue_props;

};


}// namespace buma
