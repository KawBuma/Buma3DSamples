#include "pch.h"
#include "WindowWindows.h"

namespace buma
{

WindowWindows::WindowWindows(PlatformWindows&           _platform,
                             WNDCLASSEXW&               _wnd_class,
                             const WINDOW_DESC&         _desc)
    : WindowBase            ()
    , platform              { _platform }
    , wnd_class             { _wnd_class }
    , hwnd                  {}
    , wnd_name              { _desc.name }
    , window_process_flags    {}
    , windowed_size         { _desc.width,_desc.height }
    , windowed_offset       {}
    , aspect_ratio          { float(_desc.width) / float(_desc.height) }
    , msg                   {}
    , surface               {}
    , supported_formats     {}
    , swapchain             {}
    , swapchain_flags       {}
{
    Init(_platform, windowed_size, wnd_name.c_str());
}

WindowWindows::~WindowWindows()
{
    if (hwnd)
        DestroyWindow(hwnd);
    hwnd = NULL;
}

bool WindowWindows::OffsetWindow(const buma3d::OFFSET2D& _offset)
{
    windowed_offset = _offset;
    return SetWindowPos(hwnd, NULL, _offset.x, _offset.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

bool WindowWindows::ResizeWindow(const buma3d::EXTENT2D& _size, buma3d::SWAP_CHAIN_FLAGS _swapchain_flags = buma3d::SWAP_CHAIN_FLAG_NONE)
{
    SetWindowPos(hwnd, NULL, windowed_offset.x, windowed_offset.y, _size.width, _size.height, SWP_NOZORDER);
    if (!OnResize(_size, _swapchain_flags)) return false;

    platform.GetDeviceResources()->WaitForGpu();
    auto bmr = swapchain->Resize(_size, _swapchain_flags);

    return bmr == buma3d::BMRESULT_SUCCEED;
}

bool WindowWindows::ProcessMessage()
{
    window_process_flags = WINDOW_PROCESS_FLAG_NONE;
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (msg.message == WM_QUIT)
        return false;

    return true;
}

bool WindowWindows::Exit()
{
    window_process_flags |= WINDOW_PROCESS_FLAG_EXIT;
    return true;
}

bool WindowWindows::CreateSwapChain(const buma3d::SWAP_CHAIN_DESC& _desc, std::shared_ptr<buma::SwapChain>* _dst)
{
    auto&& dr = platform.GetDeviceResources();

    buma3d::util::Ptr<buma3d::ISwapChain> ptr;

    auto desc = _desc;
    desc.surface = surface.Get();
    auto bmr = dr->GetDevice()->CreateSwapChain(_desc, &ptr);
    if (bmr >= buma3d::BMRESULT_FAILED)
        return false;

    swapchain = std::make_shared<SwapChain>(dr, surface, ptr, _desc);
    swapchain->GetSwapChain()->SetName("SwapChain");

    return true;
}

const std::vector<buma3d::SURFACE_FORMAT>& WindowWindows::GetSupportedFormats() const 
{
    return supported_formats;
}

bool WindowWindows::Init(PlatformBase& _platform, const buma3d::EXTENT2D& _size, const char* _window_name)
{
    if (!CreateWnd(_size.width, _size.height))  return false;
    if (!CreateSurface())                       return false;

    return true;
}

bool WindowWindows::CreateWnd(uint32_t _width, uint32_t _height)
{
    RECT window_rect = { 0, 0, (LONG)_width, (LONG)_height };
    AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW, FALSE);

    hwnd = CreateWindowEx(0
                          , wnd_class.lpszClassName, util::ConvertAnsiToWide(wnd_name).c_str()
                          , WS_OVERLAPPEDWINDOW
                          , CW_USEDEFAULT, CW_USEDEFAULT
                          , window_rect.right - window_rect.left
                          , window_rect.bottom - window_rect.top
                          , nullptr, nullptr
                          , platform.GetHinstance(), reinterpret_cast<void*>(this));

    if (!hwnd)
        return false;

    input::MouseInput::GetIns().SetWindow(hwnd);

    return true;
}

bool WindowWindows::CreateSurface()
{
    auto&& dr = platform.GetDeviceResources();
    buma3d::SURFACE_PLATFORM_DATA_WINDOWS data_win{ platform.GetHinstance(), hwnd };
    buma3d::SURFACE_DESC                  sfs_desc{ buma3d::SURFACE_PLATFORM_DATA_TYPE_WINDOWS, &data_win };

    auto bmr = dr->GetAdapter()->CreateSurface(sfs_desc, &surface);
    if (bmr == buma3d::BMRESULT_FAILED)
        return false;

    supported_formats.resize(surface->GetSupportedSurfaceFormats(nullptr));
    surface->GetSupportedSurfaceFormats(supported_formats.data());

    return true;
}

bool WindowWindows::OnResize(const buma3d::EXTENT2D& _size, buma3d::SWAP_CHAIN_FLAGS _swapchain_flags)
{
    windowed_size   = _size;
    aspect_ratio    = float(_size.width) / float(_size.height);
    swapchain_flags = _swapchain_flags;

    if (swapchain)
        return ResizeBuffers(_size, _swapchain_flags);

    return true;
}

bool WindowWindows::ResizeBuffers(const buma3d::EXTENT2D& _size, buma3d::SWAP_CHAIN_FLAGS _swapchain_flags)
{
    platform.GetDeviceResources()->WaitForGpu();
    auto bmr = swapchain->Resize(_size, _swapchain_flags);
    return bmr == buma3d::BMRESULT_SUCCEED;
}

LRESULT CALLBACK WindowWindows::WndProc(HWND _hwnd, UINT _message, WPARAM _wparam, LPARAM _lparam)
{
    auto fw = reinterpret_cast<WindowWindows*>(GetWindowLongPtr(_hwnd, GWLP_USERDATA));

    //myimgui->WndProcHandler(_hwnd, _message, _wparam, _lparam);

    input::KeyboardInput::ProcessMessage(_message, _wparam, _lparam);
    input::MouseInput::ProcessMessage(_message, _wparam, _lparam);

    switch (_message)
    {
    case WM_MOVE:
    {
        break;
    }
    case WM_SIZE:
    {
        fw->window_process_flags |= WINDOW_PROCESS_FLAG_SIZE;
        switch (_wparam)
        {
        case SIZE_RESTORED:
            break;

        case SIZE_MINIMIZED:
            fw->window_state_flags |= WINDOW_STATE_FLAG_MINIMIZED;
            fw->window_process_flags |= WINDOW_PROCESS_FLAG_SIZE_MINIMIZED;
            break;

        case SIZE_MAXIMIZED:
            fw->window_state_flags &= ~WINDOW_STATE_FLAG_MINIMIZED;
            break;

        case SIZE_MAXSHOW:
            fw->window_process_flags |= WINDOW_PROCESS_FLAG_SIZE_MAXSHOW;
            break;

        case SIZE_MAXHIDE:
            fw->window_process_flags |= WINDOW_PROCESS_FLAG_SIZE_MAXHIDE;
            break;

        default:
            break;
        }

        RECT rc{};
        GetClientRect(_hwnd, &rc);
        fw->OnResize({ uint32_t(rc.right - rc.left), uint32_t(rc.bottom - rc.top) }, fw->swapchain_flags);
        break;
    }
    case WM_ENTERSIZEMOVE:
    {
        fw->window_state_flags |= WINDOW_STATE_FLAG_IN_SIZEMOVE;
        fw->window_process_flags |= WINDOW_PROCESS_FLAG_SIZEMOVE;
        break;
    }
    case WM_EXITSIZEMOVE:
    {
        fw->window_state_flags &= ~WINDOW_STATE_FLAG_IN_SIZEMOVE;
        RECT rc{};
        GetClientRect(_hwnd, &rc);

        fw->OnResize({ uint32_t(rc.right - rc.left), uint32_t(rc.bottom - rc.top) }, fw->swapchain_flags);
        break;
    }
    case WM_GETMINMAXINFO:
    {
        auto info = reinterpret_cast<MINMAXINFO*>(_lparam);
        info->ptMinTrackSize.x = 256;
        info->ptMinTrackSize.y = 256;
        break;
    }
    case WM_DESTROY:
    {
        if (fw)
            fw->Exit();
        PostQuitMessage(0);
        break;
    }
    case WM_ACTIVATEAPP:
    {
        _wparam != 0
            ? fw->window_process_flags |= WINDOW_PROCESS_FLAG_ACTIVATED
            : fw->window_process_flags |= WINDOW_PROCESS_FLAG_DEACTIVATED;
        break;
    }
    case WM_ACTIVATE:
    {
        break;
    }
    case WM_CREATE:
    {
        LPCREATESTRUCT create_struct = reinterpret_cast<LPCREATESTRUCT>(_lparam);
        SetWindowLongPtr(_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(create_struct->lpCreateParams));
        SetWindowLongPtr(_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
        SetWindowLongPtr(_hwnd, GWL_EXSTYLE, 0);

        auto lpfw = reinterpret_cast<WindowWindows*>(create_struct->lpCreateParams);
        int width = lpfw->windowed_size.width;
        int height = lpfw->windowed_size.height;

        //fw->OnWindowSizeChanged(width, height);
        ShowWindow(_hwnd, SW_SHOWNORMAL);

        int dispx = GetSystemMetrics(SM_CXSCREEN);
        int dispy = GetSystemMetrics(SM_CYSCREEN);

        RECT rw, rc;
        GetWindowRect(_hwnd, &rw);
        GetClientRect(_hwnd, &rc);

        int new_width = (rw.right - rw.left) - (rc.right - rc.left) + width;
        int new_height = (rw.bottom - rw.top) - (rc.bottom - rc.top) + height;

        SetWindowPos(_hwnd, NULL, (dispx - width) / 2 - (new_width - width), (dispy - height) / 2 - ((new_height - height) / 2), new_width, new_height, SWP_NOSIZE | SWP_NOZORDER);
        SetWindowPos(_hwnd, NULL, 0, 0, new_width, new_height, SWP_NOMOVE | SWP_NOZORDER);
        break;
    }
    case WM_POWERBROADCAST:
    {
        switch (_wparam)
        {
        case PBT_APMQUERYSUSPEND:

            return TRUE;

        case PBT_APMRESUMESUSPEND:

            return TRUE;
        }
        break;
    }
    case WM_MENUCHAR:
    {
        // A menu is active and the user presses a key that does not correspond
        // to any mnemonic or accelerator key. Ignore so we don't produce an error beep.
        return MAKELRESULT(0, MNC_CLOSE);

    }
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
        if (_wparam == VK_RETURN && (_lparam & 0x60000000) == 0x20000000)
        {
            // Implements the classic ALT+ENTER fullscreen toggle
            if (fw->window_state_flags & WINDOW_STATE_FLAG_FULLSCREEN)
            {
                SetWindowLongPtr(_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
                SetWindowLongPtr(_hwnd, GWL_EXSTYLE, 0);

                auto width  = fw->windowed_size.width;
                auto height = fw->windowed_size.height;
                //fw->OnResize(fw->windowed_size, fw->swapchain_flags);
                ShowWindow(_hwnd, SW_SHOWNORMAL);

                int dispx = GetSystemMetrics(SM_CXSCREEN);
                int dispy = GetSystemMetrics(SM_CYSCREEN);

                //RECT rc;
                ////rc.
                ////AdjustWindowRect(_hwnd, rc);
                //GetWindowRect(_hwnd, &rc);
                ////width = width + (width - (rc.right - rc.left));
                ////height = height + (height - (rc.bottom - rc.top));

                //SetWindowPos(_hwnd, HWND_TOP, (dispx - width) / 2, (dispy - height) / 2, width, height, SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
                RECT rw, rc;
                GetWindowRect(_hwnd, &rw);
                GetClientRect(_hwnd, &rc);

                int new_width  = (rw.right  - rw.left) - (rc.right  - rc.left) + width;
                int new_height = (rw.bottom - rw.top ) - (rc.bottom - rc.top ) + height;

                SetWindowPos(_hwnd, NULL,
                             (dispx - width ) / 2 -  (new_width  - width ),
                             (dispy - height) / 2 - ((new_height - height) / 2),
                             new_width, new_height,
                             SWP_NOSIZE | SWP_NOZORDER);
                SetWindowPos(_hwnd, NULL, 0, 0, new_width, new_height, SWP_NOMOVE | SWP_NOZORDER);

                fw->window_state_flags &= ~WINDOW_STATE_FLAG_FULLSCREEN;
            }
            else
            {
                SetWindowLongPtr(_hwnd, GWL_STYLE, 0);
                SetWindowLongPtr(_hwnd, GWL_EXSTYLE, WS_EX_TOPMOST);

                SetWindowPos(_hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

                ShowWindow(_hwnd, SW_SHOWMAXIMIZED);

                fw->window_state_flags |= WINDOW_STATE_FLAG_FULLSCREEN;
            }
        }
        break;
    }

    default:
        break;
    }

    return DefWindowProc(_hwnd, _message, _wparam, _lparam);
}


}// namespace buma
