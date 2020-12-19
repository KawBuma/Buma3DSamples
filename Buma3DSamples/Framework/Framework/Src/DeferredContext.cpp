#include "pch.h"
#include "DeferredContext.h"

namespace buma
{

DeferredContext::DeferredContext()
    : dr                {}
    , fence_to_cpu      {}
    , queue             {}
    , allocator         {}
    , list              {}
    , upload_buffer     {}
    , readback_buffer   {}
    , submit_info       {}
    , submit            {}
    , dummy             {}
{
}

DeferredContext::~DeferredContext()
{
    queue->WaitIdle();
}

bool DeferredContext::Init(std::shared_ptr<DeviceResources> _dr, buma3d::util::Ptr<buma3d::ICommandQueue> _que)
{
    dr = _dr;
    queue = _que;
    auto d = queue->GetDevice();

    if (util::IsFailed(d->CreateFence(buma3d::hlp::init::BinaryCpuFenceDesc(), &fence_to_cpu)))
        return false;
    if (util::IsFailed(d->CreateFence(buma3d::hlp::init::BinaryFenceDesc(), &fence_to_gpu)))
        return false;

    if (util::IsFailed(d->CreateCommandAllocator(buma3d::hlp::init::CommandAllocatorDesc(queue->GetDesc().type, buma3d::COMMAND_LIST_LEVEL_PRIMARY, buma3d::COMMAND_ALLOCATOR_FLAG_TRANSIENT), &allocator)))
        return false;
    if (util::IsFailed(d->AllocateCommandList(buma3d::hlp::init::CommandListDesc(allocator.Get(), buma3d::B3D_DEFAULT_NODE_MASK), &list)))
        return false;

    type = queue->GetDesc().type;

    allocator->SetName("DeferredContext::allocator");
    list->SetName("DeferredContext::list");
    fence_to_cpu->SetName("DeferredContext::fence_to_cpu");
    fence_to_gpu->SetName("DeferredContext::fence_to_gpu");

    upload_buffer   = std::make_unique<StagingBufferPool>(_dr, buma3d::RESOURCE_HEAP_PROPERTY_FLAG_HOST_WRITABLE, buma3d::hlp::init::BUF_COPYABLE_FLAGS);
    readback_buffer = std::make_unique<StagingBufferPool>(_dr, buma3d::RESOURCE_HEAP_PROPERTY_FLAG_HOST_READABLE, buma3d::hlp::init::BUF_COPYABLE_FLAGS);

    submit_info.num_command_lists_to_execute    = 1;
    submit_info.command_lists_to_execute        = list.GetAddressOf();

    submit_info.signal_fence.num_fences     = 1;
    submit_info.signal_fence.fences         = fence_to_gpu.GetAddressOf();
    submit_info.signal_fence.fence_values   = &dummy;
    submit_info.wait_fence.fence_values     = &dummy;

    submit.num_submit_infos     = 1;
    submit.submit_infos         = &submit_info;
    submit.signal_fence_to_cpu  = fence_to_cpu.Get();

    Signal(fence_to_gpu);

    return true;
}

void DeferredContext::Reset()
{
    auto bmr = allocator->Reset(buma3d::COMMAND_ALLOCATOR_RESET_FLAG_NONE);
    BMR_ASSERT_IF_FAILED(bmr);

    upload_buffer->ResetPages();
    readback_buffer->ResetPages();

    bmr = fence_to_cpu->Reset();
    BMR_ASSERT_IF_FAILED(bmr);
    resetted = true;
}

void DeferredContext::Begin()
{
    if (!resetted)
        Reset();
    auto bmr = list->BeginRecord({ buma3d::COMMAND_LIST_BEGIN_FLAG_ONE_TIME_SUBMIT });
    BMR_ASSERT_IF_FAILED(bmr);
}

const buma3d::util::Ptr<buma3d::IFence>& DeferredContext::End(const buma3d::util::Ptr<buma3d::IFence>& _wait_fence_to_gpu)
{
    auto bmr = list->EndRecord();
    BMR_ASSERT_IF_FAILED(bmr);

    // コマンド送信前に、ホストの書き込みを確定させます。
    upload_buffer->MakeVisible();

    submit_info.wait_fence.num_fences   = _wait_fence_to_gpu ? 1 : 0;
    submit_info.wait_fence.fences       = _wait_fence_to_gpu.GetAddressOf();
    bmr = queue->Submit(submit);
    BMR_ASSERT_IF_FAILED(bmr);

    resetted = false;
    return fence_to_gpu;
}

buma3d::BMRESULT DeferredContext::Signal(const buma3d::util::Ptr<buma3d::IFence>& _fence_to_gpu)
{
    buma3d::SUBMIT_SIGNAL_DESC signal{};
    signal.signal_fence.num_fences      = 1;
    signal.signal_fence.fences          = _fence_to_gpu.GetAddressOf();
    signal.signal_fence.fence_values    = &dummy;
    return queue->SubmitSignal(signal);
}

buma3d::BMRESULT DeferredContext::Wait(const buma3d::util::Ptr<buma3d::IFence>& _fence_to_gpu)
{
    buma3d::SUBMIT_WAIT_DESC wait{};
    wait.wait_fence.num_fences      = 1;
    wait.wait_fence.fences          = _fence_to_gpu.GetAddressOf();
    wait.wait_fence.fence_values    = &dummy;
    return queue->SubmitWait(wait);
}

buma3d::BMRESULT DeferredContext::WaitOnCpu()
{
    auto bmr = fence_to_cpu->Wait(0, UINT32_MAX);
    BMR_ASSERT_IF_FAILED(bmr);
    return bmr;
}

void DeferredContext::MakeVisible()
{
    readback_buffer->MakeVisible();
}

void DeferredContext::PipelineBarrier(const buma3d::CMD_PIPELINE_BARRIER& _barrier)
{
    list->PipelineBarrier(_barrier);
}

void DeferredContext::CopyDataToBuffer(buma3d::IBuffer* _dst_buffer, uint64_t _dst_offset, size_t _src_size, const void* _src_data)
{
    auto al = upload_buffer->AllocateBufferPart(_src_size, 16);
    memcpy(al.map_data_part, _src_data, _src_size);

    buma3d::BUFFER_COPY_REGION copy_region{};
    copy_region.src_offset      = al.data_offset;
    copy_region.dst_offset      = _dst_offset;
    copy_region.size_in_bytes   = _src_size;

    buma3d::CMD_COPY_BUFFER_REGION copy{};
    copy.dst_buffer  = _dst_buffer;
    copy.num_regions = 1;
    copy.regions     = &copy_region;
    copy.src_buffer  = al.parent_resouce;

    list->CopyBufferRegion(copy);
}

void DeferredContext::CopyDataToTexture(buma3d::ITexture* _dst_texture, uint32_t _mip_slice, uint32_t _array_slice, uint64_t _src_row_pitch, uint64_t _src_texture_height, size_t _src_size, const void* _src_data)
{
    auto al = upload_buffer->AllocateBufferPart(_src_size, dr->GetDeviceAdapterLimits().buffer_copy_offset_alignment);
    memcpy(al.map_data_part, _src_data, _src_size);

    buma3d::BUFFER_TEXTURE_COPY_REGION copy_region{};
    copy_region.buffer_layout.offset            = al.data_offset;
    copy_region.buffer_layout.row_pitch         = _src_row_pitch;
    copy_region.buffer_layout.texture_height    = (uint32_t)_src_texture_height;

    copy_region.texture_subresource.offset.aspect      = buma3d::TEXTURE_ASPECT_FLAG_COLOR;
    copy_region.texture_subresource.offset.mip_slice   = _mip_slice;
    copy_region.texture_subresource.offset.array_slice = _array_slice;
    copy_region.texture_subresource.array_count        = 1;

    copy_region.texture_offset = nullptr;
    copy_region.texture_extent = nullptr;

    buma3d::CMD_COPY_BUFFER_TO_TEXTURE copy{};
    copy.src_buffer  = al.parent_resouce;
    copy.dst_texture = _dst_texture;
    copy.num_regions = 1;
    copy.regions     = &copy_region;

    list->CopyBufferToTexture(copy);
}

void DeferredContext::CopyBufferToData(BUFFER_ALLOCATION_PART* _dst_result_allocation, uint64_t _src_offset, uint64_t _src_size, buma3d::IBuffer* _src_buffer)
{
    *_dst_result_allocation = readback_buffer->AllocateBufferPart(_src_size, dr->GetDeviceAdapterLimits().buffer_copy_offset_alignment);

    buma3d::BUFFER_COPY_REGION copy_region{};
    copy_region.src_offset      = _src_offset;
    copy_region.dst_offset      = _dst_result_allocation->data_offset;
    copy_region.size_in_bytes   = _src_size;

    buma3d::CMD_COPY_BUFFER_REGION copy{};
    copy.dst_buffer  = _dst_result_allocation->parent_resouce;
    copy.num_regions = 1;
    copy.regions     = &copy_region;
    copy.src_buffer  = _src_buffer;

    list->CopyBufferRegion(copy);
}
void DeferredContext::CopyTextureToData(BUFFER_ALLOCATION_PART* _dst_result_allocation, buma3d::ITexture* _src_texture, uint32_t _mip_slice, uint32_t _array_slice)
{
    auto&& tex_desc = _src_texture->GetDesc().texture;
    uint32_t bw{}, bh{};
    util::GetFormatBlockSize(tex_desc.format_desc.format, &bw, &bh);
    auto format_size = util::GetFormatSize(tex_desc.format_desc.format);

    auto&& l = dr->GetDeviceAdapterLimits();
    auto extent    = util::CalcMipExtents(_mip_slice, tex_desc.extent);
    auto row_pitch = util::AlignUp(format_size * (extent.x / bw), l.buffer_copy_row_pitch_alignment);

    auto buffer_size = util::AlignUp(row_pitch * extent.y, l.buffer_copy_row_pitch_alignment);
    *_dst_result_allocation = upload_buffer->AllocateBufferPart(buffer_size, l.buffer_copy_offset_alignment);

    buma3d::BUFFER_TEXTURE_COPY_REGION copy_region{};
    copy_region.buffer_layout.offset            = _dst_result_allocation->data_offset;
    copy_region.buffer_layout.row_pitch         = row_pitch;
    copy_region.buffer_layout.texture_height    = extent.y;

    copy_region.texture_subresource.offset.aspect      = buma3d::TEXTURE_ASPECT_FLAG_COLOR;
    copy_region.texture_subresource.offset.mip_slice   = _mip_slice;
    copy_region.texture_subresource.offset.array_slice = _array_slice;
    copy_region.texture_subresource.array_count        = 1;

    copy_region.texture_offset = nullptr;
    copy_region.texture_extent = nullptr;

    buma3d::CMD_COPY_TEXTURE_TO_BUFFER copy{};
    copy.src_texture = _src_texture;
    copy.dst_buffer  = _dst_result_allocation->parent_resouce;
    copy.num_regions = 1;
    copy.regions     = &copy_region;

    list->CopyTextureToBuffer(copy);
}

void DeferredContext::CopyBufferRegion(const buma3d::CMD_COPY_BUFFER_REGION& _args)
{
    list->CopyBufferRegion(_args);
}
void DeferredContext::CopyTextureRegion(const buma3d::CMD_COPY_TEXTURE_REGION& _args)
{
    list->CopyTextureRegion(_args);
}

void DeferredContext::CopyBufferToTexture(const buma3d::CMD_COPY_BUFFER_TO_TEXTURE& _args)
{
    list->CopyBufferToTexture(_args);
}
void DeferredContext::CopyTextureToBuffer(const buma3d::CMD_COPY_TEXTURE_TO_BUFFER& _args)
{
    list->CopyTextureToBuffer(_args);
}


}// namespace buma
