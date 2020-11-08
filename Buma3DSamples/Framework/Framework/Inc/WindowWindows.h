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

class PlatformWindows;

class WindowWindows : public WindowBase
{
public:
    WindowWindows(PlatformWindows&                  _platform,
                  WNDCLASSEXW&                      _wnd_class,
                  uint32_t                          _back_buffer_count,
                  const buma3d::EXTENT2D&           _size,
                  const char*                       _window_name,
                  buma3d::RESOURCE_FORMAT           _format          = buma3d::RESOURCE_FORMAT_B8G8R8A8_UNORM,
                  buma3d::SWAP_CHAIN_BUFFER_FLAGS   _buffer_flags    = buma3d::FRAMEBUFFER_FLAG_NONE,
                  buma3d::SWAP_CHAIN_FLAGS          _swapchain_flags = buma3d::SWAP_CHAIN_FLAG_NONE);

    WindowWindows(const WindowWindows&) = delete;
    virtual ~WindowWindows();
    
    virtual bool Resize(const buma3d::EXTENT2D& _size, buma3d::SWAP_CHAIN_FLAGS _swapchain_flags = buma3d::SWAP_CHAIN_FLAG_NONE) override;
    static LRESULT CALLBACK WndProc(HWND _hwnd, UINT _message, WPARAM _wparam, LPARAM _lparam);

    bool ProcessMessage() override;
    bool Exit() override;

    WINDOW_STATE_FLAGS GetEWindowStateFlags() const { return window_state_flags; }

protected:
    bool Init(PlatformBase&                     _platform,
              uint32_t                          _back_buffer_count,
              const buma3d::EXTENT2D&           _size,
              const char*                       _window_name,
              buma3d::RESOURCE_FORMAT           _format             = buma3d::RESOURCE_FORMAT_UNKNOWN,
              buma3d::SWAP_CHAIN_BUFFER_FLAGS   _buffer_flags       = buma3d::FRAMEBUFFER_FLAG_NONE, 
              buma3d::SWAP_CHAIN_FLAGS          _swapchain_flags    = buma3d::SWAP_CHAIN_FLAG_NONE) override;

private:
    bool CreateWnd(uint32_t _width, uint32_t _height);
    bool CreateSurface();
    bool CreateSwapChain();

    bool GetBackBuffers();

private:
    PlatformWindows&                    platform;
    const WNDCLASSEXW&                  wnd_class;
    HWND                                hwnd;
    std::string                         wnd_name;
    WINDOW_STATE_FLAGS                  window_state_flags;
    buma3d::EXTENT2D                    windowed_size;
    float                               aspect_ratio;
    MSG                                 msg;

    buma3d::SWAP_CHAIN_DESC                             swapchain_desc;
    std::vector<buma3d::SURFACE_FORMAT>                 supported_formats;
    std::vector<buma3d::util::Ptr<buma3d::ITexture>>    back_buffers;

};


}// namespace buma
