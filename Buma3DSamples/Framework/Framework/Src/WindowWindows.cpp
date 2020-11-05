#include "pch.h"
#include "WindowWindows.h"

namespace buma
{

bool WindowWindows::Resize(const buma3d::UINT2& _size)
{

    return true;
}

bool WindowWindows::Init(PlatformBase&                      _platform,
                         uint32_t                           _buffer_count,
                         const buma3d::EXTENT2D&            _size,
                         buma3d::RESOURCE_FORMAT            _format,
                         buma3d::SWAP_CHAIN_BUFFER_FLAGS    _buffer_flags)
{
    CreateWnd(_size.x, _size.y);
    return true;
}

bool WindowWindows::CreateWnd(uint32_t _width, uint32_t _height)
{
    RECT window_rect = { 0, 0, _width, _height };
    AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW, FALSE);

    hwnd = CreateWindowEx(0
                          , WND_CLASS_NAME, ConvertAnsiToWide(wnd_name).c_str()
                          , WS_OVERLAPPEDWINDOW
                          , CW_USEDEFAULT, CW_USEDEFAULT
                          , window_rect.right - window_rect.left
                          , window_rect.bottom - window_rect.top
                          , nullptr, nullptr
                          , platform.GetHinstance(), reinterpret_cast<void*>(&platform));

    if (!hwnd)
    {
        MessageBox(NULL, L"Could not create the render window.", L"Error", MB_OK | MB_ICONERROR);
        assert(false);
    }
    else
    {
        input::MouseInput::GetIns().SetWindow(hwnd);

        ShowWindow(hwnd, SW_SHOW);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&platform));
    }

}

bool WindowWindows::CreateSwapSurface()
{
    return true;
}

bool WindowWindows::CreateSwapChain()
{
    platform.
    return true;
}



}// namespace buma
