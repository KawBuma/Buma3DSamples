#include "pch.h"
#include "WindowWindows.h"

namespace buma
{

static LRESULT CALLBACK WndProc(HWND _hwnd, UINT _message, WPARAM _wparam, LPARAM _lparam);

bool WindowWindows::Resize(const buma3d::EXTENT2D& _size)
{
    auto desc = swapchain->GetDesc();
    desc.buffer.width  = windowed_size.width;
    desc.buffer.height = windowed_size.height;
    auto bmr = swapchain->Recreate(desc);

    return bmr == buma3d::BMRESULT_SUCCEED;
}

bool WindowWindows::ProcessMessage()
{
    MSG msg{};
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
    window_state_flags |= WINDOW_STATE_FLAG_EXIT;
    return true;
}

bool WindowWindows::Init(PlatformBase&                      _platform,
                         uint32_t                           _buffer_count,
                         const buma3d::EXTENT2D&            _size,
                         const char*                        _window_name,
                         buma3d::RESOURCE_FORMAT            _format,
                         buma3d::SWAP_CHAIN_BUFFER_FLAGS    _buffer_flags)
{
    if (!RegisterWndClass())                    return false;
    if (!CreateWnd(_size.width, _size.height))  return false;
    if (!CreateSurface())                       return false;
    if (!CreateSwapChain())                     return false;

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

bool WindowWindows::CreateSurface()
{
    auto dr = platform.GetDeviceResources();
    buma3d::SURFACE_DESC                               sd{};
    buma3d::SURFACE_PLATFORM_DATA_WINDOWS              data_win{ platform.GetHinstance(), hwnd };
    buma3d::SURFACE_DESC                               sfs_desc{ buma3d::SURFACE_PLATFORM_DATA_TYPE_WINDOWS, &data_win };
    if (auto res = dr->GetAdapter()->CreateSurface(sd, &surface); res == buma3d::BMRESULT_FAILED)
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
    // if (myimgui->WndProcHandler(_hwnd, _message, _wparam, _lparam))
    //     return true;

    input::KeyboardInput::ProcessMessage(_message, _wparam, _lparam);
    input::MouseInput::ProcessMessage(_message, _wparam, _lparam);

    auto fw = reinterpret_cast<WindowWindows*>(GetWindowLongPtr(_hwnd, GWLP_USERDATA));
    fw->window_state_flags &= ~WINDOW_STATE_FLAG_ACTIVATED;
    fw->window_state_flags &= ~WINDOW_STATE_FLAG_DEACTIVATED;

    switch (_message)
    {
    case WM_MOVE:
    {
        break;
    }
    case WM_SIZE:
    {
        if (_wparam == SIZE_MINIMIZED)
        {
            if (!(fw->window_state_flags & WINDOW_STATE_FLAG_SIZE_MINIMIZED))
            {
                fw->window_state_flags |= WINDOW_STATE_FLAG_SIZE_MINIMIZED;
            }
        }
        else if (fw->window_state_flags & WINDOW_STATE_FLAG_SIZE_MINIMIZED)
        {
            fw->window_state_flags &= ~WINDOW_STATE_FLAG_SIZE_MINIMIZED;
        }
        else if (!(fw->window_state_flags & WINDOW_STATE_FLAG_SIZE_MAXHIDE))
        {
            fw->Resize({ LOWORD(_lparam), HIWORD(_lparam) });
        }
        break;
    }
    case WM_ENTERSIZEMOVE:
    {
        fw->window_state_flags |= WINDOW_STATE_FLAG_SIZE_MINIMIZED;
        break;
    }
    case WM_EXITSIZEMOVE:
    {
        fw->window_state_flags &= ~WINDOW_STATE_FLAG_SIZE_MINIMIZED;
        RECT rc{};
        GetClientRect(_hwnd, &rc);

        fw->Resize({ rc.right - rc.left, rc.bottom - rc.top });
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
        else
            PostQuitMessage(0);
        break;
    }
    case WM_ACTIVATEAPP:
    {
        _wparam != 0
            ? fw->window_state_flags |= WINDOW_STATE_FLAG_ACTIVATED
            : fw->window_state_flags |= WINDOW_STATE_FLAG_DEACTIVATED;
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

        int width = fw->windowed_size.width;
        int height = fw->windowed_size.height;

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

                int width = 1280;
                int height = 720;

                fw->Resize({ width, height });
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
