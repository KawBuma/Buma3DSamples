#pragma once

namespace buma
{

class ResizeEventArgs : public IEventArgs
{
public:
    ResizeEventArgs(const buma3d::EXTENT2D _size, const buma3d::SWAP_CHAIN_FLAGS _swapchain_flag)
        : size           { _size }
        , swapchain_flag { _swapchain_flag }
    {}
    virtual ~ResizeEventArgs() {}

public:
    const buma3d::EXTENT2D            size;
    const buma3d::SWAP_CHAIN_FLAGS    swapchain_flag;

};
using BufferResizedEventArgs = ResizeEventArgs;

enum WINDOW_STATE_FLAGS
{
    WINDOW_STATE_FLAG_NONE          = 0x0,
    WINDOW_STATE_FLAG_MINIMIZED     = 0x1,
    WINDOW_STATE_FLAG_MAXIMIZED     = 0x2,
    WINDOW_STATE_FLAG_FULLSCREEN    = 0x4,
    WINDOW_STATE_FLAG_IN_SIZEMOVE   = 0x8,
};

class WindowBase
{
public:
    WindowBase();
    WindowBase(const WindowBase&) = delete;
    virtual ~WindowBase();

    virtual bool OffsetWindow(const buma3d::OFFSET2D& _offset) = 0;
    virtual bool ResizeWindow(const buma3d::EXTENT2D& _size, buma3d::SWAP_CHAIN_FLAGS _swapchain_flags) = 0;

    virtual bool ProcessMessage() = 0;
    virtual bool Exit() = 0;

    virtual bool CreateSwapChain(const buma3d::SWAP_CHAIN_DESC& _desc, std::shared_ptr<buma::SwapChain>* _dst) = 0;
    virtual const std::vector<buma3d::SURFACE_FORMAT>& GetSupportedFormats() const = 0;

    virtual void AddResizeEvent(std::weak_ptr<IEvent> _event) const = 0;
    virtual void AddBufferResizedEvent(std::weak_ptr<IEvent> _event) const = 0;

    virtual WINDOW_STATE_FLAGS      GetWindowStateFlags()   const = 0;
    virtual float                   GetAspectRatio()        const = 0;
    virtual const buma3d::EXTENT2D& GetWindowedSize()       const = 0;
    virtual const buma3d::OFFSET2D& GetWindowedOffset()     const = 0;

protected:
    virtual bool Init(PlatformBase&             _platform,
                      const buma3d::EXTENT2D&   _size,
                      const char*               _window_name) = 0;

};


}// namespace buma
