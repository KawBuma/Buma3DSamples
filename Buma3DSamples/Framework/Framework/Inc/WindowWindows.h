#pragma once

namespace buma
{

enum WINDOW_STATE_FLAGS
{
    WINDOW_STATE_FLAG_NONE          = 0x0,
    WINDOW_STATE_FLAG_MINIMIZED     = 0x1,
    WINDOW_STATE_FLAG_MAXIMIZED     = 0x2,
    WINDOW_STATE_FLAG_FULLSCREEN    = 0x4,
    WINDOW_STATE_FLAG_IN_SIZEMOVE   = 0x8,
};
DEFINE_ENUM_FLAG_OPERATORS(WINDOW_STATE_FLAGS);

enum WINDOW_PROCESS_FLAGS
{
    WINDOW_PROCESS_FLAG_NONE            = 0x0,
    WINDOW_PROCESS_FLAG_SIZE            = 0x1,
    WINDOW_PROCESS_FLAG_SIZE_MINIMIZED  = 0x2,
    WINDOW_PROCESS_FLAG_SIZE_MAXIMIZED  = 0x4,
    WINDOW_PROCESS_FLAG_SIZE_MAXSHOW    = 0x8,
    WINDOW_PROCESS_FLAG_SIZE_MAXHIDE    = 0x10,

    WINDOW_PROCESS_FLAG_SIZEMOVE        = 0x20,

    WINDOW_PROCESS_FLAG_ACTIVATED       = 0x40,
    WINDOW_PROCESS_FLAG_DEACTIVATED     = 0x80,

    WINDOW_PROCESS_FLAG_EXIT            = 0x100,
};
DEFINE_ENUM_FLAG_OPERATORS(WINDOW_PROCESS_FLAGS);

class PlatformWindows;

class WindowWindows : public WindowBase
{
public:
    WindowWindows(PlatformWindows&      _platform,
                  WNDCLASSEXW&          _wnd_class,
                  const WINDOW_DESC&    _desc);

    WindowWindows(const WindowWindows&) = delete;
    virtual ~WindowWindows();
    
    bool OffsetWindow(const buma3d::OFFSET2D& _offset) override;
    bool ResizeWindow(const buma3d::EXTENT2D& _size, buma3d::SWAP_CHAIN_FLAGS _swapchain_flags) override;
    static LRESULT CALLBACK WndProc(HWND _hwnd, UINT _message, WPARAM _wparam, LPARAM _lparam);

    bool ProcessMessage() override;
    bool Exit()           override;

    WINDOW_STATE_FLAGS      GetWindowStateFlags() const { return window_state_flags; }
    float                   GetAspectRatio()      const override { return aspect_ratio; }
    const buma3d::EXTENT2D& GetWindowedSize()     const override { return windowed_size; }
    const buma3d::OFFSET2D& GetWindowedOffset()   const override { return windowed_offset; }

    bool CreateSwapChain(const buma3d::SWAP_CHAIN_DESC& _desc, std::shared_ptr<buma::SwapChain>* _dst) override;
    const std::vector<buma3d::SURFACE_FORMAT>& GetSupportedFormats() const override;

protected:
    bool Init(PlatformBase&             _platform,
              const buma3d::EXTENT2D&   _size,
              const char*               _window_name) override;

private:
    bool CreateWnd(uint32_t _width, uint32_t _height);
    bool CreateSurface();
    bool OnResize(const buma3d::EXTENT2D& _size, buma3d::SWAP_CHAIN_FLAGS _swapchain_flags);
    bool ResizeBuffers(const buma3d::EXTENT2D& _size, buma3d::SWAP_CHAIN_FLAGS _swapchain_flags);

private:
    PlatformWindows&                        platform;
    const WNDCLASSEXW&                      wnd_class;
    HWND                                    hwnd;
    std::string                             wnd_name;
    WINDOW_STATE_FLAGS                      window_state_flags;
    WINDOW_PROCESS_FLAGS                    window_process_flags;
    buma3d::EXTENT2D                        windowed_size;
    buma3d::OFFSET2D                        windowed_offset;
    float                                   aspect_ratio;
    MSG                                     msg;

    buma3d::util::Ptr<buma3d::ISurface>     surface;
    std::vector<buma3d::SURFACE_FORMAT>     supported_formats;
    std::shared_ptr<SwapChain>              swapchain;
    buma3d::SWAP_CHAIN_FLAGS                swapchain_flags;

};


}// namespace buma
