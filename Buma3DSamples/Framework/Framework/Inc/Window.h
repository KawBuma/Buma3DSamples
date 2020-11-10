#pragma once

namespace buma
{

class WindowBase
{
public:
    WindowBase();
    WindowBase(const WindowBase&) = delete;
    virtual ~WindowBase();

    virtual bool Resize(const buma3d::EXTENT2D& _size, buma3d::SWAP_CHAIN_FLAGS _swapchain_flags) = 0;

    virtual bool ProcessMessage() = 0;
    virtual bool Exit() = 0;

    virtual bool CreateSwapChain(const buma3d::SWAP_CHAIN_DESC& _desc, std::shared_ptr<buma::SwapChain>* _dst) = 0;
    virtual const std::vector<buma3d::SURFACE_FORMAT>& GetSupportedFormats() const = 0;

protected:
    virtual bool Init(PlatformBase&             _platform,
                      const buma3d::EXTENT2D&   _size,
                      const char*               _window_name) = 0;

};


}// namespace buma
