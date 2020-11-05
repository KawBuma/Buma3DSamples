#pragma once

namespace buma
{

class WindowWindows : public WindowBase
{
public:
    inline static constexpr const wchar_t* WND_CLASS_NAME = L"Buma3DSamples Framework";

public:
    WindowWindows(PlatformWindows& _platform)
        : WindowBase()
        , platform  { _platform }
        , hwnd      {}
        , wnd_name  {}
    {

    }

    ~WindowWindows()
    {
        if (hwnd)
            DestroyWindow(hwnd);
        hwnd = NULL;
    }
    
    bool Resize(const buma3d::UINT2& _size) override;

protected:
    bool Init(PlatformBase&                     _platform,
              uint32_t                          _back_buffer_count,
              const buma3d::EXTENT2D&           _size,
              const char*                       _window_name,
              buma3d::RESOURCE_FORMAT           _format         = buma3d::RESOURCE_FORMAT_UNKNOWN,
              buma3d::SWAP_CHAIN_BUFFER_FLAGS   _buffer_flags   = false) override;

private:
    bool CreateWnd(uint32_t _width, uint32_t _height);
    bool CreateSwapSurface();
    bool CreateSwapChain();

private:
    PlatformWindows&                    platform;
    HWND                                hwnd;
    std::string                         wnd_name;

};


}// namespace buma
