#include "pch.h"
#include "PlatformWindows.h"

#include <shellapi.h>

#include <memory>

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

void B3D_APIENTRY PlatformWindows::B3DMessageCallback(buma3d::DEBUG_MESSAGE_SEVERITY _sev, buma3d::DEBUG_MESSAGE_CATEGORY_FLAG _category, const buma3d::Char8T* const _msg, void* _user_data)
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

    debug::ILogger* logger = (debug::ILogger*)(_user_data);

    auto E = [&]()
    {
        std::cout << std::endl;
    };
    auto P = [&](const auto& s)
    {
        logger->LogInfo(s);;
        std::cout << s;

        switch (_sev)
        {
        case buma3d::DEBUG_MESSAGE_SEVERITY_INFO       : logger->LogInfo(_msg);     break;
        case buma3d::DEBUG_MESSAGE_SEVERITY_WARNING    : logger->LogWarn(_msg);     break;
        case buma3d::DEBUG_MESSAGE_SEVERITY_ERROR      : logger->LogError(_msg);    break;
        case buma3d::DEBUG_MESSAGE_SEVERITY_CORRUPTION : logger->LogCritical(_msg); break;

        default:
            logger->LogInfo(_msg);
            break;
        }
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

PlatformWindows::PlatformWindows()
    : PlatformBase      ()
    , wnd_class         {}
    , hins              {}
    , prev_hins         {}
    , cmdline           {}
    , num_cmdshow       {}
    , window_windows    {}
    , execution_path    {}
    , console_session   {}
    , logger            {}
{
    console_session = std::make_shared<ConsoleSession>();
    console_session->Begin();

    logger = std::make_shared<debug::LoggerWindows>();
}

PlatformWindows::~PlatformWindows()
{
    console_session.reset();
    logger.reset();
}

int PlatformWindows::MainLoop()
{
    int result = 0;
    while (window_windows->ProcessMessage())
    {
        app->Tick();
    }

    return result;
}

bool PlatformWindows::Init(const PLATFORM_DESC& _desc)
{
    if (_desc.type != PLATFORM_TYPE_WINDOWS) return false;
    if (!ParseCommandLines(_desc))           return false;
    if (!app)                                return false;

    auto dat = reinterpret_cast<const PLATFORM_DATA_WINDOWS*>(_desc.data);
    hins        = (HINSTANCE)dat->hInstance;
    prev_hins   = (HINSTANCE)dat->hPrevInstance;
    cmdline     = util::ConvertWideToAnsi(dat->lpCmdLine);
    num_cmdshow = dat->nCmdShow;

    if (!RegisterWndClass())                return false;
    if (!PrepareDeviceResources())          return false;
    if (!PrepareWindow(_desc.window_desc))  return false;

    app->Prepare(*this);

    return true;
}

bool PlatformWindows::Term()
{
    app.reset();
    window_windows.reset();
    window.reset();
    device_resources.reset();

    num_cmdshow = {};
    cmdline     = {};
    prev_hins   = {};
    hins        = {};

    return true;
}

bool PlatformWindows::ParseCommandLines(const PLATFORM_DESC& _desc)
{
    auto dat = reinterpret_cast<const PLATFORM_DATA_WINDOWS*>(_desc.data);

	int  argc{};
	auto argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argc != 0)
    {
        execution_path = std::move(util::ConvertWideToAnsi(argv[0]));

        cmd_lines.reserve(argc - 1);
        for (size_t i = 1; i < argc; i++)
        {
            cmd_lines.emplace_back(
                std::make_unique<std::string>(
                    std::move(util::ConvertWideToAnsi(argv[i]))));
        }
    }

    return true;
}

bool PlatformWindows::PrepareDeviceResources()
{
    device_resources = std::make_shared<DeviceResources>();

    INTERNAL_API_TYPE type = INTERNAL_API_TYPE_D3D12;
    auto&& api_type = std::find_if(cmd_lines.begin(), cmd_lines.end(), [](const std::unique_ptr<std::string>& _str) { return  (*_str) == "--internal-api-type"; });
    if (api_type != cmd_lines.end())
    {
        auto&& next = (**(api_type + 1));
        if (next == "vulkan")
            type = INTERNAL_API_TYPE_VULKAN;

        else if (next == "d3d12" || next == "dx12")
            type = INTERNAL_API_TYPE_D3D12;
    }

    auto&& dll_dir = std::find_if(cmd_lines.begin(), cmd_lines.end(), [](const std::unique_ptr<std::string>& _str) { return  (*_str) == "--library-dir"; });
    const char* dir = nullptr;
    if (dll_dir != cmd_lines.end())
        dir = (**(dll_dir + 1)).c_str();

    DEVICE_RESOURCE_DESC drd{};
    drd.type                   = type;
    drd.library_dir            = dir ? dir : "";
    drd.is_enable_debug        = true;
    drd.message_logger         = logger;
    drd.DebugMessageCallback   = B3DMessageCallback;
    if (!device_resources->Init(drd)) return false;

    return true;
}

bool PlatformWindows::PrepareWindow(const WINDOW_DESC& _desc)
{
    window_windows = std::make_shared<WindowWindows>(*this, wnd_class, _desc);
    window = window_windows;

    return true;
}

bool PlatformWindows::RegisterWndClass()
{
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    wnd_class.cbSize            = sizeof(WNDCLASSEXW);
    wnd_class.style             = CS_HREDRAW | CS_VREDRAW;
    wnd_class.lpfnWndProc       = WindowWindows::WndProc;
    wnd_class.cbClsExtra        = 0;
    wnd_class.cbWndExtra        = 0;
    wnd_class.hInstance         = hins;
    wnd_class.hIcon             = LoadIcon(wnd_class.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    wnd_class.hCursor           = LoadCursor(nullptr, IDC_ARROW);
    wnd_class.hbrBackground     = (HBRUSH)(COLOR_WINDOW + 1);
    wnd_class.lpszMenuName      = nullptr;
    wnd_class.lpszClassName     = CLASS_NAME;
    wnd_class.hIconSm           = LoadIcon(wnd_class.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

    auto res =  RegisterClassEx(&wnd_class);
    return res != 0;
}


}// namespace buma
