#pragma once

namespace buma
{

class DeferredContext;

class ImmediateContext
{
public:
    ImmediateContext();
    ~ImmediateContext();

    bool Init(std::shared_ptr<DeviceResources> _dr, buma3d::util::Ptr<buma3d::ICommandQueue> _que);

    void PipelineBarrier(const buma3d::CMD_PIPELINE_BARRIER& _barrier);

    void CopyDataToBuffer(buma3d::IBuffer* _dst_buffer, uint64_t _dst_offset, size_t _src_size, const void* _src_data);
    void CopyDataToTexture(buma3d::ITexture* _dst_texture, uint32_t _mip_slice, uint32_t _array_slice, uint64_t _src_row_pitch, uint64_t _src_texture_height, size_t _src_size, const void* _src_data);

    template<typename T>
    void CopyBufferToData(std::vector<T>* _dst_data, uint64_t _src_offset, uint64_t _src_size, buma3d::IBuffer* _src_buffer)
    {
        BUFFER_ALLOCATION_PART al{};
        CopyBufferToData(&al, _src_offset, _src_size, _src_buffer);

        _dst_data->resize(al.size_in_bytes / sizeof(T));
        memcpy(_dst_data->data(), al.map_data_part, _dst_data->size() * sizeof(T));
    }

    template<typename T>
    void CopyTextureToData(std::vector<T>* _dst_data, buma3d::ITexture* _src_texture, uint32_t _mip_slice = 0, uint32_t _array_slice = 0)
    {
        BUFFER_ALLOCATION_PART al{};
        CopyTextureToData(&al, _src_texture, _mip_slice, _array_slice);

        _dst_data->resize(al.size_in_bytes / sizeof(T));
        memcpy(_dst_data->data(), al.map_data_part, _dst_data->size() * sizeof(T));
    }

    void CopyBufferRegion(const buma3d::CMD_COPY_BUFFER_REGION& _args);
    void CopyTextureRegion(const buma3d::CMD_COPY_TEXTURE_REGION& _args);

    void CopyBufferToTexture(const buma3d::CMD_COPY_BUFFER_TO_TEXTURE& _args);
    void CopyTextureToBuffer(const buma3d::CMD_COPY_TEXTURE_TO_BUFFER& _args);

private:
    void CopyBufferToData(BUFFER_ALLOCATION_PART* _dst_result_allocation, uint64_t _src_offset, uint64_t _src_size, buma3d::IBuffer* _src_buffer);
    void CopyTextureToData(BUFFER_ALLOCATION_PART* _dst_result_allocation, buma3d::ITexture* _src_texture, uint32_t _mip_slice = 0, uint32_t _array_slice = 0);

private:
    class CommandExecutionScope;
    std::unique_ptr<DeferredContext> ctx;

};


}// namespace buma
