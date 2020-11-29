#pragma once

namespace buma
{

class DeferredContext
{
public:
    DeferredContext();
    ~DeferredContext();

    bool Init(std::shared_ptr<DeviceResources> _dr, buma3d::util::Ptr<buma3d::ICommandQueue> _que);

    void Begin();

    void PipelineBarrier(const buma3d::CMD_PIPELINE_BARRIER& _barrier);

    void CopyDataToBuffer(buma3d::IBuffer* _dst_buffer, uint64_t _dst_offset, size_t _src_size, const void* _src_data);
    void CopyDataToTexture(buma3d::ITexture* _dst_texture, uint32_t _mip_slice, uint32_t _array_slice, uint64_t _src_row_pitch, uint64_t _src_texture_height, size_t _src_size, const void* _src_data);

    void CopyBufferToData(BUFFER_ALLOCATION_PART* _dst_result_allocation, uint64_t _src_offset, uint64_t _src_size, buma3d::IBuffer* _src_buffer);              // End()呼び出し後にBUFFER_ALLOCATION_PART::map_data_partから取得します。
    void CopyTextureToData(BUFFER_ALLOCATION_PART* _dst_result_allocation, buma3d::ITexture* _src_texture, uint32_t _mip_slice = 0, uint32_t _array_slice = 0); // End()呼び出し後にBUFFER_ALLOCATION_PART::map_data_partから取得します。

    void CopyBufferRegion(const buma3d::CMD_COPY_BUFFER_REGION& _args);
    void CopyTextureRegion(const buma3d::CMD_COPY_TEXTURE_REGION& _args);

    void CopyBufferToTexture(const buma3d::CMD_COPY_BUFFER_TO_TEXTURE& _args);
    void CopyTextureToBuffer(const buma3d::CMD_COPY_TEXTURE_TO_BUFFER& _args);

    // この関数が呼び出され処理が帰ったタイミングで、結果が反映されます。
    void End();

private:
    std::shared_ptr<DeviceResources>                dr;
    buma3d::util::Ptr<buma3d::IFence>               fence_to_cpu;
    buma3d::util::Ptr<buma3d::ICommandQueue>        queue;
    buma3d::util::Ptr<buma3d::ICommandAllocator>    allocator;
    buma3d::util::Ptr<buma3d::ICommandList>         list;

    std::unique_ptr<StagingBufferPool>              upload_buffer;
    std::unique_ptr<StagingBufferPool>              readback_buffer;

    buma3d::SUBMIT_INFO                             submit_info;
    buma3d::SUBMIT_DESC                             submit;

};


}// namespace buma
