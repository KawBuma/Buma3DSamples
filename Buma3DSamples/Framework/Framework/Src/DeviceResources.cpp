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

    return true;
}

bool DeviceResources::InitB3D()
{
    return false;
}

bool DeviceResources::PickAdapter()
{

    return true;
}

bool DeviceResources::CreateDevice()
{

    return true;
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
        for (auto& i : cmd_queues[i])
        {
            fence_submit_desc.Reset();
            fence_submit_desc.AddFence(gpu_wait_fence.Get(), gpu_wait_fence_val.signal());
            i->SubmitSignal(fence_submit_desc.GetAsSignal());

            ++gpu_wait_fence_val;
            auto bmr = gpu_wait_fence->Wait(gpu_wait_fence_val.wait(), UINT32_MAX);

            if (bmr != buma3d::BMRESULT_SUCCEED)
                result = false;
        }
    }
    return result;
}


}// namespace buma
