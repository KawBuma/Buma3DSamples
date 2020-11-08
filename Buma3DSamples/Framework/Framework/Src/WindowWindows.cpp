#include "pch.h"
#include "WindowWindows.h"

namespace buma
{

static LRESULT CALLBACK WndProc(HWND _hwnd, UINT _message, WPARAM _wparam, LPARAM _lparam);

WindowWindows::WindowWindows(PlatformWindows&                  _platform,
                             uint32_t                          _back_buffer_count,
                             const buma3d::EXTENT2D&           _size,
                             const char*                       _window_name,
                             buma3d::RESOURCE_FORMAT           _format,
                             buma3d::SWAP_CHAIN_BUFFER_FLAGS   _buffer_flags,
                             buma3d::SWAP_CHAIN_FLAGS          _swapchain_flags)
    : WindowBase            ()
    , platform              { _platform }
    , wnd_class             {}
    , hwnd                  {}
    , wnd_name              {}
    , window_state_flags    {}
    , windowed_size         {}
    , aspect_ratio          {}
    , swapchain_desc        {}
    , supported_formats     {}
    , back_buffers          {}
{
    Init(_platform, _back_buffer_count, _size, _window_name, _format, _buffer_flags, _swapchain_flags);
}

WindowWindows::~WindowWindows()
{
    if (hwnd)
        DestroyWindow(hwnd);
    hwnd = NULL;
}

bool WindowWindows::Resize(const buma3d::EXTENT2D& _size, buma3d::SWAP_CHAIN_FLAGS _swapchain_flags)
{
    windowed_size = _size;
    aspect_ratio = float(_size.width) / float(_size.height);

    swapchain_desc = swapchain->GetDesc();
    swapchain_desc.buffer.width  = windowed_size.width;
    swapchain_desc.buffer.height = windowed_size.height;
    swapchain_desc.flags         = _swapchain_flags;
    auto bmr = swapchain->Recreate(swapchain_desc);
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
                         buma3d::SWAP_CHAIN_BUFFER_FLAGS    _buffer_flags,
                         buma3d::SWAP_CHAIN_FLAGS           _swapchain_flags)
{
    swapchain_desc.color_space                  = buma3d::COLOR_SPACE_SRGB_NONLINEAR;
    swapchain_desc.pre_roration                 = buma3d::ROTATION_MODE_IDENTITY;
    swapchain_desc.buffer.width                 = _size.width;
    swapchain_desc.buffer.height                = _size.height;
    swapchain_desc.buffer.count                 = _buffer_count;
    swapchain_desc.buffer.format_desc.format    = _format;
    swapchain_desc.buffer.flags                 = _buffer_flags;
    swapchain_desc.alpha_mode                   = buma3d::SWAP_CHAIN_ALPHA_MODE_DEFAULT;
    swapchain_desc.flags                        = _swapchain_flags;

    if (!RegisterWndClass())                    return false;
    if (!CreateWnd(_size.width, _size.height))  return false;
    if (!CreateSurface())                       return false;
    if (!CreateSwapChain())                     return false;

    return true;
}

bool WindowWindows::RegisterWndClass()
{
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    wnd_class.cbSize            = sizeof(WNDCLASSEXW);
    wnd_class.style             = CS_HREDRAW | CS_VREDRAW;
    wnd_class.lpfnWndProc       = WndProc;
    wnd_class.cbClsExtra        = 0;
    wnd_class.cbWndExtra        = 0;
    wnd_class.hInstance         = platform.GetHinstance();
    wnd_class.hIcon             = LoadIcon(wnd_class.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    wnd_class.hCursor           = LoadCursor(nullptr, IDC_ARROW);
    wnd_class.hbrBackground     = (HBRUSH)(COLOR_WINDOW + 1);
    wnd_class.lpszMenuName      = nullptr;
    wnd_class.lpszClassName     = WND_CLASS_NAME;
    wnd_class.hIconSm           = LoadIcon(wnd_class.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

    auto res =  RegisterClassEx(&wnd_class);
    return res != 0;
}

bool WindowWindows::CreateWnd(uint32_t _width, uint32_t _height)
{
    RECT window_rect = { 0, 0, (LONG)_width, (LONG)_height };
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
    auto&& dr = platform.GetDeviceResources();
    buma3d::SURFACE_PLATFORM_DATA_WINDOWS data_win{ platform.GetHinstance(), hwnd };
    buma3d::SURFACE_DESC                  sfs_desc{ buma3d::SURFACE_PLATFORM_DATA_TYPE_WINDOWS, &data_win };

    auto res = dr->GetAdapter()->CreateSurface(sfs_desc, &surface);
    if (res == buma3d::BMRESULT_FAILED)
        return false;

    return true;
}

bool WindowWindows::CreateSwapChain()
{
    // スワップチェインのフォーマットを取得
    buma3d::SURFACE_FORMAT sfs_format{};
    {
        supported_formats.resize(surface->GetSupportedSurfaceFormats(nullptr));
        surface->GetSupportedSurfaceFormats(supported_formats.data());

        auto&& it_find = std::find_if(supported_formats.begin(), supported_formats.end(),[this](const buma3d::SURFACE_FORMAT& _format)
        {
            return _format.format      == swapchain_desc.buffer.format_desc.format &&
                   _format.color_space == swapchain_desc.color_space;
        });

        if (it_find != supported_formats.end())
            sfs_format = supported_formats[0];
        else
            sfs_format = *it_find;
    }

    // スワップチェインを作成
    {
        auto&& state = surface->GetState();
        aspect_ratio = float(state.size.width) / float(state.size.height);
        windowed_size = state.size;

        swapchain_desc.surface                      = surface.Get();
        swapchain_desc.color_space                  = sfs_format.color_space;
        swapchain_desc.buffer.format_desc.format    = sfs_format.format;

        auto&& dr = platform.GetDeviceResources();
        auto&& cmd_que = dr->GetCommandQueues(buma3d::COMMAND_TYPE_DIRECT);
        buma3d::ICommandQueue* queues[] = { cmd_que[0].Get() };
        swapchain_desc.num_present_queues           = 1;
        swapchain_desc.present_queues               = queues;

        auto bmr = dr->GetDevice()->CreateSwapChain(swapchain_desc, &swapchain);
        if (bmr == buma3d::BMRESULT_FAILED)
            return false;

        swapchain->SetName("SwapChain");
    }

    // バックバッファを取得
    {
        auto&& scd = swapchain->GetDesc();
        back_buffers.resize(scd.buffer.count);
        for (uint32_t i = 0; i < scd.buffer.count; i++)
        {
            auto bmr = swapchain->GetBuffer(i, &back_buffers[i]);
            if (bmr == buma3d::BMRESULT_FAILED)
                return false;

            back_buffers[i]->SetName((std::string("SwapChain buffer") + std::to_string(i)).c_str());
        }
    }

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

        fw->Resize({ uint32_t(rc.right - rc.left), uint32_t(rc.bottom - rc.top) });
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

                uint32_t width = 1280;
                uint32_t height = 720;

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
