#include "pch.h"
#include "DeviceResources.h"

namespace buma
{

namespace /*anonymous*/
{

std::string* GetCurrentDir(std::string* _result)
{
    _result->resize(size_t(GetCurrentDirectoryA(0, nullptr) - 1), '\0');
    GetCurrentDirectoryA((DWORD)_result->size() + 1, _result->data());
    return _result;
}

void ConvertBackShashToSlash(std::string* _str)
{
    auto&&  str         = *_str;
    auto    srd_data    = str.data();
    auto    pos         = str.find('\\');
    if (pos != std::string::npos) {
        while (pos != std::string::npos) {
            srd_data[pos] = '/';
            pos = str.find('\\');
        }
    }
}

void ConvertSlashToBackShash(std::string* _str)
{
    auto&&  str         = *_str;
    auto    srd_data    = str.data();
    auto    pos         = str.find('/');
    if (pos != std::string::npos) {
        while (pos != std::string::npos) {
            srd_data[pos] = '\\';
            pos = str.find('/');
        }
    }
}

}// namespace /*anonymous*/


struct DeviceResources::B3D_PFN
{
    HMODULE                                     b3d_module;
    buma3d::PFN_Buma3DInitialize                Buma3DInitialize;
    buma3d::PFN_Buma3DGetInternalHeaderVersion  Buma3DGetInternalHeaderVersion;
    buma3d::PFN_Buma3DCreateDeviceFactory       Buma3DCreateDeviceFactory;
    buma3d::PFN_Buma3DUninitialize              Buma3DUninitialize;
};

DeviceResources::DeviceResources()
    : desc                                              {}
    , pfn                                               {}
    , factory                                           {}
    , adapter                                           {}
    , device                                            {}
    , cmd_queues                                        {}         
    //, gpu_timer_pools                                   {}    
    //, my_imugi                                          {}
    , shader_laoder                                     {}
    , resource_heaps_allocator                          {}
    , resource_create                                   {}
    , limits                                            {}
    , resource_heap_props                               {}
    , queue_props                                       {}
{

}

DeviceResources::~DeviceResources()
{
    WaitForGpu();
    resource_create          .reset();
    resource_heaps_allocator .reset();
    resource_heap_props      .reset();
    shader_laoder            .reset();
    UninitB3D();
}

bool DeviceResources::Init(const DEVICE_RESOURCE_DESC& _desc)
{
    desc = _desc;
    if (!InitB3D(desc.type, desc.library_dir.c_str()))  return false;
    if (!PickAdapter())                                 return false;
    if (!CreateDevice())                                return false;
    if (!GetCommandQueues())                            return false;
    if (!CreateMyImGui())                               return false;

    resource_heap_props      = std::make_shared<ResourceHeapProperties>(device.Get());
    shader_laoder            = std::make_unique<shader::ShaderLoader>(desc.type);
    resource_heaps_allocator = std::make_unique<ResourceHeapsAllocator>(adapter.Get(), device.Get());
    resource_create          = std::make_unique<res::ResourceCreate>(shared_from_this());

    return true;
}

bool DeviceResources::InitB3D(INTERNAL_API_TYPE _type, const char* _library_dir)
{
    pfn = std::make_unique<B3D_PFN>();

    buma3d::ALLOCATOR_DESC b3d_desc{};
    b3d_desc.is_enabled_allocator_debug = false;
    b3d_desc.custom_allocator           = nullptr;

    std::string path;
    ConvertBackShashToSlash(GetCurrentDir(&path));

#ifdef _DEBUG
    const char* CONFIG = "Debug";
    const char* BUILD = "_Debug.dll";
#else
    const char* CONFIG = "Release";
    const char* BUILD = "_Release.dll";
#endif // _DEBUG

    if (strlen(_library_dir) != 0)
        path += _library_dir;
    else
        path += "/External/Buma3D/Project/";

    // dllを読み込む
    switch (_type)
    {
    case buma::INTERNAL_API_TYPE_D3D12:
        if (strlen(_library_dir) == 0)
        {
            path += "D3D12/v16/DLLBuild/";
            path += CONFIG;
            path += "/x64/Out/";
        }
        path = path + "Buma3D_D3D12_DLL" + BUILD;
        ConvertSlashToBackShash(&path);
        pfn->b3d_module = LoadLibraryA(path.c_str());
        break;

    case buma::INTERNAL_API_TYPE_VULKAN:
        if (strlen(_library_dir) == 0)
        {
            path += "Vulkan/v16/DLLBuild/";
            path += CONFIG;
            path += "/x64/Out/";
        }
        path = path + "Buma3D_Vulkan_DLL" + BUILD;
        ConvertSlashToBackShash(&path);
        pfn->b3d_module = LoadLibraryA(path.c_str());
        break;

    default:
        break;
    }
    assert(pfn->b3d_module != NULL);

    pfn->Buma3DInitialize               = (buma3d::PFN_Buma3DInitialize)              GetProcAddress(pfn->b3d_module, "Buma3DInitialize");
    pfn->Buma3DGetInternalHeaderVersion = (buma3d::PFN_Buma3DGetInternalHeaderVersion)GetProcAddress(pfn->b3d_module, "Buma3DGetInternalHeaderVersion");
    pfn->Buma3DCreateDeviceFactory      = (buma3d::PFN_Buma3DCreateDeviceFactory)     GetProcAddress(pfn->b3d_module, "Buma3DCreateDeviceFactory");
    pfn->Buma3DUninitialize             = (buma3d::PFN_Buma3DUninitialize)            GetProcAddress(pfn->b3d_module, "Buma3DUninitialize");

    auto bmr = pfn->Buma3DInitialize(b3d_desc);
    assert(bmr == buma3d::BMRESULT_SUCCEED);
    return bmr == buma3d::BMRESULT_SUCCEED;
}

bool DeviceResources::PickAdapter()
{
    // ファクトリ作成
    buma3d::DEVICE_FACTORY_DESC fac_desc{};

    fac_desc.flags              = buma3d::DEVICE_FACTORY_FLAG_NONE;
    fac_desc.debug.is_enabled   = desc.is_enabled_debug;

    if (fac_desc.debug.is_enabled)
    {
        fac_desc.debug.debug_message_callback.user_data = desc.message_logger.get();
        fac_desc.debug.debug_message_callback.Callback  = desc.DebugMessageCallback;
    }
    buma3d::DEBUG_MESSAGE_DESC descs[buma3d::DEBUG_MESSAGE_SEVERITY_END]{};
    for (size_t i = 0; i < buma3d::DEBUG_MESSAGE_SEVERITY_END; i++)
    {
        auto&& debug_desc = descs[i];
        debug_desc.is_enabled_debug_break = false;// レポート時のブレイク
        debug_desc.severity = buma3d::DEBUG_MESSAGE_SEVERITY(i);
        debug_desc.category_flags = buma3d::DEBUG_MESSAGE_CATEGORY_FLAG_ALL;
        if (debug_desc.severity == buma3d::DEBUG_MESSAGE_SEVERITY_ERROR)
            debug_desc.is_enabled_debug_break = false;// レポート時のブレイク
    }
    fac_desc.debug.num_debug_messages             = ARRAYSIZE(descs);
    fac_desc.debug.debug_messages                 = descs;
    fac_desc.debug.gpu_based_validation.is_enabled = false;// GPU検証
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
        auto&& adapter_desc = adapter_tmp->GetDesc();
        if (max_vram < adapter_desc.dedicated_video_memory)
        {
            max_vram = adapter_desc.dedicated_video_memory;
            adapter = adapter_tmp;
        }
    }
    if (!adapter)
        return false;

    adapter->GetDeviceAdapterLimits(&limits);
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
        while (device->GetCommandQueue(buma3d::COMMAND_TYPE(i), cnt++, &ptr) == buma3d::BMRESULT_SUCCEED)
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

    pfn->Buma3DUninitialize();

    FreeLibrary(pfn->b3d_module);
    pfn->b3d_module = NULL;
    pfn.reset();
}

bool DeviceResources::WaitForGpu()
{
    if (device)
        return device->WaitIdle() == buma3d::BMRESULT_SUCCEED;

    return true;
}


}// namespace buma
