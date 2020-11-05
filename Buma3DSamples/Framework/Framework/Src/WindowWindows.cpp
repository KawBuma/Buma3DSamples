#include "pch.h"
#include "WindowWindows.h"

namespace buma
{

static LRESULT CALLBACK WndProc(HWND _hwnd, UINT _message, WPARAM _wparam, LPARAM _lparam);

bool WindowWindows::Resize(const buma3d::UINT2& _size)
{

    return true;
}

bool WindowWindows::Init(

    PlatformBase& _platform, uint32_t _back_buffer_count, const buma3d::EXTENT2D& _size, const char* _window_name, buma3d::RESOURCE_FORMAT _format, buma3d::SWAP_CHAIN_BUFFER_FLAGS _buffer_flags)
{

}

bool WindowWindows::Init(PlatformBase&                      _platform,
                         uint32_t                           _buffer_count,
                         const buma3d::EXTENT2D&            _size,
                         const char*                        _window_name,
                         buma3d::RESOURCE_FORMAT            _format,
                         buma3d::SWAP_CHAIN_BUFFER_FLAGS    _buffer_flags)
{
    if (!RegisterWndClass())
        return false;

    if (!CreateWnd(_size.width, _size.height))
        return false;

    return true;
}

bool WindowWindows::RegisterWndClass()
{
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    WNDCLASSEXW wcex{};
    wcex.cbSize            = sizeof(WNDCLASSEXW);
    wcex.style             = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc       = WndProc;
    wcex.cbClsExtra        = 0;
    wcex.cbWndExtra        = 0;
    wcex.hInstance         = platform.GetHinstance();
    wcex.hIcon             = LoadIcon(wcex.hInstance, L"IDI_ICON");
    wcex.hCursor           = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground     = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName      = nullptr;
    wcex.lpszClassName     = WND_CLASS_NAME;
    wcex.hIconSm           = LoadIcon(wcex.hInstance, L"IDI_ICON");

    auto res =  RegisterClassEx(&wcex);
    return res != 0;
}

bool WindowWindows::CreateWnd(uint32_t _width, uint32_t _height)
{
    RECT window_rect = { 0, 0, _width, _height };
    AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW, FALSE);

    hwnd = CreateWindowEx(0
                          , WND_CLASS_NAME, util::ConvertAnsiToWide(wnd_name).c_str()
                          , WS_OVERLAPPEDWINDOW
                          , CW_USEDEFAULT, CW_USEDEFAULT
                          , window_rect.right - window_rect.left
                          , window_rect.bottom - window_rect.top
                          , nullptr, nullptr
                          , platform.GetHinstance(), reinterpret_cast<void*>(&platform));

    if (!hwnd)
        return false;

    input::MouseInput::GetIns().SetWindow(hwnd);

    ShowWindow(hwnd, SW_SHOW);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&platform));

    return true;
}

bool WindowWindows::CreateSwapSurface()
{
    auto dr = platform.GetDeviceResources();
    buma3d::SURFACE_DESC                               sd{};
    buma3d::SURFACE_PLATFORM_DATA_WINDOWS              data_win{ platform.GetHinstance(), hwnd };
    buma3d::SURFACE_DESC                               sfs_desc{ buma3d::SURFACE_PLATFORM_DATA_TYPE_WINDOWS, &data_win };
    if (auto res = dr->GetAdapter()->CreateSurface(sd, &surface) | res == buma3d::BMRESULT_FAILED)
        return false;

    return true;
}

bool WindowWindows::CreateSwapChain()
{
    auto hins = platform.GetHinstance();
    buma3d::SWAP_CHAIN_DESC scd{};

    return true;
}


