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
        //: ofs("./log.txt", std::ios::out | std::ios::trunc, std::ios::_Default_open_prot)
        //, stream{}
        : stream{}
    {
    }

    ~ConsoleSession()
    {
        End();
    }

    void Begin()
    {
        //if (true) return;

        auto res = AllocConsole();
        assert(res);

        // coutにする
        auto console_out = "CONOUT$";
        freopen_s(&stream, console_out, "w+", stdout);
    }

    void End()
    {
        //if (true) return;

        auto res = FreeConsole();
        assert(res != 0);
        fclose(stream);
        //ofs.close();
    }

    //std::ofstream ofs;
private:
    FILE* stream;

};

void B3D_APIENTRY PlatformWindows::B3DMessageCallback(buma3d::DEBUG_MESSAGE_SEVERITY _sev, buma3d::DEBUG_MESSAGE_CATEGORY_FLAG _category, const buma3d::Char8T* const _msg, void* _user_data)
{
    if (_category == buma3d::DEBUG_MESSAGE_CATEGORY_FLAG_B3D_DETAILS)
        return;
    else if (_category == buma3d::DEBUG_MESSAGE_CATEGORY_FLAG_PERFORMANCE)
        return;

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
     , ", PERFORMANCE ] "
    };

    debug::ILogger* logger = (debug::ILogger*)(_user_data);
    std::stringstream ss;

    //if (_sev == buma3d::DEBUG_MESSAGE_SEVERITY_ERROR)
    //    ss << "\n\n";
    //else if (_sev == buma3d::DEBUG_MESSAGE_SEVERITY_WARNING)
    //    ss << ("\n");

    ss << SEVERITIES[_sev];
    DWORD i = 0;
    if (_BitScanForward(&i, _category))
        ss << CATEGORIES[i];
    ss << _msg;

    //if (_sev == buma3d::DEBUG_MESSAGE_SEVERITY_ERROR)
    //    ss << "\n";

    switch (_sev)
    {
    case buma3d::DEBUG_MESSAGE_SEVERITY_INFO       : logger->LogInfo    (ss.str().c_str()); break;
    case buma3d::DEBUG_MESSAGE_SEVERITY_WARNING    : logger->LogWarn    (ss.str().c_str()); break;
    case buma3d::DEBUG_MESSAGE_SEVERITY_ERROR      : logger->LogError   (ss.str().c_str()); break;
    case buma3d::DEBUG_MESSAGE_SEVERITY_CORRUPTION : logger->LogCritical(ss.str().c_str()); break;

    default:
        logger->LogInfo(ss.str().c_str());
        break;
    }
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
    , inputs            {}
{
    inputs = std::make_unique<input::PCInputsWindows>();
}

PlatformWindows::~PlatformWindows()
{
    console_session.reset();
    logger.reset();
    inputs.reset();
}

int PlatformWindows::MainLoop()
{
    int result = 0;
    auto&& wnd = *window;
    while (wnd.ProcessMessage())
    {
        ProcessMain();
    }

    return result;
}

void PlatformWindows::ProcessMain()
{
    timer.Tick();
    app->Tick();
}

bool PlatformWindows::Prepare(const PLATFORM_DESC& _desc)
{
    if (_desc.type != PLATFORM_TYPE_WINDOWS) return false;
    if (!ParseCommandLines(_desc))           return false;
    if (!app)                                return false;

    auto dat = reinterpret_cast<const PLATFORM_DATA_WINDOWS*>(_desc.data);
    hins        = (HINSTANCE)dat->hInstance;
    prev_hins   = (HINSTANCE)dat->hPrevInstance;
    cmdline     = util::ConvertWideToAnsi(dat->lpCmdLine);
    num_cmdshow = dat->nCmdShow;

    if (!PrepareLog())                      return false;
    if (!RegisterWndClass())                return false;
    if (!PrepareDeviceResources())          return false;
    if (!PrepareWindow())                   return false;
    if (!app->Prepare(*this))               return false;

    is_prepared = true;
    return true;
}

bool PlatformWindows::Init()
{
    auto&& wd = app->GetSettings().window_desc;
    if (!window_windows->Init(*this, wd))    return false;
    if (!app->Init())                        return false;

    return true;
}

bool PlatformWindows::Term()
{
    app->Term();
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

        LocalFree(argv);
    }
    return true;
}

std::vector<std::unique_ptr<std::string>>::iterator PlatformWindows::FindArgument(const char* _find_str)
{
    return std::find_if(cmd_lines.begin(), cmd_lines.end(), [_find_str](const std::unique_ptr<std::string>& _str) { return (*_str) == _find_str; });
}

bool PlatformWindows::PrepareDeviceResources()
{
    device_resources = std::make_shared<DeviceResources>();

    INTERNAL_API_TYPE type = INTERNAL_API_TYPE_D3D12;
    if (auto api_type = FindArgument("--internal-api-type"); api_type != cmd_lines.end())
    {
        auto&& next = (**(api_type + 1));
        if (next == "vulkan")
            type = INTERNAL_API_TYPE_VULKAN;

        else if ((next == "d3d12") || (next == "dx12"))
            type = INTERNAL_API_TYPE_D3D12;
    }

    const char* dir = nullptr;
    if (auto dll_dir = FindArgument("--library-dir"); dll_dir != cmd_lines.end())
        dir = (**(dll_dir + 1)).c_str();

    DEVICE_RESOURCE_DESC drd{};
    drd.type                    = type;
    drd.library_dir             = dir ? dir : "";
    drd.is_enabled_debug        = FindArgument("--enable-b3d-debug") != cmd_lines.end();
    drd.message_logger          = logger;
    drd.DebugMessageCallback    = B3DMessageCallback;
    if (!device_resources->Init(drd)) return false;

    return true;
}

bool PlatformWindows::PrepareWindow()
{
    window_windows = std::make_shared<WindowWindows>(*this, wnd_class);
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

bool PlatformWindows::PrepareLog()
{
    auto&& enable_debug = std::find_if(cmd_lines.begin(), cmd_lines.end(), [](const std::unique_ptr<std::string>& _str) { return (*_str) == "--enable-log"; });
    auto is_enabled = enable_debug != cmd_lines.end();
    if (is_enabled)
    {
        console_session = std::make_shared<ConsoleSession>();
        console_session->Begin();
    }
    logger = std::make_shared<debug::LoggerWindows>(is_enabled);

    return true;
}


}// namespace buma
