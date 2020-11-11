#include "pch.h"
#include "PlatformWindows.h"

#include <shellapi.h>

#include <memory>

namespace buma
{

PlatformWindows::PlatformWindows()
    : PlatformBase      ()
    , wnd_class         {}
    , hins              {}
    , prev_hins         {}
    , cmdline           {}
    , num_cmdshow       {}
    , window_windows    {}
    , execution_path    {}
{
}

PlatformWindows::~PlatformWindows()
{
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
    if (!device_resources->Init(type, dir)) return false;

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
