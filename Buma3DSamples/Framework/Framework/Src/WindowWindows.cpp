#include "pch.h"
#include "Platform.h"
#include "PlatformWindows.h"
#include "WindowWindows.h"

namespace buma
{

namespace /*anonymous*/
{

inline buma3d::EXTENT2D ConvertToWindowSize(HWND _hwnd, const buma3d::EXTENT2D& _client_size)
{
    RECT wr{}, cr{};
    GetWindowRect(_hwnd, &wr);
    GetClientRect(_hwnd, &cr);
    uint32_t diff_w = uint32_t((wr.right - wr.left) - cr.right);
    uint32_t diff_h = uint32_t((wr.bottom - wr.top) - cr.bottom);
    return { _client_size.width + diff_w, _client_size.height + diff_h };
}

}// namespace /*anonymous*/


WindowWindows::WindowWindows(PlatformWindows&   _platform,
                             WNDCLASSEXW&       _wnd_class,
                             const WINDOW_DESC& _desc)
    : WindowBase                    ()
    , platform                      { _platform }
    , wnd_class                     { _wnd_class }
    , hwnd                          {}
    , wnd_name                      { _desc.name ? _desc.name : "" }
    , window_state_flags            {}
    , window_process_flags          {}
    , windowed_size                 { _desc.width, _desc.height }
    , windowed_offset               {}
    , aspect_ratio                  { float(_desc.width) / float(_desc.height) }
    , msg                           {}
    , surface                       {}
    , supported_formats             {}
    , swapchain                     {}
    , swapchain_flags               {}
    , delegate_on_resize            {}
    , delegate_on_buffer_resized    {}

{
    Init(_platform, windowed_size, wnd_name.c_str());
}

WindowWindows::~WindowWindows()
{
    swapchain.reset();
    surface.Reset();
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
    auto window_size = ConvertToWindowSize(hwnd, _size);
    SetWindowPos(hwnd, NULL, (int)windowed_offset.x, (int)windowed_offset.y, int(window_size.width), int(window_size.height), SWP_NOZORDER);
    if (!OnResize(_size, _swapchain_flags)) return false;

    return true;
}

bool WindowWindows::SetWindowTitle(const char* _text)
{
    return SetWindowTextA(hwnd, _text);
}

bool WindowWindows::ProcessMessage()
{
    if (window_process_flags & WINDOW_PROCESS_FLAG_EXIT)
        return false;

    window_process_flags = WINDOW_PROCESS_FLAG_NONE;
    // キューの残りメッセージがゼロになるまで取得
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            return false;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    input::KeyboardInput::GetIns().Update(platform.GetStepTimer()->GetElapsedSecondsF());
    input::MouseInput::GetIns().Update(platform.GetStepTimer()->GetElapsedSecondsF());
    //input::GamePadInputs::GetIns().Update(platform.GetStepTimer()->GetElapsedSecondsF());

    return true;
}

bool WindowWindows::Exit()
{
    window_process_flags |= WINDOW_PROCESS_FLAG_EXIT;
    platform.GetLogger()->LogInfo("exit");
    return true;
}

bool WindowWindows::CreateSwapChain(const buma3d::SWAP_CHAIN_DESC& _desc, std::shared_ptr<buma::SwapChain>* _dst)
{
    auto&& dr = platform.GetDeviceResources();

    buma3d::util::Ptr<buma3d::ISwapChain> ptr;

    buma3d::SWAP_CHAIN_DESC desc = _desc;
    desc.surface = surface.Get();
    auto bmr = dr->GetDevice()->CreateSwapChain(desc, &ptr);
    if (bmr >= buma3d::BMRESULT_FAILED)
        return false;

    swapchain = std::make_shared<SwapChain>(dr, surface, ptr, desc);
    swapchain->GetSwapChain()->SetName("SwapChain");
    *_dst = swapchain;

    return true;
}

const std::vector<buma3d::SURFACE_FORMAT>& WindowWindows::GetSupportedFormats() const 
{
    return supported_formats;
}

void WindowWindows::AddResizeEvent(const EventPtr& _event)
{
    delegate_on_resize += _event;
}

void WindowWindows::AddBufferResizedEvent(const EventPtr& _event)
{
    delegate_on_buffer_resized += _event;
}

bool WindowWindows::Init(PlatformBase& _platform, const buma3d::EXTENT2D& _size, const char* _window_name)
{
    if (!CreateWnd(_size.width, _size.height))  return false;
    if (!SetWindowTitle(_window_name))          return false;
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

    ShowWindow(hwnd, SW_SHOWNORMAL);

    RECT rc{};
    GetWindowRect(hwnd, &rc);
    windowed_offset.x = rc.left;
    windowed_offset.y = rc.top;

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
    buma3d::EXTENT2D size = _size;
    if (!(window_state_flags & WINDOW_STATE_FLAG_MAXIMIZED) && window_state_flags & WINDOW_STATE_FLAG_MINIMIZED)
        windowed_size = _size;

    aspect_ratio    = float(_size.width) / float(_size.height);
    swapchain_flags = _swapchain_flags;

    bool result = true;
    if (swapchain)
    {
        auto args = ResizeEventArgs{ size, swapchain_flags };
        delegate_on_resize(&args);
        result = ResizeBuffers(_size, _swapchain_flags);
        if (result)
            delegate_on_buffer_resized(&args);
    }

    return result;
}

bool WindowWindows::ResizeBuffers(const buma3d::EXTENT2D& _size, buma3d::SWAP_CHAIN_FLAGS _swapchain_flags)
{
    platform.GetDeviceResources()->WaitForGpu();
    return swapchain->Resize(_size, _swapchain_flags);
}

LRESULT CALLBACK WindowWindows::WndProc(HWND _hwnd, UINT _message, WPARAM _wparam, LPARAM _lparam)
{
    auto fw = reinterpret_cast<WindowWindows*>(GetWindowLongPtr(_hwnd, GWLP_USERDATA));

    //myimgui->WndProcHandler(_hwnd, _message, _wparam, _lparam);

//#if defined _DEBUG
#if 0
#define KEY_NAME(x) { x, #x }
    static std::unordered_map<UINT/*message*/, const char*> MESSAGE_NAMES_MAP = {
          KEY_NAME(WM_NULL)
        , KEY_NAME(WM_CREATE)
        , KEY_NAME(WM_DESTROY)
        , KEY_NAME(WM_MOVE)
        , KEY_NAME(WM_SIZE)
        , KEY_NAME(WM_ACTIVATE)
        , KEY_NAME(WM_SETFOCUS)
        , KEY_NAME(WM_KILLFOCUS)
        , KEY_NAME(WM_ENABLE)
        , KEY_NAME(WM_SETREDRAW)
        , KEY_NAME(WM_SETTEXT)
        , KEY_NAME(WM_GETTEXT)
        , KEY_NAME(WM_GETTEXTLENGTH)
        , KEY_NAME(WM_PAINT)
        , KEY_NAME(WM_CLOSE)
        , KEY_NAME(WM_QUERYENDSESSION)
        , KEY_NAME(WM_QUERYOPEN)
        , KEY_NAME(WM_ENDSESSION)
        , KEY_NAME(WM_QUIT)
        , KEY_NAME(WM_ERASEBKGND)
        , KEY_NAME(WM_SYSCOLORCHANGE)
        , KEY_NAME(WM_SHOWWINDOW)
        , KEY_NAME(WM_WININICHANGE)
        , KEY_NAME(WM_SETTINGCHANGE)
        , KEY_NAME(WM_DEVMODECHANGE)
        , KEY_NAME(WM_ACTIVATEAPP)
        , KEY_NAME(WM_FONTCHANGE)
        , KEY_NAME(WM_TIMECHANGE)
        , KEY_NAME(WM_CANCELMODE)
        , KEY_NAME(WM_SETCURSOR)
        , KEY_NAME(WM_MOUSEACTIVATE)
        , KEY_NAME(WM_CHILDACTIVATE)
        , KEY_NAME(WM_QUEUESYNC)
        , KEY_NAME(WM_GETMINMAXINFO)
        , KEY_NAME(WM_PAINTICON)
        , KEY_NAME(WM_ICONERASEBKGND)
        , KEY_NAME(WM_NEXTDLGCTL)
        , KEY_NAME(WM_SPOOLERSTATUS)
        , KEY_NAME(WM_DRAWITEM)
        , KEY_NAME(WM_MEASUREITEM)
        , KEY_NAME(WM_DELETEITEM)
        , KEY_NAME(WM_VKEYTOITEM)
        , KEY_NAME(WM_CHARTOITEM)
        , KEY_NAME(WM_SETFONT)
        , KEY_NAME(WM_GETFONT)
        , KEY_NAME(WM_SETHOTKEY)
        , KEY_NAME(WM_GETHOTKEY)
        , KEY_NAME(WM_QUERYDRAGICON)
        , KEY_NAME(WM_COMPAREITEM)
        , KEY_NAME(WM_GETOBJECT)
        , KEY_NAME(WM_COMPACTING)
        , KEY_NAME(WM_COMMNOTIFY)
        , KEY_NAME(WM_WINDOWPOSCHANGING)
        , KEY_NAME(WM_WINDOWPOSCHANGED)
        , KEY_NAME(WM_POWER)
        , KEY_NAME(WM_COPYDATA)
        , KEY_NAME(WM_CANCELJOURNAL)
        , KEY_NAME(WM_NOTIFY)
        , KEY_NAME(WM_INPUTLANGCHANGEREQUEST)
        , KEY_NAME(WM_INPUTLANGCHANGE)
        , KEY_NAME(WM_TCARD)
        , KEY_NAME(WM_HELP)
        , KEY_NAME(WM_USERCHANGED)
        , KEY_NAME(WM_NOTIFYFORMAT)
        , KEY_NAME(WM_CONTEXTMENU)
        , KEY_NAME(WM_STYLECHANGING)
        , KEY_NAME(WM_STYLECHANGED)
        , KEY_NAME(WM_DISPLAYCHANGE)
        , KEY_NAME(WM_GETICON)
        , KEY_NAME(WM_SETICON)
        , KEY_NAME(WM_NCCREATE)
        , KEY_NAME(WM_NCDESTROY)
        , KEY_NAME(WM_NCCALCSIZE)
        , KEY_NAME(WM_NCHITTEST)
        , KEY_NAME(WM_NCPAINT)
        , KEY_NAME(WM_NCACTIVATE)
        , KEY_NAME(WM_GETDLGCODE)
        , KEY_NAME(WM_SYNCPAINT)
        , KEY_NAME(WM_NCMOUSEMOVE)
        , KEY_NAME(WM_NCLBUTTONDOWN)
        , KEY_NAME(WM_NCLBUTTONUP)
        , KEY_NAME(WM_NCLBUTTONDBLCLK)
        , KEY_NAME(WM_NCRBUTTONDOWN)
        , KEY_NAME(WM_NCRBUTTONUP)
        , KEY_NAME(WM_NCRBUTTONDBLCLK)
        , KEY_NAME(WM_NCMBUTTONDOWN)
        , KEY_NAME(WM_NCMBUTTONUP)
        , KEY_NAME(WM_NCMBUTTONDBLCLK)
        , KEY_NAME(WM_NCXBUTTONDOWN)
        , KEY_NAME(WM_NCXBUTTONUP)
        , KEY_NAME(WM_NCXBUTTONDBLCLK)
        , KEY_NAME(WM_INPUT_DEVICE_CHANGE)
        , KEY_NAME(WM_INPUT)
        , KEY_NAME(WM_KEYFIRST)
        , KEY_NAME(WM_KEYDOWN)
        , KEY_NAME(WM_KEYUP)
        , KEY_NAME(WM_CHAR)
        , KEY_NAME(WM_DEADCHAR)
        , KEY_NAME(WM_SYSKEYDOWN)
        , KEY_NAME(WM_SYSKEYUP)
        , KEY_NAME(WM_SYSCHAR)
        , KEY_NAME(WM_SYSDEADCHAR)
        , KEY_NAME(WM_UNICHAR)
        , KEY_NAME(WM_KEYLAST)
        , KEY_NAME(WM_KEYLAST)
        , KEY_NAME(WM_IME_STARTCOMPOSITION)
        , KEY_NAME(WM_IME_ENDCOMPOSITION)
        , KEY_NAME(WM_IME_COMPOSITION)
        , KEY_NAME(WM_IME_KEYLAST)
        , KEY_NAME(WM_INITDIALOG)
        , KEY_NAME(WM_COMMAND)
        , KEY_NAME(WM_SYSCOMMAND)
        , KEY_NAME(WM_TIMER)
        , KEY_NAME(WM_HSCROLL)
        , KEY_NAME(WM_VSCROLL)
        , KEY_NAME(WM_INITMENU)
        , KEY_NAME(WM_INITMENUPOPUP)
        , KEY_NAME(WM_GESTURE)
        , KEY_NAME(WM_GESTURENOTIFY)
        , KEY_NAME(WM_MENUSELECT)
        , KEY_NAME(WM_MENUCHAR)
        , KEY_NAME(WM_ENTERIDLE)
        , KEY_NAME(WM_MENURBUTTONUP)
        , KEY_NAME(WM_MENUDRAG)
        , KEY_NAME(WM_MENUGETOBJECT)
        , KEY_NAME(WM_UNINITMENUPOPUP)
        , KEY_NAME(WM_MENUCOMMAND)
        , KEY_NAME(WM_CHANGEUISTATE)
        , KEY_NAME(WM_UPDATEUISTATE)
        , KEY_NAME(WM_QUERYUISTATE)
        , KEY_NAME(WM_CTLCOLORMSGBOX)
        , KEY_NAME(WM_CTLCOLOREDIT)
        , KEY_NAME(WM_CTLCOLORLISTBOX)
        , KEY_NAME(WM_CTLCOLORBTN)
        , KEY_NAME(WM_CTLCOLORDLG)
        , KEY_NAME(WM_CTLCOLORSCROLLBAR)
        , KEY_NAME(WM_CTLCOLORSTATIC)
        , KEY_NAME(WM_MOUSEFIRST)
        , KEY_NAME(WM_MOUSEMOVE)
        , KEY_NAME(WM_LBUTTONDOWN)
        , KEY_NAME(WM_LBUTTONUP)
        , KEY_NAME(WM_LBUTTONDBLCLK)
        , KEY_NAME(WM_RBUTTONDOWN)
        , KEY_NAME(WM_RBUTTONUP)
        , KEY_NAME(WM_RBUTTONDBLCLK)
        , KEY_NAME(WM_MBUTTONDOWN)
        , KEY_NAME(WM_MBUTTONUP)
        , KEY_NAME(WM_MBUTTONDBLCLK)
        , KEY_NAME(WM_MOUSEWHEEL)
        , KEY_NAME(WM_XBUTTONDOWN)
        , KEY_NAME(WM_XBUTTONUP)
        , KEY_NAME(WM_XBUTTONDBLCLK)
        , KEY_NAME(WM_MOUSEHWHEEL)
        , KEY_NAME(WM_MOUSELAST)
        , KEY_NAME(WM_MOUSELAST)
        , KEY_NAME(WM_MOUSELAST)
        , KEY_NAME(WM_MOUSELAST)
        , KEY_NAME(WM_PARENTNOTIFY)
        , KEY_NAME(WM_ENTERMENULOOP)
        , KEY_NAME(WM_EXITMENULOOP)
        , KEY_NAME(WM_NEXTMENU)
        , KEY_NAME(WM_SIZING)
        , KEY_NAME(WM_CAPTURECHANGED)
        , KEY_NAME(WM_MOVING)
        , KEY_NAME(WM_POWERBROADCAST)
        , KEY_NAME(WM_DEVICECHANGE)
        , KEY_NAME(WM_MDICREATE)
        , KEY_NAME(WM_MDIDESTROY)
        , KEY_NAME(WM_MDIACTIVATE)
        , KEY_NAME(WM_MDIRESTORE)
        , KEY_NAME(WM_MDINEXT)
        , KEY_NAME(WM_MDIMAXIMIZE)
        , KEY_NAME(WM_MDITILE)
        , KEY_NAME(WM_MDICASCADE)
        , KEY_NAME(WM_MDIICONARRANGE)
        , KEY_NAME(WM_MDIGETACTIVE)
        , KEY_NAME(WM_MDISETMENU)
        , KEY_NAME(WM_ENTERSIZEMOVE)
        , KEY_NAME(WM_EXITSIZEMOVE)
        , KEY_NAME(WM_DROPFILES)
        , KEY_NAME(WM_MDIREFRESHMENU)
        , KEY_NAME(WM_POINTERDEVICECHANGE)
        , KEY_NAME(WM_POINTERDEVICEINRANGE)
        , KEY_NAME(WM_POINTERDEVICEOUTOFRANGE)
        , KEY_NAME(WM_TOUCH)
        , KEY_NAME(WM_NCPOINTERUPDATE)
        , KEY_NAME(WM_NCPOINTERDOWN)
        , KEY_NAME(WM_NCPOINTERUP)
        , KEY_NAME(WM_POINTERUPDATE)
        , KEY_NAME(WM_POINTERDOWN)
        , KEY_NAME(WM_POINTERUP)
        , KEY_NAME(WM_POINTERENTER)
        , KEY_NAME(WM_POINTERLEAVE)
        , KEY_NAME(WM_POINTERACTIVATE)
        , KEY_NAME(WM_POINTERCAPTURECHANGED)
        , KEY_NAME(WM_TOUCHHITTESTING)
        , KEY_NAME(WM_POINTERWHEEL)
        , KEY_NAME(WM_POINTERHWHEEL)
        , KEY_NAME(WM_POINTERROUTEDTO)
        , KEY_NAME(WM_POINTERROUTEDAWAY)
        , KEY_NAME(WM_POINTERROUTEDRELEASED)
        , KEY_NAME(WM_IME_SETCONTEXT)
        , KEY_NAME(WM_IME_NOTIFY)
        , KEY_NAME(WM_IME_CONTROL)
        , KEY_NAME(WM_IME_COMPOSITIONFULL)
        , KEY_NAME(WM_IME_SELECT)
        , KEY_NAME(WM_IME_CHAR)
        , KEY_NAME(WM_IME_REQUEST)
        , KEY_NAME(WM_IME_KEYDOWN)
        , KEY_NAME(WM_IME_KEYUP)
        , KEY_NAME(WM_MOUSEHOVER)
        , KEY_NAME(WM_MOUSELEAVE)
        , KEY_NAME(WM_NCMOUSEHOVER)
        , KEY_NAME(WM_NCMOUSELEAVE)
        , KEY_NAME(WM_WTSSESSION_CHANGE)
        , KEY_NAME(WM_TABLET_FIRST)
        , KEY_NAME(WM_TABLET_LAST)
        , KEY_NAME(WM_DPICHANGED)
        , KEY_NAME(WM_DPICHANGED_BEFOREPARENT)
        , KEY_NAME(WM_DPICHANGED_AFTERPARENT)
        , KEY_NAME(WM_GETDPISCALEDSIZE)
        , KEY_NAME(WM_CUT)
        , KEY_NAME(WM_COPY)
        , KEY_NAME(WM_PASTE)
        , KEY_NAME(WM_CLEAR)
        , KEY_NAME(WM_UNDO)
        , KEY_NAME(WM_RENDERFORMAT)
        , KEY_NAME(WM_RENDERALLFORMATS)
        , KEY_NAME(WM_DESTROYCLIPBOARD)
        , KEY_NAME(WM_DRAWCLIPBOARD)
        , KEY_NAME(WM_PAINTCLIPBOARD)
        , KEY_NAME(WM_VSCROLLCLIPBOARD)
        , KEY_NAME(WM_SIZECLIPBOARD)
        , KEY_NAME(WM_ASKCBFORMATNAME)
        , KEY_NAME(WM_CHANGECBCHAIN)
        , KEY_NAME(WM_HSCROLLCLIPBOARD)
        , KEY_NAME(WM_QUERYNEWPALETTE)
        , KEY_NAME(WM_PALETTEISCHANGING)
        , KEY_NAME(WM_PALETTECHANGED)
        , KEY_NAME(WM_HOTKEY)
        , KEY_NAME(WM_PRINT)
        , KEY_NAME(WM_PRINTCLIENT)
        , KEY_NAME(WM_APPCOMMAND)
        , KEY_NAME(WM_THEMECHANGED)
        , KEY_NAME(WM_CLIPBOARDUPDATE)
        , KEY_NAME(WM_DWMCOMPOSITIONCHANGED)
        , KEY_NAME(WM_DWMNCRENDERINGCHANGED)
        , KEY_NAME(WM_DWMCOLORIZATIONCOLORCHANGED)
        , KEY_NAME(WM_DWMWINDOWMAXIMIZEDCHANGE)
        , KEY_NAME(WM_DWMSENDICONICTHUMBNAIL)
        , KEY_NAME(WM_DWMSENDICONICLIVEPREVIEWBITMAP)
        , KEY_NAME(WM_GETTITLEBARINFOEX)
        , KEY_NAME(WM_HANDHELDFIRST)
        , KEY_NAME(WM_HANDHELDLAST)
        , KEY_NAME(WM_AFXFIRST)
        , KEY_NAME(WM_AFXLAST)
        , KEY_NAME(WM_PENWINFIRST)
        , KEY_NAME(WM_PENWINLAST)
        , KEY_NAME(WM_APP)
        , KEY_NAME(WM_USER)

    };
#undef KEY_NAME 

    if (fw && MESSAGE_NAMES_MAP.find(_message) != MESSAGE_NAMES_MAP.end())
        fw->platform.GetLogger()->LogDebug(MESSAGE_NAMES_MAP.at(_message));
#endif // _DEBUG

    switch (_message)
    {
    case WM_SIZE:
    {
        fw->window_process_flags |= WINDOW_PROCESS_FLAG_SIZE;
        switch (_wparam)
        {
        case SIZE_RESTORED:
            fw->platform.GetLogger()->LogDebug("window restored");
            fw->window_state_flags &= ~WINDOW_STATE_FLAG_MINIMIZED;
            fw->window_state_flags &= ~WINDOW_STATE_FLAG_MAXIMIZED;
            break;

        case SIZE_MINIMIZED:
            fw->platform.GetLogger()->LogDebug("window minimized");
            fw->window_state_flags |= WINDOW_STATE_FLAG_MINIMIZED;
            fw->window_process_flags |= WINDOW_PROCESS_FLAG_SIZE_MINIMIZED;
            fw->window_state_flags &= ~WINDOW_STATE_FLAG_MAXIMIZED;
            break;

        case SIZE_MAXIMIZED:
            fw->platform.GetLogger()->LogDebug("window maximized");
            fw->window_state_flags |= WINDOW_STATE_FLAG_MAXIMIZED;
            fw->window_state_flags &= ~WINDOW_STATE_FLAG_MINIMIZED;
            break;

        case SIZE_MAXSHOW:
            fw->platform.GetLogger()->LogDebug("window maxshow");
            fw->window_process_flags |= WINDOW_PROCESS_FLAG_SIZE_MAXSHOW;
            fw->window_state_flags &= ~WINDOW_STATE_FLAG_MINIMIZED;
            fw->window_state_flags &= ~WINDOW_STATE_FLAG_MAXIMIZED;
            break;

        case SIZE_MAXHIDE:
            fw->platform.GetLogger()->LogDebug("window maxhide");
            fw->window_process_flags |= WINDOW_PROCESS_FLAG_SIZE_MAXHIDE;
            fw->window_state_flags &= ~WINDOW_STATE_FLAG_MINIMIZED;
            fw->window_state_flags &= ~WINDOW_STATE_FLAG_MAXIMIZED;
            break;

        default:
            break;
        }

        if (fw->window_state_flags & WINDOW_STATE_FLAG_MINIMIZED)
            break;

        fw->OnResize({ uint32_t(LOWORD(_lparam)), uint32_t(HIWORD(_lparam)) }, fw->swapchain_flags);
        break;
    }
    case WM_MOVE:
    {
        if (!(fw->window_state_flags & WINDOW_STATE_FLAG_FULLSCREEN))
            fw->windowed_offset = { (int32_t)LOWORD(_lparam), (int32_t)HIWORD(_lparam) };
        break;
    }
    case WM_ENTERSIZEMOVE:
    {
        fw->window_state_flags |= WINDOW_STATE_FLAG_IN_SIZEMOVE;
        fw->window_process_flags |= WINDOW_PROCESS_FLAG_SIZEMOVE;
        fw->platform.GetLogger()->LogDebug("enter sizemove");
        break;
    }
    case WM_EXITSIZEMOVE:
    {
        fw->window_state_flags &= ~WINDOW_STATE_FLAG_IN_SIZEMOVE;
        fw->platform.GetLogger()->LogDebug("exit sizemove");
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
        _wparam == TRUE
            ? fw->window_process_flags |= WINDOW_PROCESS_FLAG_ACTIVATED
            : fw->window_process_flags |= WINDOW_PROCESS_FLAG_DEACTIVATED;
        break;
    }
    case WM_ACTIVATE:
    {
        input::MouseInput::GetIns().ProcessMessage(_message, _wparam, _lparam);
        input::KeyboardInput::GetIns().ProcessMessage(_message, _wparam, _lparam);
        switch (_wparam)
        {
        case WA_ACTIVE:
            fw->platform.GetLogger()->LogDebug("window activated");
            fw->window_process_flags |= WINDOW_PROCESS_FLAG_ACTIVATED;
            break;
        case WA_CLICKACTIVE:
            fw->platform.GetLogger()->LogDebug("window click activated");
            fw->window_process_flags |= WINDOW_PROCESS_FLAG_ACTIVATED;
            break;
        case WA_INACTIVE:
            fw->platform.GetLogger()->LogDebug("window deactivated");
            fw->window_process_flags |= WINDOW_PROCESS_FLAG_DEACTIVATED;
            break;
        default:
            break;
        }
        break;
    }
    case WM_CREATE:
    {
        LPCREATESTRUCT create_struct = reinterpret_cast<LPCREATESTRUCT>(_lparam);
        SetWindowLongPtr(_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(create_struct->lpCreateParams));
        SetWindowLong(_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
        SetWindowLong(_hwnd, GWL_EXSTYLE, 0);

        auto lpfw = reinterpret_cast<WindowWindows*>(create_struct->lpCreateParams);
        int width = lpfw->windowed_size.width;
        int height = lpfw->windowed_size.height;

        ShowWindow(_hwnd, SW_SHOWNORMAL);

        int dispx = GetSystemMetrics(SM_CXSCREEN);
        int dispy = GetSystemMetrics(SM_CYSCREEN);

        RECT rw, rc;
        GetWindowRect(_hwnd, &rw);
        GetClientRect(_hwnd, &rc);

        int new_width = (rw.right - rw.left) - (rc.right - rc.left) + width;
        int new_height = (rw.bottom - rw.top) - (rc.bottom - rc.top) + height;

        lpfw->windowed_offset.x = (dispx - width ) / 2 -  (new_width  - width);
        lpfw->windowed_offset.y = (dispy - height) / 2 - ((new_height - height) / 2);
        SetWindowPos(_hwnd, NULL, lpfw->windowed_offset.x, lpfw->windowed_offset.y, new_width, new_height, SWP_NOSIZE | SWP_NOZORDER);
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
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
        input::KeyboardInput::ProcessMessage(_message, _wparam, _lparam);

        if (_wparam == VK_RETURN && (_lparam & 0x60000000) == 0x20000000)
        {
            // Implements the classic ALT+ENTER fullscreen toggle
            if (fw->window_state_flags & WINDOW_STATE_FLAG_FULLSCREEN)
            {
                SetWindowLong(_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
                SetWindowLong(_hwnd, GWL_EXSTYLE, 0);

                fw->window_state_flags &= ~WINDOW_STATE_FLAG_FULLSCREEN;

                SetWindowPos(_hwnd, NULL,
                             fw->windowed_offset.x, fw->windowed_offset.y,
                             fw->windowed_size.width, fw->windowed_size.height,
                             SWP_NOZORDER);
                ShowWindow(_hwnd, SW_SHOWNORMAL);
            }
            else
            {
                SetWindowLong(_hwnd, GWL_STYLE, 0);
                SetWindowLong(_hwnd, GWL_EXSTYLE, WS_EX_TOPMOST);

                fw->window_state_flags |= WINDOW_STATE_FLAG_FULLSCREEN;

                ShowWindow(_hwnd, SW_SHOWMAXIMIZED);
                SetWindowPos(_hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
            }
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
        input::MouseInput::ProcessMessage(_message, _wparam, _lparam);
        break;
    }

    default:
        // WndProc関数内で処理しないメッセージの場合、DefWindowProcを返すことでシステムがデフォルトの処理を行います。
        return DefWindowProc(_hwnd, _message, _wparam, _lparam);
    }

    // WndProc関数内で処理を行った(処理を実装している)メッセージは、0を返します。
    return 0;
}


}// namespace buma
