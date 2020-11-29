#include "pch.h"
#include "ImmediateContext.h"

namespace buma
{

class ImmediateContext::CommandExecutionScope
{
public:
    CommandExecutionScope(ImmediateContext* _ictx) : ctx{ _ictx->ctx.get() } { ctx->Begin(); }
    ~CommandExecutionScope() { ctx->End(); }
private:
    DeferredContext* ctx;
};

ImmediateContext::ImmediateContext()
    : ctx{}
{
    ctx = std::make_unique<DeferredContext>();
}

ImmediateContext::~ImmediateContext()
{
    ctx.reset();
}

bool ImmediateContext::Init(std::shared_ptr<DeviceResources> _dr, buma3d::util::Ptr<buma3d::ICommandQueue> _que)
{
    return ctx->Init(_dr, _que);
}

void ImmediateContext::PipelineBarrier(const buma3d::CMD_PIPELINE_BARRIER& _barrier)
{
    CommandExecutionScope e(this);
    ctx->PipelineBarrier(_barrier);
}

void ImmediateContext::CopyDataToBuffer(buma3d::IBuffer* _dst_buffer, uint64_t _dst_offset, size_t _src_size, const void* _src_data)
{
    CommandExecutionScope e(this);
    ctx->CopyDataToBuffer(_dst_buffer, _dst_offset, _src_size, _src_data);
}
void ImmediateContext::CopyDataToTexture(buma3d::ITexture* _dst_texture, uint32_t _mip_slice, uint32_t _array_slice, uint64_t _src_row_pitch, uint64_t _src_texture_height, size_t _src_size, const void* _src_data)
{
    CommandExecutionScope e(this);
    ctx->CopyDataToTexture(_dst_texture, _mip_slice, _array_slice, _src_row_pitch, _src_texture_height, _src_size, _src_data);
}

void ImmediateContext::CopyBufferToData(BUFFER_ALLOCATION_PART* _dst_result_allocation, uint64_t _src_offset, uint64_t _src_size, buma3d::IBuffer* _src_buffer)
{
    CommandExecutionScope e(this);
    ctx->CopyBufferToData(_dst_result_allocation, _src_offset, _src_size, _src_buffer);
}
void ImmediateContext::CopyTextureToData(BUFFER_ALLOCATION_PART* _dst_result_allocation, buma3d::ITexture* _src_texture, uint32_t _mip_slice, uint32_t _array_slice)
{
    CommandExecutionScope e(this);
    ctx->CopyTextureToData(_dst_result_allocation, _src_texture, _mip_slice, _array_slice);
}

void ImmediateContext::CopyBufferRegion(const buma3d::CMD_COPY_BUFFER_REGION& _args)
{
    CommandExecutionScope e(this);
    ctx->CopyBufferRegion(_args);

}
void ImmediateContext::CopyTextureRegion(const buma3d::CMD_COPY_TEXTURE_REGION& _args)
{
    CommandExecutionScope e(this);
    ctx->CopyTextureRegion(_args);
}

void ImmediateContext::CopyBufferToTexture(const buma3d::CMD_COPY_BUFFER_TO_TEXTURE& _args)
{
    CommandExecutionScope e(this);
    ctx->CopyBufferToTexture(_args);
}
void ImmediateContext::CopyTextureToBuffer(const buma3d::CMD_COPY_TEXTURE_TO_BUFFER& _args)
{
    CommandExecutionScope e(this);
    ctx->CopyTextureToBuffer(_args);
}


}// namespace buma
