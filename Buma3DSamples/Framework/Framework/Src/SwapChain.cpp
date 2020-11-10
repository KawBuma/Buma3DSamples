#include "pch.h"
#include "SwapChain.h"

namespace buma
{

SwapChain::SwapChain(std::shared_ptr<DeviceResources> _device_resources, buma3d::util::Ptr<buma3d::ISurface> _surface, buma3d::util::Ptr<buma3d::ISwapChain> _swapchain, const buma3d::SWAP_CHAIN_DESC& _desc)
    : supported_formats     {}
    , swapchain_desc        {}
    , surface               {}
    , swapchain             {}
    , back_buffers          {}
    , back_buffer_index     {}
{
    Init(_device_resources, _surface, _swapchain, _desc);
}

SwapChain::~SwapChain()
{
    if (swapchain)
        return;

    back_buffers = {};
    swapchain->GetDevice()->WaitIdle();
}

uint32_t SwapChain::AcquireNextBuffer(const buma3d::SWAP_CHAIN_ACQUIRE_NEXT_BUFFER_INFO& _info)
{
    uint32_t next_buffer_index = 0;
    auto bmr = swapchain->AcquireNextBuffer(_info, &next_buffer_index);
    if (bmr != buma3d::BMRESULT_SUCCEED)
        return ~0u;

    back_buffer_index = next_buffer_index;
    return back_buffer_index;
}

bool SwapChain::Present(const buma3d::SWAP_CHAIN_PRESENT_INFO& _info)
{
    auto bmr = swapchain->Present(_info);
    return bmr == buma3d::BMRESULT_SUCCEED ? true : false;
}

bool SwapChain::Resize(const buma3d::EXTENT2D& _size, buma3d::SWAP_CHAIN_FLAGS _swapchain_flags)
{
    swapchain_desc.buffer.width  = _size.width;
    swapchain_desc.buffer.height = _size.height;
    swapchain_desc.flags         = _swapchain_flags;
    auto bmr = swapchain->Recreate(swapchain_desc);

    return bmr == buma3d::BMRESULT_SUCCEED;
}

bool SwapChain::Recreate(const buma3d::SWAP_CHAIN_DESC& _desc)
{
    auto bmr = swapchain->Recreate(_desc);
    return bmr == buma3d::BMRESULT_SUCCEED;
}

bool SwapChain::Init(std::shared_ptr<DeviceResources> _device_resources, buma3d::util::Ptr<buma3d::ISurface> _surface, buma3d::util::Ptr<buma3d::ISwapChain> _swapchain, const buma3d::SWAP_CHAIN_DESC& _desc)
{
    device_resources = _device_resources;
    surface          = _surface;
    swapchain        = _swapchain;
    swapchain_desc   = _desc;

    if (!GetBackBuffers())  return false;
    if (!CreateViews())     return false;

    return true;
}

bool SwapChain::GetBackBuffers()
{
    auto&& scd = swapchain->GetDesc();
    back_buffers.resize(scd.buffer.count);
    for (uint32_t i = 0; i < scd.buffer.count; i++)
    {
        auto&& bb = back_buffers[i];
        
        auto bmr = swapchain->GetBuffer(i, &bb.tex);
        if (bmr >= buma3d::BMRESULT_FAILED)
            return false;

        bb.tex->SetName((std::string("SwapChain buffer") + std::to_string(i)).c_str());
    }
    return true;
}

bool SwapChain::CreateViews()
{
    auto device = device_resources->GetDevice();
    for (auto& i : back_buffers)
    {
        auto&& tdesc = i.tex->GetDesc();
        if (tdesc.texture.usage & buma3d::TEXTURE_USAGE_FLAG_COLOR_ATTACHMENT)
        {
            buma3d::RENDER_TARGET_VIEW_DESC rtvdesc = buma3d::hlp::init::RenderTargetViewDescTex2D(tdesc.texture.format_desc.format);
            auto bmr = device->CreateRenderTargetView(i.tex.Get(), rtvdesc, &i.rtv);
            if (bmr >= buma3d::BMRESULT_FAILED)
                return false;
        }
        if (tdesc.texture.usage & buma3d::TEXTURE_USAGE_FLAG_COLOR_ATTACHMENT)
        {
            buma3d::SHADER_RESOURCE_VIEW_DESC srvdesc = buma3d::hlp::init::ShaderResourceViewDescDescTex2D(tdesc.texture.format_desc.format);
            auto bmr = device->CreateShaderResourceView(i.tex.Get(), srvdesc, &i.srv);
            if (bmr >= buma3d::BMRESULT_FAILED)
                return false;
        }
    }
}

}// namespace buma
