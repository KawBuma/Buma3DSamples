#pragma once

namespace buma
{

enum WINDOW_STATE_FLAGS
{
    WINDOW_STATE_FLAG_SIZE_RESTORED     = 0x1,
    WINDOW_STATE_FLAG_SIZE_MINIMIZED    = 0x2,
    WINDOW_STATE_FLAG_SIZE_MAXIMIZED    = 0x4,
    WINDOW_STATE_FLAG_SIZE_MAXSHOW      = 0x8,
    WINDOW_STATE_FLAG_SIZE_MAXHIDE      = 0x10,

    WINDOW_STATE_FLAG_IN_SIZEMOVE       = 0x20,

    WINDOW_STATE_FLAG_FULLSCREEN        = 0x40,

    WINDOW_STATE_FLAG_ACTIVATED         = 0x80,
    WINDOW_STATE_FLAG_DEACTIVATED       = 0x100,

    WINDOW_STATE_FLAG_EXIT              = 0x200,

};
DEFINE_ENUM_FLAG_OPERATORS(WINDOW_STATE_FLAGS);


class WindowWindows : public WindowBase
{
public:
    inline static constexpr const wchar_t* WND_CLASS_NAME = L"Buma3DSamples Framework";

public:
    WindowWindows(PlatformWindows&                  _platform,
                  uint32_t                          _back_buffer_count,
                  const buma3d::EXTENT2D&           _size,
                  const char*                       _window_name,
                  buma3d::RESOURCE_FORMAT           _format            = buma3d::RESOURCE_FORMAT_UNKNOWN,
                  buma3d::SWAP_CHAIN_BUFFER_FLAGS   _buffer_flags      = buma3d::FRAMEBUFFER_FLAG_NONE)   
        : WindowBase()
        , platform  { _platform }
        , hwnd      {}
        , wnd_name  {}
    {
        Init(_platform, _back_buffer_count, _size, _window_name, _format, _buffer_flags);
    }

    ~WindowWindows()
    {
        if (hwnd)
            DestroyWindow(hwnd);
        hwnd = NULL;
    }
    
    bool Resize(const buma3d::EXTENT2D& _size) override;
    friend static LRESULT CALLBACK WndProc(HWND _hwnd, UINT _message, WPARAM _wparam, LPARAM _lparam);

    bool ProcessMessage() override;
    bool Exit() override;

    WINDOW_STATE_FLAGS GetEWindowStateFlags() const { return window_state_flags; }

protected:
    bool Init(PlatformBase&                     _platform,
              uint32_t                          _back_buffer_count,
              const buma3d::EXTENT2D&           _size,
              const char*                       _window_name,
              buma3d::RESOURCE_FORMAT           _format            = buma3d::RESOURCE_FORMAT_UNKNOWN,
              buma3d::SWAP_CHAIN_BUFFER_FLAGS   _buffer_flags      = buma3d::FRAMEBUFFER_FLAG_NONE) override;

private:
    bool RegisterWndClass();
    bool CreateWnd(uint32_t _width, uint32_t _height);
    bool CreateSurface();
    bool CreateSwapChain();

private:
    PlatformWindows&                    platform;
    HWND                                hwnd;
    std::string                         wnd_name;
    WINDOW_STATE_FLAGS                  window_state_flags;
    buma3d::EXTENT2D                    windowed_size;

};


}// namespace buma
