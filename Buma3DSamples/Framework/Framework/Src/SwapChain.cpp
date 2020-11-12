#include "pch.h"
#include "SwapChain.h"

namespace buma
{

SwapChain::SwapChain(std::shared_ptr<DeviceResources> _device_resources, buma3d::util::Ptr<buma3d::ISurface> _surface, buma3d::util::Ptr<buma3d::ISwapChain> _swapchain, const buma3d::SWAP_CHAIN_DESC& _desc)
    : supported_formats         {}
    , swapchain_desc            {}
    , surface                   {}
    , swapchain                 {}
    , back_buffers              {}
    , back_buffer_index         {}
    , acquire_info              {}
    , present_complete_fences   {}
{
    Init(_device_resources, _surface, _swapchain, _desc);
}

SwapChain::~SwapChain()
{
    swapchain->GetDevice()->WaitIdle();
    if (swapchain)
        return;

    back_buffers = {};
}

buma3d::BMRESULT SwapChain::AcquireNextBuffer(uint32_t _timeout_millisec, uint32_t* _dst_back_buffer_index)
{
    acquire_info.signal_fence        = present_complete_fences.signal_fence.Get();
    acquire_info.signal_fence_to_cpu = present_complete_fences.signal_fence_to_cpu.Get();
    acquire_info.timeout_millisec    = _timeout_millisec;

    uint32_t next_buffer_index = 0;
    auto bmr = swapchain->AcquireNextBuffer(acquire_info, &next_buffer_index);
    if (bmr >= buma3d::BMRESULT_FAILED)
        return bmr;

    back_buffer_index = next_buffer_index;
    *_dst_back_buffer_index = next_buffer_index;
    return bmr;
}

buma3d::BMRESULT SwapChain::Present(const buma3d::SWAP_CHAIN_PRESENT_INFO& _info)
{
    return swapchain->Present(_info);
}

bool SwapChain::Resize(const buma3d::EXTENT2D& _size, buma3d::SWAP_CHAIN_FLAGS _swapchain_flags)
{
    auto tmp = swapchain_desc;
    tmp.buffer.width  = _size.width;
    tmp.buffer.height = _size.height;
    tmp.flags         = _swapchain_flags;
    if (!Recreate(tmp)) return false;

    return true;
}

bool SwapChain::Recreate(const buma3d::SWAP_CHAIN_DESC& _desc)
{
    swapchain_desc = _desc;
    back_buffers.clear();
    back_buffer_index = 0;

    if (swapchain->Recreate(swapchain_desc) >= buma3d::BMRESULT_FAILED)
        return false;

    if (!GetBackBuffers())  return false;
    if (!CreateViews())     return false;

    return true;
}

bool SwapChain::Init(std::shared_ptr<DeviceResources> _device_resources, buma3d::util::Ptr<buma3d::ISurface> _surface, buma3d::util::Ptr<buma3d::ISwapChain> _swapchain, const buma3d::SWAP_CHAIN_DESC& _desc)
{
    device_resources = _device_resources;
    surface          = _surface;
    swapchain        = _swapchain;
    swapchain_desc   = _desc;

    if (!GetBackBuffers())  return false;
    if (!CreateViews())     return false;
    if (!CreatePresentCompleteFences()) return false;

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
        if (tdesc.texture.usage & buma3d::TEXTURE_USAGE_FLAG_SHADER_RESOURCE)
        {
            buma3d::SHADER_RESOURCE_VIEW_DESC srvdesc = buma3d::hlp::init::ShaderResourceViewDescDescTex2D(tdesc.texture.format_desc.format);
            auto bmr = device->CreateShaderResourceView(i.tex.Get(), srvdesc, &i.srv);
            if (bmr >= buma3d::BMRESULT_FAILED)
                return false;
        }
    }
    return true;
}

bool SwapChain::CreatePresentCompleteFences()
{
    auto&& device = device_resources->GetDevice();

    auto bmr = device->CreateFence(buma3d::hlp::init::BinaryFenceDesc(), &present_complete_fences.signal_fence);
    if (bmr >= buma3d::BMRESULT_FAILED)
        return false;

    bmr = device->CreateFence(buma3d::hlp::init::BinaryCpuFenceDesc(), &present_complete_fences.signal_fence_to_cpu);
    if (bmr >= buma3d::BMRESULT_FAILED)
        return false;

    acquire_info.timeout_millisec = 0;
    return true;
}


}// namespace buma