static LRESULT CALLBACK WndProc(HWND _hwnd, UINT _message, WPARAM _wparam, LPARAM _lparam)
{
    PAINTSTRUCT ps{};
    HDC         hdc{};

    // if (myimgui->WndProcHandler(_hwnd, _message, _wparam, _lparam))
    //     return true;

    auto fw = reinterpret_cast<WindowWindows*>(GetWindowLongPtr(_hwnd, GWLP_USERDATA));
    switch (_message)
    {
    case WM_PAINT:
    {
        fw->Tick();
        break;
    }
    case WM_MOVE:
    {
        fw->OnWindowMoved();
        break;
    }
    case WM_SIZE:
    {
        if (_wparam == SIZE_MINIMIZED)
        {
            if (!fw->is_minimized)
            {
                fw->is_minimized = true;

                if (!fw->is_in_suspend)
                    fw->OnSuspending();
                fw->is_in_suspend = true;
            }
        }
        else if (fw->is_minimized)
        {
            fw->is_minimized = false;

            if (fw->is_in_suspend)
                fw->OnResuming();
            fw->is_in_suspend = false;
        }
        else if (!fw->is_in_sizemove)
        {
            fw->OnWindowSizeChanged(LOWORD(_lparam), HIWORD(_lparam));
        }
        break;
    }
    case WM_ENTERSIZEMOVE:
    {
        fw->is_in_sizemove = true;
        break;
    }
    case WM_EXITSIZEMOVE:
    {
        fw->is_in_sizemove = false;
        RECT rc{};
        GetClientRect(_hwnd, &rc);

        fw->OnWindowSizeChanged(rc.right - rc.left, rc.bottom - rc.top);
        break;
    }
    case WM_GETMINMAXINFO:
    {
        auto info = reinterpret_cast<MINMAXINFO*>(_lparam);
        info->ptMinTrackSize.x = 16;
        info->ptMinTrackSize.y = 16;
        break;
    }
    case WM_DESTROY:
    {
        if (fw)
            fw->Quit();
        else
            PostQuitMessage(0);
        break;
    }
    case WM_ACTIVATEAPP:
    {
        input::KeyboardInput::ProcessMessage(_message, _wparam, _lparam);
        input::MouseInput::ProcessMessage(_message, _wparam, _lparam);

        if (_wparam)
            fw->OnActivated();
        else
            fw->OnDeactivated();
        break;
    }
    case WM_ACTIVATE:
    {
        break;
    }
    case WM_CREATE:
    {
        SetWindowLongPtr(_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
        SetWindowLongPtr(_hwnd, GWL_EXSTYLE, 0);

        int width = 800;
        int height = 600;
        fw->GetDefaultSize(width, height);

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
            if (!fw->is_in_suspend)
                fw->OnSuspending();
            fw->is_in_suspend = true;
            return TRUE;

        case PBT_APMRESUMESUSPEND:
            if (!fw->is_minimized)
            {
                if (fw->is_in_suspend)
                    fw->OnResuming();
                fw->is_in_suspend = false;
            }
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
        KEY.ProcessMessage(_message, _wparam, _lparam);

        if (false && _wparam == VK_RETURN && (_lparam & 0x60000000) == 0x20000000)
        {
            // Implements the classic ALT+ENTER fullscreen toggle
            if (fw->is_fullscreen)
            {
                SetWindowLongPtr(_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
                SetWindowLongPtr(_hwnd, GWL_EXSTYLE, 0);

                int width = 800;
                int height = 600;
                fw->GetDefaultSize(width, height);

                fw->OnWindowSizeChanged(width, height);
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

                int new_width = (rw.right - rw.left) - (rc.right - rc.left) + width;
                int new_height = (rw.bottom - rw.top) - (rc.bottom - rc.top) + height;

                SetWindowPos(_hwnd, NULL, (dispx - width) / 2 - (new_width - width), (dispy - height) / 2 - ((new_height - height) / 2), new_width, new_height, SWP_NOSIZE | SWP_NOZORDER);
                SetWindowPos(_hwnd, NULL, 0, 0, new_width, new_height, SWP_NOMOVE | SWP_NOZORDER);
            }
            else
            {
                SetWindowLongPtr(_hwnd, GWL_STYLE, 0);
                SetWindowLongPtr(_hwnd, GWL_EXSTYLE, WS_EX_TOPMOST);

                SetWindowPos(_hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

                ShowWindow(_hwnd, SW_SHOWMAXIMIZED);
            }

            fw->is_fullscreen = !fw->is_fullscreen;
        }
        break;
    }
    case WM_INPUT:
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MOUSEWHEEL:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
    case WM_MOUSEHOVER:
    {
        MOUSE.ProcessMessage(_message, _wparam, _lparam);
        break;
    }
    default:
    {
        break;
    }
    }

    return DefWindowProc(_hwnd, _message, _wparam, _lparam);
}


}// namespace buma
