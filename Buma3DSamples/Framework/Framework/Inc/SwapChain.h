#pragma once

namespace buma
{

class SwapChain
{
public:
    struct SWAP_CHAIN_BUFFER
    {
        buma3d::util::Ptr<buma3d::ITexture>             tex;
        buma3d::util::Ptr<buma3d::IRenderTargetView>    rtv;
        buma3d::util::Ptr<buma3d::IShaderResourceView>  srv;
    };

public:
    SwapChain(std::shared_ptr<DeviceResources>      _device_resources,
              buma3d::util::Ptr<buma3d::ISurface>   _surface,
              buma3d::util::Ptr<buma3d::ISwapChain> _swapchain,
              const buma3d::SWAP_CHAIN_DESC&        _desc);
    ~SwapChain();

    [[nodiscard]]
    virtual uint32_t AcquireNextBuffer(const buma3d::SWAP_CHAIN_ACQUIRE_NEXT_BUFFER_INFO& _info);
    virtual bool     Present(const buma3d::SWAP_CHAIN_PRESENT_INFO& _info);
    virtual bool     Resize(const buma3d::EXTENT2D& _size, buma3d::SWAP_CHAIN_FLAGS _swapchain_flags);
    virtual bool     Recreate(const buma3d::SWAP_CHAIN_DESC& _desc);

    buma3d::util::Ptr<buma3d::ISurface>     GetSurface()    const { return surface; }
    buma3d::util::Ptr<buma3d::ISwapChain>   GetSwapChain()  const { return swapchain; }
    const std::vector<SWAP_CHAIN_BUFFER>&   GetBuffers()    const { return back_buffers; }

protected:
    bool Init(std::shared_ptr<DeviceResources>      _device_resources,
              buma3d::util::Ptr<buma3d::ISurface>   _surface,
              buma3d::util::Ptr<buma3d::ISwapChain> _swapchain,
              const buma3d::SWAP_CHAIN_DESC&        _desc);

    bool GetBackBuffers();
    bool CreateViews();

protected:
    std::shared_ptr<DeviceResources>        device_resources;
    std::vector<buma3d::SURFACE_FORMAT>     supported_formats;
    buma3d::SWAP_CHAIN_DESC                 swapchain_desc;
    buma3d::util::Ptr<buma3d::ISurface>     surface;
    buma3d::util::Ptr<buma3d::ISwapChain>   swapchain;
    std::vector<SWAP_CHAIN_BUFFER>          back_buffers;
    uint32_t                                back_buffer_index;


};

        
}// namespace buma
