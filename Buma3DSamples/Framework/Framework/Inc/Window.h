#pragma once

namespace buma
{

class WindowBase
{
public:
    struct SWAP_CHAIN_BUFFER
    {
        buma3d::util::Ptr<buma3d::ITexture>             tex;
        buma3d::util::Ptr<buma3d::IRenderTargetView>    rtv;
        buma3d::util::Ptr<buma3d::IShaderResourceView>  srv;
    };

public:
    WindowBase();
    WindowBase(const WindowBase&) = delete;
    virtual ~WindowBase();

    [[nodiscard]]
    virtual uint32_t AcquireNextBuffer(const buma3d::SWAP_CHAIN_ACQUIRE_NEXT_BUFFER_INFO& _info);
    virtual bool     Present(const buma3d::SWAP_CHAIN_PRESENT_INFO& _info);
    virtual bool     Resize(const buma3d::EXTENT2D& _size) = 0;

    virtual bool     ProcessMessage() = 0;
    virtual bool     Exit() = 0;

    buma3d::util::Ptr<buma3d::ISurface>     GetSurface()    const { return surface; }
    buma3d::util::Ptr<buma3d::ISwapChain>   GetSwapChain()  const { return swapchain; }
    const std::vector<SWAP_CHAIN_BUFFER>&   GetBuffers()    const { return back_buffers; }

protected:
    virtual bool Init(PlatformBase&                     _platform,
                      uint32_t                          _back_buffer_count,
                      const buma3d::EXTENT2D&           _size,
                      const char*                       _window_name,
                      buma3d::RESOURCE_FORMAT           _format         = buma3d::RESOURCE_FORMAT_UNKNOWN,
                      buma3d::SWAP_CHAIN_BUFFER_FLAGS   _buffer_flags   = buma3d::FRAMEBUFFER_FLAG_NONE) = 0;

protected:
    buma3d::util::Ptr<buma3d::ISurface>     surface;
    buma3d::util::Ptr<buma3d::ISwapChain>   swapchain;
    std::vector<SWAP_CHAIN_BUFFER>          back_buffers;
    uint32_t                                back_buffer_index;

};


}// namespace buma
