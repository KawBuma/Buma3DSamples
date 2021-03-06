#include "pch.h"
#include "SwapChain.h"

namespace buma
{
SwapChain::SwapChain()
    : supported_formats         {}
    , swapchain_desc            {}
    , surface                   {}
    , swapchain                 {}
    , back_buffers              {}
    , back_buffer_index         {}
    , acquire_info              {}
    , present_complete_fences   {}
    , is_signaled_cpu_fence     {}
{

}

SwapChain::SwapChain(std::shared_ptr<DeviceResources> _device_resources, buma3d::util::Ptr<buma3d::ISurface> _surface, buma3d::util::Ptr<buma3d::ISwapChain> _swapchain, const buma3d::SWAP_CHAIN_DESC& _desc)
    : supported_formats         {}
    , swapchain_desc            {}
    , surface                   {}
    , swapchain                 {}
    , back_buffers              {}
    , back_buffer_index         {}
    , acquire_info              {}
    , present_complete_fences   {}
    , is_signaled_cpu_fence     {}
{
    Init(_device_resources, _surface, _swapchain, _desc);
}

SwapChain::~SwapChain()
{
    if (swapchain)
        swapchain->GetDevice()->WaitIdle();

    back_buffers = {};
    swapchain.Reset();
    surface.Reset();
    present_complete_fences.signal_fence.Reset();
    present_complete_fences.signal_fence_to_cpu.Reset();
}

buma3d::BMRESULT SwapChain::AcquireNextBuffer(uint32_t _timeout_millisec, uint32_t* _dst_back_buffer_index, bool _use_custom_fences, const buma3d::SWAP_CHAIN_ACQUIRE_NEXT_BUFFER_INFO* _acquire_info)
{
    if (_use_custom_fences)
    {
        if (_acquire_info)
            acquire_info = *_acquire_info;
        else 
        {
            // _acquire_infoもnullptrの場合、GPU_TO_GPUフェンスのみを設定します。
            acquire_info.signal_fence        = present_complete_fences.signal_fence.Get();
            acquire_info.signal_fence_to_cpu = nullptr;
        }
        acquire_info.timeout_millisec = _timeout_millisec;
    }
    else
    {
        acquire_info.signal_fence        = present_complete_fences.signal_fence.Get();
        acquire_info.signal_fence_to_cpu = present_complete_fences.signal_fence_to_cpu.Get();
        acquire_info.timeout_millisec    = _timeout_millisec;
        is_signaled_cpu_fence = true;
    }

    uint32_t next_buffer_index = 0;
    auto bmr = swapchain->AcquireNextBuffer(acquire_info, &next_buffer_index);
    if (bmr >= buma3d::BMRESULT_FAILED)
        return bmr;

    back_buffer_index = next_buffer_index;
    *_dst_back_buffer_index = next_buffer_index;
    return bmr;
}

bool SwapChain::WaitDefaultCpuFence()
{
    if (!is_signaled_cpu_fence)
        return true;

    auto bmr = present_complete_fences.signal_fence_to_cpu->Wait(0, UINT32_MAX);
    BMR_RET_IF_FAILED(bmr);
    bmr = present_complete_fences.signal_fence_to_cpu->Reset();
    BMR_RET_IF_FAILED(bmr);

    is_signaled_cpu_fence = false;
    return true;
}

buma3d::BMRESULT SwapChain::Present(const buma3d::SWAP_CHAIN_PRESENT_INFO& _info, bool _use_custom_fences)
{
    WaitDefaultCpuFence();

    if (_use_custom_fences)
        return swapchain->Present(_info);
    else
    {
        buma3d::SWAP_CHAIN_PRESENT_INFO info = _info;
        info.wait_fence = present_complete_fences.signal_fence.Get();
        return swapchain->Present(info);
    }
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

bool SwapChain::Create(std::shared_ptr<DeviceResources> _device_resources, const buma3d::SURFACE_DESC& _surface_desc, const buma3d::SWAP_CHAIN_DESC& _desc)
{
    back_buffers = {};
    swapchain.Reset();
    surface.Reset();
    supported_formats.clear();

    buma3d::util::Ptr<buma3d::ISurface> sf;
    buma3d::util::Ptr<buma3d::ISwapChain> sc;

    auto bmr = _device_resources->GetAdapter()->CreateSurface(_surface_desc, &sf);
    BMR_RET_IF_FAILED(bmr);

    supported_formats.resize(sf->GetSupportedSurfaceFormats(nullptr));
    sf->GetSupportedSurfaceFormats(supported_formats.data());

    // 指定のフォーマットが見つからなければデフォルトのフォーマットを設定
    buma3d::SWAP_CHAIN_DESC desc = _desc;
    auto it_find = std::find_if(supported_formats.begin(), supported_formats.end(), [&](const buma3d::SURFACE_FORMAT& _f)
    { return (_f.format == desc.buffer.format_desc.format && _f.color_space == desc.color_space); });
    if (it_find == supported_formats.end())
    {
        desc.buffer.format_desc.format = supported_formats.back().format;
        desc.color_space               = supported_formats.back().color_space;// FIXME: supported_formats
    }

    desc.surface = sf.Get();
    bmr = _device_resources->GetDevice()->CreateSwapChain(desc, &sc);
    BMR_RET_IF_FAILED(bmr);

    return Init(_device_resources, sf, sc, desc);
}

bool SwapChain::SetName(const char* _name)
{
    swapchain->SetName((std::string("SwapChain ") + '('+_name+')').c_str());
    for (uint32_t i = 0, size = (uint32_t)back_buffers.size(); i < size; i++)
        back_buffers[i].tex->SetName((std::string("SwapChain buffer") + std::to_string(i) + '('+_name+')').c_str());

    return true;
}

bool SwapChain::Init(std::shared_ptr<DeviceResources> _device_resources, buma3d::util::Ptr<buma3d::ISurface> _surface, buma3d::util::Ptr<buma3d::ISwapChain> _swapchain, const buma3d::SWAP_CHAIN_DESC& _desc)
{
    device_resources = _device_resources;
    surface          = _surface;
    swapchain        = _swapchain;
    swapchain_desc   = _desc;
    swapchain_desc.surface = _surface.Get();

    if (supported_formats.empty())
    {
        supported_formats.resize(surface->GetSupportedSurfaceFormats(nullptr));
        surface->GetSupportedSurfaceFormats(supported_formats.data());
    }

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
    auto&& device = device_resources->GetDevice();
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
            if (!(tdesc.texture.usage & buma3d::TEXTURE_USAGE_FLAG_INPUT_ATTACHMENT))
                srvdesc.flags |= buma3d::SHADER_RESOURCE_VIEW_FLAG_DENY_INPUT_ATTACHMENT;
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

    present_complete_fences.signal_fence       ->SetName("SwapChain::present_complete_fences.signal_fence");
    present_complete_fences.signal_fence_to_cpu->SetName("SwapChain::present_complete_fences.signal_fence_to_cpu");
    acquire_info.timeout_millisec = 0;
    return true;
}


}// namespace buma
