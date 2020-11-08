#include "pch.h"
#include "DeviceResources.h"

namespace buma
{

class ConsoleSession
{
public:
    ConsoleSession()
        : ofs("./log.txt", std::ios::out | std::ios::trunc, std::ios::_Default_open_prot)
    {
    }

    ~ConsoleSession()
    {
        End();
    }

    void Begin()
    {
        auto res = AllocConsole();
        assert(res);

        // coutにする
        auto console_out = "CONOUT$";
        freopen_s(&stream, console_out, "w+", stdout);
    }

    void End()
    {
        auto res = FreeConsole();
        assert(res != 0);
        fclose(stream);
        ofs.close();
    }

    std::ofstream ofs;
private:
    FILE*         stream;

};


void B3D_APIENTRY DeviceResources::B3DMessageCallback(buma3d::DEBUG_MESSAGE_SEVERITY _sev, buma3d::DEBUG_MESSAGE_CATEGORY_FLAG _category, const buma3d::Char8T* const _msg, void* _user_data)
{
    static const char* SEVERITIES[]
    {
       "[ INFO"
     , "[ WARNING"
     , "[ ERROR"
     , "[ CORRUPTION"
     , "[ OTHER"
    };

    static const char* CATEGORIES[]
    {
       ", UNKNOWN ] "
     , ", MISCELLANEOUS ] "
     , ", INITIALIZATION ] "
     , ", CLEANUP ] "
     , ", COMPILATION ] "
     , ", STATE_CREATION ] "
     , ", STATE_SETTING ] "
     , ", STATE_GETTING ] "
     , ", RESOURCE_MANIPULATION ] "
     , ", EXECUTION ] "
     , ", SHADER ] "
     , ", B3D ] "
     , ", B3D_DETAILS ] "
    };

    ConsoleSession* session = (ConsoleSession*)(_user_data);

    auto&& o = session->ofs;
    auto E = [&]()
    {
        o << std::endl;
        std::cout << std::endl;
    };
    auto P = [&](const auto& s)
    {
        o << s;
        std::cout << s;
    };


    if (_sev == buma3d::DEBUG_MESSAGE_SEVERITY_ERROR)
        P("\n\n");
    else if (_sev == buma3d::DEBUG_MESSAGE_SEVERITY_WARNING)
        P("\n");

    P(SEVERITIES[_sev]);

    DWORD i = 0;
    if (_BitScanForward(&i, _category))
        P(CATEGORIES[i]);

    if (_sev == buma3d::DEBUG_MESSAGE_SEVERITY_ERROR)
        P("\n\n");
    else if (_sev == buma3d::DEBUG_MESSAGE_SEVERITY_WARNING)
        P("\n");

    P(_msg);
    E();
}

DeviceResources::DeviceResources()
    : device                {}
    , cmd_queues            {}
    , gpu_wait_fence        {}
    , gpu_wait_fence_val    {}
    , fence_submit_desc     {}
    //, gpu_timer_pools       {}
    //, my_imugi              {}
{

}

DeviceResources::~DeviceResources()
{
    WaitForGpu();
}

bool DeviceResources::Init()
{
    if (!InitB3D())             return false;
    if (!PickAdapter())         return false;
    if (!CreateDevice())        return false;
    if (!GetCommandQueues())    return false;
    if (!CreateMyImGui())       return false;

    return true;
}

bool DeviceResources::InitB3D()
{
    buma3d::ALLOCATOR_DESC desc{};
    desc.is_enable_allocator_debug = false;
    desc.custom_allocator          = nullptr;

    auto bmr = buma3d::Buma3DInitialize(desc);
    return bmr == buma3d::BMRESULT_SUCCEED;
}

bool DeviceResources::PickAdapter()
{
    // ファクトリ作成
    buma3d::DEVICE_FACTORY_DESC fac_desc{};

    fac_desc.flags           = buma3d::DEVICE_FACTORY_FLAG_NONE;
    fac_desc.debug.is_enable = true; // デバッグレポート

    if (fac_desc.debug.is_enable)
    {
        fac_desc.debug.debug_message_callback.user_data = (console_session = std::make_shared<ConsoleSession>()).get();
        fac_desc.debug.debug_message_callback.Callback  = B3DMessageCallback;
    }
    buma3d::DEBUG_MESSAGE_DESC descs[buma3d::DEBUG_MESSAGE_SEVERITY_END]{};
    for (size_t i = 0; i < buma3d::DEBUG_MESSAGE_SEVERITY_END; i++)
    {
        auto&& desc = descs[i];
        desc.is_enable_debug_break = false;// レポート時のブレイク
        desc.severity              = buma3d::DEBUG_MESSAGE_SEVERITY(i);
        desc.category_flags        = buma3d::DEBUG_MESSAGE_CATEGORY_FLAG_ALL;
        if (desc.severity == buma3d::DEBUG_MESSAGE_SEVERITY_ERROR)
            desc.is_enable_debug_break = false;// レポート時のブレイク
    }
    fac_desc.debug.num_debug_messages             = ARRAYSIZE(descs);
    fac_desc.debug.debug_messages                 = descs;
    fac_desc.debug.gpu_based_validation.is_enable = false;// GPU検証
    fac_desc.debug.gpu_based_validation.flags     = buma3d::GPU_BASED_VALIDATION_FLAG_NONE;

    // 作成
    auto bmr = buma3d::Buma3DCreateDeviceFactory(fac_desc, &factory);
    if (bmr != buma3d::BMRESULT_SUCCEED)
        return false;

    bmr = factory->EnumAdapters(0, &adapter);
    return bmr == buma3d::BMRESULT_SUCCEED;
}

bool DeviceResources::CreateDevice()
{
    // デバイス作成
    buma3d::DEVICE_DESC dd{};
    dd.adapter = adapter.Get();

    // コマンドキューの情報を取得。
    auto num_queue_tyeps = dd.adapter->GetCommandQueueProperties(nullptr);// サイズを返します。
    queue_props.resize(num_queue_tyeps);
    adapter->GetCommandQueueProperties(queue_props.data());

    // コマンドキュー作成情報を構成
    std::vector<buma3d::COMMAND_QUEUE_CREATE_DESC>           queue_descs     (num_queue_tyeps);
    std::vector<std::vector<buma3d::COMMAND_QUEUE_PRIORITY>> queue_priorities(num_queue_tyeps);
    std::vector<std::vector<buma3d::NodeMask>>               queue_node_masks(num_queue_tyeps);
    for (uint32_t i = 0; i < num_queue_tyeps; i++)
    {
        auto&& qd = queue_descs[i];
        qd.type       = queue_props[i].type;
        qd.flags      = buma3d::COMMAND_QUEUE_FLAG_NONE;
        qd.num_queues = std::min(std::thread::hardware_concurrency(), queue_props[i].num_max_queues);

        auto&& qps  = queue_priorities[i];
        auto&& qnms = queue_node_masks[i];
        qps .resize(qd.num_queues, buma3d::COMMAND_QUEUE_PRIORITY_DEFAULT);
        qnms.resize(qd.num_queues, buma3d::B3D_DEFAULT_NODE_MASK);
        qd.priorities = qps.data();
        qd.node_masks = qnms.data();
    }
    dd.num_queue_create_descs   = (uint32_t)queue_descs.size();
    dd.queue_create_descs       = queue_descs.data();
    dd.flags                    = buma3d::DEVICE_FLAG_NONE;

    auto bmr = factory->CreateDevice(dd, &device);
    return bmr == buma3d::BMRESULT_SUCCEED;
}

bool DeviceResources::GetCommandQueues()
{
    for (uint32_t i = 0; i < uint32_t(buma3d::COMMAND_TYPE_NUM_TYPES); i++)
    {
        buma3d::util::Ptr<buma3d::ICommandQueue> ptr;
        uint32_t                                 cnt = 0;
        auto&&                                   queues = cmd_queues[i];
        while (device->GetCommandQueue(buma3d::COMMAND_TYPE(i), cnt++, &ptr) != buma3d::BMRESULT_FAILED_OUT_OF_RANGE)
            queues.emplace_back(ptr);
    }
    return true;
}

bool DeviceResources::CreateMyImGui()
{
    // TODO: ImGui
    return false;
}

bool DeviceResources::WaitForGpu()
{
    // 全てのコマンドキューの実行を待機する。
    bool result = true;
    for (size_t i = 0; i < size_t(buma3d::COMMAND_TYPE_NUM_TYPES); i++)
    {        
        for (auto& i_que : cmd_queues[i])
        {
            fence_submit_desc.Reset();
            fence_submit_desc.AddFence(gpu_wait_fence.Get(), gpu_wait_fence_val.signal());
            i_que->SubmitSignal(fence_submit_desc.GetAsSignal());

            ++gpu_wait_fence_val;
            auto bmr = gpu_wait_fence->Wait(gpu_wait_fence_val.wait(), UINT32_MAX);

            if (bmr != buma3d::BMRESULT_SUCCEED)
                result = false;
        }
    }
    return result;
}


}// namespace buma
