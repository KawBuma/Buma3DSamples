#include "pch.h"
#include "PlatformWindows.h"

namespace buma
{

PlatformWindows::PlatformWindows(HINSTANCE   _h_instance,
                                 HINSTANCE   _h_previnstance,
                                 LPWSTR      _lp_cmdline,
                                 int         _n_cmdshow)
    : hins          { _h_instance }
    , prev_hins     { _h_previnstance } 
    , cmdline       { util::ConvertWideToAnsi(_lp_cmdline) }
    , num_cmdshow   { _n_cmdshow } 
{
    Prepare();
}

PlatformWindows::~PlatformWindows()
{

}

int PlatformWindows::MainLoop()
{
    int result = 0;
    while (!window_windows->ProcessMessage())
    {
        app->Tick();
    }

    app.reset();
    window.reset();
    window_windows.reset();
    device_resources.reset();

    return result;
}

bool PlatformWindows::Prepare()
{
    auto res = PrepareWindow();
    device_resources = std::make_shared<DeviceResources>();
    device_resources->Init();
    app->Prepare(*this);
    return res;
}

bool PlatformWindows::PrepareWindow()
{
    window = window_windows = std::make_shared<WindowWindows>(WindowWindows(*this, 3, { 1280,720 }, "Buma3DSample"
                                                                            , buma3d::RESOURCE_FORMAT_UNKNOWN
                                                                            , buma3d::SWAP_CHAIN_BUFFER_FLAG_COPY_DST |
                                                                              buma3d::SWAP_CHAIN_BUFFER_FLAG_COLOR_ATTACHMENT));
    return true;
}

}// namespace buma
