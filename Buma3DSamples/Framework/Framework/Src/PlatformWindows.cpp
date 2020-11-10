#include "pch.h"
#include "PlatformWindows.h"

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
    if (!app)
        return false;

    if (_desc.type != PLATFORM_TYPE_WINDOWS)
        return false;

    auto dat = reinterpret_cast<const PLATFORM_DATA_WINDOWS*>(_desc.data);
    hins        = (HINSTANCE)dat->hInstance;
    prev_hins   = (HINSTANCE)dat->hPrevInstance;
    cmdline     = util::ConvertWideToAnsi(dat->lpCmdLine);
    num_cmdshow = dat->nCmdShow;

    if (!RegisterWndClass())       return false;
    if (!PrepareDeviceResources()) return false;
    if (!PrepareWindow())          return false;

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

bool PlatformWindows::PrepareDeviceResources()
{
    device_resources = std::make_shared<DeviceResources>();

    // FIXME: INTERNAL_API_TYPE type = INTERNAL_API_TYPE_D3D12;
    INTERNAL_API_TYPE type = INTERNAL_API_TYPE_D3D12;
    if (!device_resources->Init(type)) return false;

    return true;
}

bool PlatformWindows::PrepareWindow()
{
    window_windows = std::make_shared<WindowWindows>(*this, wnd_class, 3, buma3d::EXTENT2D{ 1280,720 }, "Buma3DSample",
                                                     buma3d::RESOURCE_FORMAT_B8G8R8A8_UNORM,
                                                     buma3d::SWAP_CHAIN_BUFFER_FLAG_COPY_DST | buma3d::SWAP_CHAIN_BUFFER_FLAG_COLOR_ATTACHMENT);
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
