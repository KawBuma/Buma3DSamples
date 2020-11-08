#include "pch.h"
#include "DeviceResources.h"

namespace buma
{

namespace /*anonymous*/
{

std::string GetCurrentDir()
{
    char c_dir[MAX_PATH];
    memset(c_dir, 0, sizeof(c_dir));

    GetCurrentDirectoryA(MAX_PATH, c_dir);
    return std::string(c_dir);
}

void ConvertBackShashToSlash(std::string* _str)
{
    auto&& str = *_str;
    auto pos = str.find('\\');
    if (pos != std::string::npos)
    {
        while (pos != std::string::npos)
        {
            str[pos] = '/';
            pos = str.find('\\');
        }
    }
}

void ConvertSlashToBackShash(std::string* _str)
{
    auto&& str = *_str;
    auto pos = str.find('/');
    if (pos != std::string::npos)
    {
        while (pos != std::string::npos)
        {
            str[pos] = '\\';
            pos = str.find('/');
        }
    }
}

}

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

struct DeviceResources::B3D_PFN
{
    HMODULE                                     b3d_module;
    buma3d::PFN_Buma3DInitialize                Buma3DInitialize;
    buma3d::PFN_Buma3DGetInternalHeaderVersion  Buma3DGetInternalHeaderVersion;
    buma3d::PFN_Buma3DCreateDeviceFactory       Buma3DCreateDeviceFactory;
    buma3d::PFN_Buma3DUninitialize              Buma3DUninitialize;
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
    : pfn                   {}
    , type                  {}
    , factory               {}
    , adapter               {}
    , device                {}
    , cmd_queues            {}
    //, gpu_timer_pools     {}
    //, my_imugi            {}
    , console_session       {}
    , queue_props           {}
    , shader_laoder         {}
{

}

DeviceResources::~DeviceResources()
{
    WaitForGpu();
    UninitB3D();
}

bool DeviceResources::Init(INTERNAL_API_TYPE _type)
{
    if (!InitB3D(_type))        return false;
    if (!PickAdapter())         return false;
    if (!CreateDevice())        return false;
    if (!GetCommandQueues())    return false;
    if (!CreateMyImGui())       return false;
    shader_laoder = std::make_unique<shader::ShaderLoader>(type);

    return true;
}

bool DeviceResources::InitB3D(INTERNAL_API_TYPE _type)
{
    pfn = std::make_unique<B3D_PFN>();

    buma3d::ALLOCATOR_DESC desc{};
    desc.is_enable_allocator_debug = false;
    desc.custom_allocator          = nullptr;

    auto path = GetCurrentDir();
    ConvertBackShashToSlash(&path);

#ifdef _DEBUG
    const char* CONFIG = "Debug";
    const char* BUILD = "_Debug.dll";
#else
    const char* CONFIG = "Release";
    const char* BUILD = "_Release.dll";
#endif // _DEBUG
    path += "/External/Buma3D/Project/";

    switch (_type)
    {
    case buma::INTERNAL_API_TYPE_D3D12:
        path += "D3D12/v16/DLLBuild/";
        path += CONFIG;
        path += "/x64/Out/";
        path = path + "Buma3D_D3D12_DLL" + BUILD;
        ConvertSlashToBackShash(&path);
        pfn->b3d_module = LoadLibraryA(path.c_str());
        //pfn->b3d_module = LoadLibraryA((std::string("Buma3D_D3D12_DLL") + BUILD).c_str());
        break;

    case buma::INTERNAL_API_TYPE_VULKAN:
        path += "Vulkan/v16/DLLBuild/";
        path += CONFIG;
        path += "/x64/Out/";
        path = path + "Buma3D_Vulkan_DLL" + BUILD;
        ConvertSlashToBackShash(&path);
        pfn->b3d_module = LoadLibraryA(path.c_str());
        //pfn->b3d_module = LoadLibraryA((std::string("Buma3D_Vulkan_DLL") + BUILD).c_str());
        break;

    default:
        break;
    }
    assert(pfn->b3d_module != NULL);

    pfn->Buma3DInitialize               = (buma3d::PFN_Buma3DInitialize)              GetProcAddress(pfn->b3d_module, "Buma3DInitialize");
    pfn->Buma3DGetInternalHeaderVersion = (buma3d::PFN_Buma3DGetInternalHeaderVersion)GetProcAddress(pfn->b3d_module, "Buma3DGetInternalHeaderVersion");
    pfn->Buma3DCreateDeviceFactory      = (buma3d::PFN_Buma3DCreateDeviceFactory)     GetProcAddress(pfn->b3d_module, "Buma3DCreateDeviceFactory");
    pfn->Buma3DUninitialize             = (buma3d::PFN_Buma3DUninitialize)            GetProcAddress(pfn->b3d_module, "Buma3DUninitialize");

    auto bmr = pfn->Buma3DInitialize(desc);
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
        console_session = std::make_shared<ConsoleSession>();
        console_session->Begin();
        fac_desc.debug.debug_message_callback.user_data = console_session.get();
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
    auto bmr = pfn->Buma3DCreateDeviceFactory(fac_desc, &factory);
    if (bmr != buma3d::BMRESULT_SUCCEED)
        return false;

    // 高パフォーマンスアダプタを取得
    size_t cnt = 0;
    uint64_t max_vram = 0;
    buma3d::util::Ptr<buma3d::IDeviceAdapter> adapter_tmp{};
    while (factory->EnumAdapters(cnt++, &adapter_tmp) != buma3d::BMRESULT_FAILED_OUT_OF_RANGE)
    {
        auto&& desc = adapter_tmp->GetDesc();
        if (max_vram < desc.dedicated_video_memory)
            adapter = adapter_tmp;
    }

    return true;
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
    return true;
}

void DeviceResources::UninitB3D()
{
    if (!pfn || !pfn->b3d_module)
        return;

    //gpu_timer_pools.reset();
    //my_imugi.reset();

    for (auto& i_que : cmd_queues)
        for (auto& i : i_que)
            i.Reset();

    device.Reset();
    adapter.Reset();
    factory.Reset();

    console_session.reset();

    pfn->Buma3DUninitialize();

    FreeLibrary(pfn->b3d_module);
    pfn->b3d_module = NULL;
    pfn.reset();
}

bool DeviceResources::WaitForGpu()
{
    return device->WaitIdle() == buma3d::BMRESULT_SUCCEED;
}


}// namespace buma
