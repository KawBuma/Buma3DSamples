#pragma once
#include <cassert>
#include <string>
#include <sstream>
#include <iomanip>
#include "Util/Buma3DPtr.h"

namespace buma
{
namespace util
{

template<typename T>
inline uint32_t SafeRelease(T*& _p)
{
    auto result = (uint32_t)(_p ? _p->Release() : 0);
    _p = nullptr;
    return result;
}
template<typename T>
inline T* SafeAddRef(T*& _p)
{
    if (!_p) return;
    _p->AddRef();
    return _p;
}

#pragma region containerhelper

template <typename T>
inline void SwapClear(T& _container)
{
    { T().swap(_container); }
}

template <typename T>
inline typename T::iterator EraseContainerElem(T& _container, const size_t _erase_pos)
{
    return _container.erase(_container.begin() + _erase_pos);
}

// _first_pos: 0~, _last_pos: _container.size()までの間で設定してください
template <typename T>
inline typename T::iterator EraseContainerRange(T& _container, const size_t _first_pos, const size_t _last_pos)
{
    typename T::const_iterator it = _container.begin();
    return _container.erase(it + _first_pos, it + _last_pos);
}

template <typename T>
inline typename T::iterator InsertContainerElem(T& _container, const size_t _insert_pos, const typename T::value_type& _value)
{
    return _container.insert(_container.begin() + _insert_pos, _value);
}

template <typename T>
inline typename T::iterator InsertContainerElem(T& _container, const size_t _insert_pos, typename T::value_type&& _value)
{
    return _container.insert(_container.begin() + _insert_pos, _value);
}

template <typename T>
inline typename T::iterator InsertContainerElemCount(T& _container, const size_t _insert_pos, const size_t _insert_count, const typename T::value_type& _value)
{
    return _container.insert(_container.begin() + _insert_pos, _insert_count, _value);
}

template <typename T>
inline typename T::iterator InsertContainerElemCount(T& _container, const size_t _insert_pos, const size_t _insert_count, typename T::value_type&& _value)
{
    return _container.insert(_container.begin() + _insert_pos, _insert_count, _value);
}

// _insert_first: 0 ~ _insert_container.size()までの間で設定してください
// _insert_last: 0 ~ _insert_container.size()までの間で設定してください
// _insert_firstと _insert_lastが同じの場合要素は挿入されません
template <typename T>
inline typename T::iterator InsertContainerElemRange(T& _container, const size_t _insert_pos, T& _insert_container, const size_t _insert_first, const size_t _insert_last)
{
    typename T::iterator ins_it = _insert_container.begin();
    return _container.insert(_container.begin() + _insert_pos, ins_it + _insert_first, ins_it + _insert_last);
}

#pragma endregion containerhelper

inline std::string GetUUIDString(const uint8_t _uuid[16])
{
#define B3DFW std::setfill('0') << std::setw(2) 
    std::stringstream ss;
    ss << std::hex 
        << B3DFW << (uint32_t)_uuid[0]  << B3DFW << (uint32_t)_uuid[1] << B3DFW << (uint32_t)_uuid[2] << B3DFW << (uint32_t)_uuid[3] << "-"
        << B3DFW << (uint32_t)_uuid[4]  << B3DFW << (uint32_t)_uuid[5] << '-'
        << B3DFW << (uint32_t)_uuid[6]  << B3DFW << (uint32_t)_uuid[7] << '-'
        << B3DFW << (uint32_t)_uuid[8]  << B3DFW << (uint32_t)_uuid[9] << '-'
        << B3DFW << (uint32_t)_uuid[10] << B3DFW << (uint32_t)_uuid[11] << B3DFW << (uint32_t)_uuid[12] << B3DFW << (uint32_t)_uuid[13] << B3DFW << (uint32_t)_uuid[14] << B3DFW << (uint32_t)_uuid[15]
        << std::dec;
    return ss.str();
#undef B3DFW
}

inline std::string GetLUIDString(const uint8_t _luid[8])
{
#define B3DFW std::setfill('0') << std::setw(2) 
    std::stringstream ss;
    ss << std::hex
        << "Low: "    << B3DFW << (uint32_t)_luid[0] << B3DFW << (uint32_t)_luid[1] << B3DFW << (uint32_t)_luid[2] << B3DFW << (uint32_t)_luid[3]
        << ", High: " << B3DFW << (uint32_t)_luid[4] << B3DFW << (uint32_t)_luid[5] << B3DFW << (uint32_t)_luid[6] << B3DFW << (uint32_t)_luid[7]
        << std::dec;
    return ss.str();
#undef B3DFW
}

enum CODEPAGE : uint32_t
{
      CODEPAGE_ACP        = 0        // default to ANSI code page
    , CODEPAGE_OEMCP      = 1        // default to OEM  code page
    , CODEPAGE_MACCP      = 2        // default to MAC  code page
    , CODEPAGE_THREAD_ACP = 3        // current thread's ANSI code page
    , CODEPAGE_SYMBOL     = 42       // SYMBOL translations

    , CODEPAGE_UTF7       = 65000    // UTF-7 translation
    , CODEPAGE_UTF8       = 65001    // UTF-8 translation
};

std::string ConvertWideToCp(CODEPAGE _code_page /*= CP_UTF8*/, int _len_with_null_term, const wchar_t* _wstr);
std::wstring ConvertCpToWide(CODEPAGE _code_page /*= CP_UTF8*/, int _len_with_null_term, const char* _str);

inline std::string  ConvertWideToCp  (CODEPAGE _code_page, const std::wstring& _wstr)    { return ConvertWideToCp(_code_page, int(_wstr.size() + 1ull), _wstr.c_str()); }
inline std::wstring ConvertCpToWide  (CODEPAGE _code_page, const std::string& _str)      { return ConvertCpToWide(_code_page, int(_str.size() + 1ull), _str.c_str()); }

inline std::wstring ConvertUtf8ToWide(const char*          _str)                          { return ConvertCpToWide(CODEPAGE_UTF8, _str); }
inline std::wstring ConvertUtf8ToWide(const std::string&  _str)                           { return ConvertCpToWide(CODEPAGE_UTF8, int(_str.size() + 1ull), _str.c_str()); }
inline std::string  ConvertWideToUtf8(const wchar_t*       _wstr)                         { return ConvertWideToCp(CODEPAGE_UTF8, _wstr); }
inline std::string  ConvertWideToUtf8(const std::wstring& _wstr)                          { return ConvertWideToCp(CODEPAGE_UTF8, int(_wstr.size() + 1ull), _wstr.c_str()); }

inline std::wstring ConvertAnsiToWide(const char*          _str)                          { return ConvertCpToWide(CODEPAGE_ACP, _str); }
inline std::wstring ConvertAnsiToWide(const std::string&  _str)                           { return ConvertCpToWide(CODEPAGE_ACP, int(_str.size() + 1ull), _str.c_str()); }
inline std::string  ConvertWideToAnsi(const wchar_t*       _wstr)                         { return ConvertWideToCp(CODEPAGE_ACP, _wstr); }
inline std::string  ConvertWideToAnsi(const std::wstring& _wstr)                          { return ConvertWideToCp(CODEPAGE_ACP, int(_wstr.size() + 1ull), _wstr.c_str()); }

inline buma3d::UINT3 CalcMipExtents(uint32_t _mip_slice, const buma3d::EXTENT3D& _extent_mip0)
{
    return buma3d::UINT3{  (std::max)(_extent_mip0.width  >> _mip_slice, 1ui32)
                         , (std::max)(_extent_mip0.height >> _mip_slice, 1ui32)
                         , (std::max)(_extent_mip0.depth  >> _mip_slice, 1ui32) };
}

class Mapper
{
public:
    template<typename T>
    class MappedData
    {
    public:
        MappedData(T* _data) :data{ _data } {}
        ~MappedData() { data = nullptr; }

        T* GetData()
        {
            return data;
        }

        void Write(uint32_t _index, const T& _src)
        {
            if (data)
                data[_index] = _src;
        }
        bool WriteRaw(size_t _offset, const void* _src, size_t _size)
        {
            if (data)
                memcpy((uint8_t*)data + _offset, _src, _size);
        }

    private:
        T* data;

    };
    
public:
    Mapper(buma3d::util::Ptr<buma3d::IResourceHeap> _heap)
        : heap  { _heap }
        , range {}
        , data  {}
    {
        heap->Map();
        heap->GetMappedData(&range, &data);
    }

    Mapper(buma3d::util::Ptr<buma3d::IResourceHeap> _heap, const buma3d::MAPPED_RANGE& _range)
        : heap  { _heap }
        , range { _range }
        , data  {}
    {
        heap->Map(&range);
        heap->GetMappedData(&range, &data);
    }

    ~Mapper()
    {
        heap->Unmap();
        heap.Reset();
        range = {};
        data  = nullptr;
    }

    template<typename T>
    MappedData<T> As()
    {
        return MappedData<T>(reinterpret_cast<T*>(data));
    }

private:
    buma3d::util::Ptr<buma3d::IResourceHeap>    heap;
    buma3d::MAPPED_RANGE                        range;
    void*                                       data;

};

struct FENCE_VALUES
{
    FENCE_VALUES() : value{} {}
    FENCE_VALUES(uint64_t _val) : value{ _val } {}
    uint64_t value;
    FENCE_VALUES& operator++()    { ++value; return *this; } 
    FENCE_VALUES  operator++(int) { return FENCE_VALUES(value++); }
    inline uint64_t wait  () const { return value; }
    inline uint64_t signal() const { return value + 1; }
};

class FenceSubmitDesc
{
public:
    FenceSubmitDesc()
        : signal_desc  {}
        , wait_desc    {}
        , num_fences   {}
        , fences       {}
        , fence_values {}
    {

    }

    ~FenceSubmitDesc()
    {

    }

    void Reset()
    {
        num_fences = 0;

        signal_desc.signal_fence.num_fences     = 0;
        signal_desc.signal_fence.fences         = 0;
        signal_desc.signal_fence.fence_values   = 0;
        signal_desc.signal_fence_to_cpu = nullptr;

        wait_desc.wait_fence.num_fences         = 0;
        wait_desc.wait_fence.fences             = 0;
        wait_desc.wait_fence.fence_values       = 0;
    }

    void AddFence(buma3d::IFence* _fence, uint64_t _fence_value)
    {
        Resize(num_fences + 1);
        fences      .data()[num_fences] = _fence;
        fence_values.data()[num_fences] = _fence_value;
        num_fences++;
    }

    void SetSignalFenceToGpu(buma3d::IFence* _fence)
    {
        signal_desc.signal_fence_to_cpu = _fence;
    }

    const buma3d::SUBMIT_SIGNAL_DESC& GetAsSignal()
    {
        signal_desc.signal_fence.num_fences   = num_fences;
        signal_desc.signal_fence.fences       = fences.data();
        signal_desc.signal_fence.fence_values = fence_values.data();
        return signal_desc;
    }

    const buma3d::SUBMIT_WAIT_DESC& GetAsWait()
    {
        wait_desc.wait_fence.num_fences   = num_fences;
        wait_desc.wait_fence.fences       = fences.data();
        wait_desc.wait_fence.fence_values = fence_values.data();
        return wait_desc;
    }

    const buma3d::FENCE_SUBMISSION& GetAsFenceSubmission()
    {
        return GetAsWait().wait_fence;
    }

private:
    void Resize(uint32_t _num_fences)
    {
        if (_num_fences > fences.size())
        {
            fences.resize(_num_fences);
            fence_values.resize(_num_fences);

            wait_desc.wait_fence.fences             = fences.data();
            wait_desc.wait_fence.fence_values       = fence_values.data();

            signal_desc.signal_fence.fences         = fences.data();
            signal_desc.signal_fence.fence_values   = fence_values.data();
        }
    }

private:
    buma3d::SUBMIT_SIGNAL_DESC      signal_desc;
    buma3d::SUBMIT_WAIT_DESC        wait_desc;
    uint32_t                        num_fences;
    std::vector<buma3d::IFence*>    fences;
    std::vector<uint64_t>           fence_values;

};


class SubmitInfo
{
public:
    SubmitInfo()
        : submit_info               {}
        , wait_fence                {}
        , signal_fence              {}
        , command_lists_to_execute  {}
    {
    }

    ~SubmitInfo()
    {
    }

    void Reset()
    {
        wait_fence.Reset();
        signal_fence.Reset();
        submit_info.num_command_lists_to_execute = 0;
    }
    SubmitInfo& AddWaitFence(buma3d::IFence* _fence, uint64_t _fence_value = 0)
    {
        wait_fence.AddFence(_fence, _fence_value);
        return *this;
    }
    SubmitInfo& AddSignalFence(buma3d::IFence* _fence, uint64_t _fence_value = 0)
    {
        signal_fence.AddFence(_fence, _fence_value);
        return *this;
    }
    SubmitInfo& AddCommandList(buma3d::ICommandList* _command_list_to_execute)
    {
        Resize(submit_info.num_command_lists_to_execute + 1);
        command_lists_to_execute.data()[submit_info.num_command_lists_to_execute++] = _command_list_to_execute;
        return *this;
    }

    const buma3d::SUBMIT_INFO& Get()
    {
        submit_info.wait_fence   = wait_fence.GetAsFenceSubmission();
        submit_info.signal_fence = signal_fence.GetAsFenceSubmission();
        return submit_info;
    }

private:
    void Resize(uint32_t _num_command_lists_to_execute)
    {
        if (_num_command_lists_to_execute > (uint32_t)command_lists_to_execute.size())
        {
            command_lists_to_execute.resize(_num_command_lists_to_execute);
            submit_info.command_lists_to_execute = command_lists_to_execute.data();
        }
    }

private:
    buma3d::SUBMIT_INFO                 submit_info;
    FenceSubmitDesc                     wait_fence;
    FenceSubmitDesc                     signal_fence;
    std::vector<buma3d::ICommandList*>  command_lists_to_execute;

};

class SubmitDesc
{
public:
    SubmitDesc()
        : desc      {}
        , infos     {}
        , b3d_infos {}
    {
    }

    ~SubmitDesc()
    {
    }

    void Reset()
    {
        for (auto& i : infos)
            i->Reset();
        desc.num_submit_infos = 0;
        desc.signal_fence_to_cpu = nullptr;
    }
    void SetSignalFenceToCpu(buma3d::IFence* _signal_fence_to_cpu)
    {
        desc.signal_fence_to_cpu = _signal_fence_to_cpu;
    }
    SubmitInfo& AddNewSubmitInfo()
    {
        Resize(desc.num_submit_infos + 1);
        return (*infos.data()[desc.num_submit_infos++].get());
    }

    const buma3d::SUBMIT_DESC& Get()
    {
        auto id = infos.data();
        auto bid = b3d_infos.data();
        for (uint32_t i = 0; i < desc.num_submit_infos; i++)
            bid[i] = id[i]->Get();

        return desc;
    }

private:
    void Resize(uint32_t _num_infos)
    {
        if (_num_infos > (uint32_t)infos.size())
        {
            infos.resize(_num_infos);
            b3d_infos.resize(_num_infos);
            desc.submit_infos = b3d_infos.data();

            for (auto& i : infos)
            {
                if (!i)
                    i = std::make_shared<SubmitInfo>();
            }
        }
    }

private:
    buma3d::SUBMIT_DESC                         desc;
    std::vector<std::shared_ptr<SubmitInfo>>    infos;
    std::vector<buma3d::SUBMIT_INFO>            b3d_infos;

};

#pragma region pipeline barrier

class TextureBarrierRange
{
public:
    TextureBarrierRange()
        : barrier_range{}
        , subres_ranges{}
    {
    }

    ~TextureBarrierRange()
    {
    }

    void AddSubresRange()
    {

    }

    void Reset()
    {
        barrier_range.texture                   = nullptr;
        barrier_range.num_subresource_ranges    = 0;
    }
    void AddSubresRange(buma3d::TEXTURE_ASPECT_FLAGS _aspect, uint32_t _mip_slice, uint32_t _array_slice, uint32_t _array_size = 1, uint32_t _mip_levels = 1)
    {
        Resize(barrier_range.num_subresource_ranges + 1);
        auto&& range = subres_ranges.data()[barrier_range.num_subresource_ranges++];
        range.array_size    = _array_size;
        range.mip_levels    = _mip_levels;
        range.offset.aspect         = _aspect;
        range.offset.mip_slice      = _mip_slice;
        range.offset.array_slice    = _array_slice;
    }

    const buma3d::TEXTURE_BARRIER_RANGE* Get(buma3d::ITexture* _texture)
    {
        barrier_range.texture = _texture;
        return &barrier_range;
    }

private:
    void Resize(uint32_t _num_subres_ranges)
    {
        if (_num_subres_ranges > barrier_range.num_subresource_ranges)
        {
            subres_ranges.resize(_num_subres_ranges);
            barrier_range.subresource_ranges = subres_ranges.data();
        }
    }

private:
    buma3d::TEXTURE_BARRIER_RANGE           barrier_range;
    std::vector<buma3d::SUBRESOURCE_RANGE>  subres_ranges;

};

class PipelineBarrierDesc
{
public:
    PipelineBarrierDesc()
        : barrier           {}
        , buffer_barreirs   {}
        , texture_barreirs  {}
    {}
    ~PipelineBarrierDesc() {}

    void Reset()
    {
        barrier.num_buffer_barriers  = 0;
        barrier.num_texture_barriers = 0;
        barrier.dependency_flags     = buma3d::DEPENDENCY_FLAG_NONE;
    }
    void AddBufferBarrier(  buma3d::IBuffer*                _buffer
                          , buma3d::RESOURCE_STATE          _src_state
                          , buma3d::RESOURCE_STATE          _dst_state
                          , buma3d::RESOURCE_BARRIER_FLAG   _barrier_flags  = buma3d::RESOURCE_BARRIER_FLAG_NONE
                          , buma3d::COMMAND_TYPE            _src_queue_type = buma3d::COMMAND_TYPE_DIRECT
                          , buma3d::COMMAND_TYPE            _dst_queue_type = buma3d::COMMAND_TYPE_DIRECT)
    {
        Resize(barrier.num_buffer_barriers + 1, barrier.buffer_barriers, &buffer_barreirs);
        buffer_barreirs.data()[barrier.num_buffer_barriers++] = { _buffer , _src_state , _dst_state , _src_queue_type , _dst_queue_type , _barrier_flags };
    }
    void AddBufferBarrier(const buma3d::BUFFER_BARRIER_DESC& _buffer_barrier)
    {
        Resize(barrier.num_buffer_barriers + 1, barrier.buffer_barriers, &buffer_barreirs);
        buffer_barreirs.data()[barrier.num_buffer_barriers++] = _buffer_barrier;
    }

    void AddTextureBarrier(const buma3d::TEXTURE_BARRIER_RANGE* _barrier_range
                           , buma3d::RESOURCE_STATE             _src_state
                           , buma3d::RESOURCE_STATE             _dst_state
                           , buma3d::RESOURCE_BARRIER_FLAG      _barrier_flags  = buma3d::RESOURCE_BARRIER_FLAG_NONE
                           , buma3d::COMMAND_TYPE               _src_queue_type = buma3d::COMMAND_TYPE_DIRECT
                           , buma3d::COMMAND_TYPE               _dst_queue_type = buma3d::COMMAND_TYPE_DIRECT)
    {
        Resize(barrier.num_texture_barriers + 1, barrier.texture_barriers, &texture_barreirs);
        texture_barreirs.data()[barrier.num_texture_barriers++] = { buma3d::TEXTURE_BARRIER_TYPE_BARRIER_RANGE , _barrier_range , _src_state , _dst_state , _src_queue_type , _dst_queue_type , _barrier_flags };
    }
    void AddTextureBarrier(  buma3d::IView*                     _view
                           , buma3d::RESOURCE_STATE             _src_state
                           , buma3d::RESOURCE_STATE             _dst_state
                           , buma3d::RESOURCE_BARRIER_FLAG      _barrier_flags = buma3d::RESOURCE_BARRIER_FLAG_NONE
                           , buma3d::COMMAND_TYPE               _src_queue_type = buma3d::COMMAND_TYPE_DIRECT
                           , buma3d::COMMAND_TYPE               _dst_queue_type = buma3d::COMMAND_TYPE_DIRECT)
    {
        Resize(barrier.num_texture_barriers + 1, barrier.texture_barriers, &texture_barreirs);
        (texture_barreirs.data()[barrier.num_texture_barriers++] = { buma3d::TEXTURE_BARRIER_TYPE_VIEW , nullptr, _src_state , _dst_state , _src_queue_type , _dst_queue_type , _barrier_flags })
            .view = _view;
    }

    const buma3d::CMD_PIPELINE_BARRIER& Get(buma3d::PIPELINE_STAGE_FLAGS _src_stages, buma3d::PIPELINE_STAGE_FLAGS _dst_stages, buma3d::DEPENDENCY_FLAGS _dependency_flags = buma3d::DEPENDENCY_FLAG_NONE)
    {
        barrier.src_stages       = _src_stages;
        barrier.dst_stages       = _dst_stages;
        barrier.dependency_flags = _dependency_flags;
        return barrier;
    }

private:
    template<typename T>
    void Resize(uint32_t _num, const T*& _ptr, std::vector<T>* _dst)
    {
        if (_num > _dst->size())
        {
            _dst->resize(_num);
            _ptr = _dst->data();
        }
    }

private:
    buma3d::CMD_PIPELINE_BARRIER                    barrier;
    std::vector<buma3d::BUFFER_BARRIER_DESC>        buffer_barreirs;
    std::vector<buma3d::TEXTURE_BARRIER_DESC>       texture_barreirs;

};

#pragma endregion pipeline barrier

#pragma region root signature

class RootParameter
{
public:
    RootParameter()
        : parameter {}
        , ranges    {}
    {
        Reset();
    }
    ~RootParameter()
    {
        Reset();
    }

    void Reset()
    {
        parameter = {};
        parameter.shader_visibility = buma3d::SHADER_VISIBILITY_ALL_GRAPHICS_COMPUTE;
        ranges.reset();
    }

    void SetShaderVisibility(buma3d::SHADER_VISIBILITY _visibility)
    {
        parameter.shader_visibility = _visibility;
    }

    void InitAsPush32BitConstants(  uint32_t _num32_bit_values
                                  , uint32_t _shader_register
                                  , uint32_t _register_space = 0)
    {
        parameter.type = buma3d::ROOT_PARAMETER_TYPE_PUSH_32BIT_CONSTANTS;
        parameter.inline_constants.num32_bit_values = _num32_bit_values;
        parameter.inline_constants.shader_register  = _shader_register;
        parameter.inline_constants.register_space   = _register_space;
    }

    void InitAsDynamicDescriptor(  buma3d::DESCRIPTOR_TYPE   _type
                                 , uint32_t                  _shader_register
                                 , uint32_t                  _register_space     = 0 
                                 , buma3d::DESCRIPTOR_FLAGS  _flags              = buma3d::DEPENDENCY_FLAG_NONE)
    {
        parameter.type = buma3d::ROOT_PARAMETER_TYPE_DYNAMIC_DESCRIPTOR;
        parameter.dynamic_descriptor.type            = _type;
        parameter.dynamic_descriptor.shader_register = _shader_register;
        parameter.dynamic_descriptor.register_space  = _register_space;
        parameter.dynamic_descriptor.flags           = _flags;
    }

    void InitAsDescriptorTable()
    {
        assert(!ranges);
        parameter.type = buma3d::ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        ranges = std::make_shared<std::vector<buma3d::DESCRIPTOR_RANGE>>();
    }

    void AddRange(  buma3d::DESCRIPTOR_TYPE   _type
                  , uint32_t                  _num_descriptors
                  , uint32_t                  _base_shader_register
                  , uint32_t                  _register_space       = 0
                  , buma3d::DESCRIPTOR_FLAGS  _flags                = buma3d::DEPENDENCY_FLAG_NONE)
    {
        assert(ranges);
        ranges->emplace_back(buma3d::DESCRIPTOR_RANGE{ _type, _num_descriptors, _base_shader_register, _register_space, _flags });
        parameter.descriptor_table.num_descriptor_ranges = (uint32_t)ranges->size();
        parameter.descriptor_table.descriptor_ranges     = ranges->data();
    }

    const buma3d::ROOT_PARAMETER& Get()
    {
        return parameter;
    }

private:
    buma3d::ROOT_PARAMETER                                  parameter;
    std::shared_ptr<std::vector<buma3d::DESCRIPTOR_RANGE>>  ranges;

};

class RootSignatureDesc
{
public:
    RootSignatureDesc()
        : desc              {}
        , parameters        {}
        , static_samplers   {}
        , register_shifts   {}
    {}
    ~RootSignatureDesc()
    {
        desc            = {};
        parameters      = {};
        b3d_parameters  = {};
        static_samplers = {};
        register_shifts = {};
    }

    void Reset()
    {
        desc = {};
        parameters.clear();
        static_samplers.clear();
    }

    RootParameter& AddNewRootParameter() { return parameters.emplace_back(); }
    void AddRootParameter(RootParameter&&       _parameter) { parameters.emplace_back(std::move(_parameter)); }
    void AddRootParameter(const RootParameter&  _parameter) { parameters.emplace_back(_parameter); }

    void AddStaticSampler(buma3d::STATIC_SAMPLER&&      _static_sampler) { static_samplers.emplace_back(std::move(_static_sampler)); }
    void AddStaticSampler(const buma3d::STATIC_SAMPLER& _static_sampler) { static_samplers.emplace_back(_static_sampler); }
    void AddStaticSampler(uint32_t _shader_register, uint32_t _register_space, buma3d::ISamplerView* _sampler, buma3d::SHADER_VISIBILITY _shader_visibility = buma3d::SHADER_VISIBILITY_ALL_GRAPHICS_COMPUTE)
    { static_samplers.emplace_back(buma3d::STATIC_SAMPLER{ _shader_register, _register_space, _shader_visibility, _sampler }); }

    void SetRegisterShift(buma3d::SHADER_REGISTER_SHIFT&&      _shift) { register_shifts.emplace_back(std::move(_shift)); }
    void SetRegisterShift(const buma3d::SHADER_REGISTER_SHIFT& _shift) { register_shifts.emplace_back(_shift); }
    void SetRegisterShift(buma3d::SHADER_REGISTER_TYPE _type, uint32_t _register_shift, uint32_t _register_space)
    { register_shifts.emplace_back(buma3d::SHADER_REGISTER_SHIFT{ _type, _register_shift,_register_space }); }

    const buma3d::ROOT_SIGNATURE_DESC& Get(buma3d::ROOT_SIGNATURE_FLAGS _flags, buma3d::RAY_TRACING_SHADER_VISIBILITY_FLAGS _raytracing_shader_visibilities = buma3d::RAY_TRACING_SHADER_VISIBILITY_FLAG_NONE)
    {
        b3d_parameters.resize(parameters.size());
        size_t cnt = 0;
        for (auto& i : parameters)
            b3d_parameters.data()[cnt++] = i.Get();

        desc.flags                          = _flags;
        desc.raytracing_shader_visibilities = _raytracing_shader_visibilities;
        desc.num_parameters                 = (uint32_t)b3d_parameters.size();
        desc.parameters                     =           b3d_parameters.data();
        desc.num_static_samplers            = (uint32_t)static_samplers.size();
        desc.static_samplers                =           static_samplers.data();
        desc.num_register_shifts            = (uint32_t)register_shifts.size();
        desc.register_shifts                =           register_shifts.data();
        return desc;
    }

private:
    buma3d::ROOT_SIGNATURE_DESC                 desc;
    std::vector<RootParameter>                  parameters;
    std::vector<buma3d::ROOT_PARAMETER>         b3d_parameters;
    std::vector<buma3d::STATIC_SAMPLER>         static_samplers;
    std::vector<buma3d::SHADER_REGISTER_SHIFT>  register_shifts;

};

#pragma endregion root signature

#pragma region descriptor update

class WriteDescriptorRange
{
public:
    WriteDescriptorRange()
        : range     {}
        , src_views {}
    {
        src_views = std::make_shared<std::vector<buma3d::IView*>>();
    }

    ~WriteDescriptorRange()
    {
    }

    WriteDescriptorRange& SetDstRange(uint32_t _dst_range_index, uint32_t _dst_first_array_element, uint32_t _num_descriptors)
    {
        range.dst_range_index           = _dst_range_index;
        range.dst_first_array_element   = _dst_first_array_element;
        range.num_descriptors           = _num_descriptors;
        src_views->resize(_num_descriptors);
        return *this;
    }

    template<typename T>
    WriteDescriptorRange& SetSrcView(uint32_t _index, T* _view)
    {
        return SetSrcView(_index, static_cast<buma3d::IView*>(_view));
    }
    template<>
    WriteDescriptorRange& SetSrcView(uint32_t _index, buma3d::IView* _view)
    {
        src_views->at(_index) = _view;
        return *this;
    }
    WriteDescriptorRange& SetSrcViews(uint32_t _offset, const std::initializer_list<buma3d::IView*>& _views)
    {
        size_t idx = _offset;
        for (auto& i : _views)
            src_views->at(idx) = i;

        return *this;
    }

    const buma3d::WRITE_DESCRIPTOR_RANGE& Get()
    {
        range.num_descriptors   = (uint32_t)src_views->size();
        range.src_views         =           src_views->data();
        return range;
    }

private:
    buma3d::WRITE_DESCRIPTOR_RANGE                  range;
    std::shared_ptr<std::vector<buma3d::IView*>>    src_views;

};

class WriteDescriptorTable
{
public:
    WriteDescriptorTable()
        : tables        {}
        , ranges        {}
        , b3d_ranges    {}
    {
        ranges = std::make_shared<std::vector<WriteDescriptorRange>>();
        b3d_ranges = std::make_shared<std::vector<buma3d::WRITE_DESCRIPTOR_RANGE>>();
    }

    ~WriteDescriptorTable()
    {
    }

    WriteDescriptorRange& AddNewWriteDescriptorRange()
    {
        return ranges->emplace_back();
    }

    void Finalize(uint32_t _dst_root_parameter_index)
    {
        tables.dst_root_parameter_index = _dst_root_parameter_index;
        b3d_ranges->resize(ranges->size());
        size_t cnt = 0;
        for (auto& i : *ranges)
            b3d_ranges->data()[cnt++] = i.Get();
    }
    const buma3d::WRITE_DESCRIPTOR_TABLE& Get()
    {
        tables.num_ranges   = (uint32_t)b3d_ranges->size();
        tables.ranges       =           b3d_ranges->data();
        return tables;
    }

private:
    buma3d::WRITE_DESCRIPTOR_TABLE                                  tables;
    std::shared_ptr<std::vector<WriteDescriptorRange>>              ranges;
    std::shared_ptr<std::vector<buma3d::WRITE_DESCRIPTOR_RANGE>>    b3d_ranges;

};

class WriteDescriptorSet
{
public:
    WriteDescriptorSet()
        : write_set             {}
        , tables                {}
        , b3d_tables            {}
        , dynamic_descriptors   {}
    {
        tables              = std::make_shared<std::vector<WriteDescriptorTable>>();
        b3d_tables          = std::make_shared<std::vector<buma3d::WRITE_DESCRIPTOR_TABLE>>();
        dynamic_descriptors = std::make_shared<std::vector<buma3d::WRITE_DYNAMIC_DESCRIPTOR>>();
    }

    ~WriteDescriptorSet()
    {
    }

    WriteDescriptorTable& AddNewWriteDescriptorTable()
    {
        return tables->emplace_back();
    }
    WriteDescriptorSet& AddWriteDynamicDescriptor(  uint32_t          _dst_root_parameter_index
                                                  , buma3d::IView*    _src_view
                                                  , uint64_t          _src_view_buffer_offset)
    {
        dynamic_descriptors->emplace_back(buma3d::WRITE_DYNAMIC_DESCRIPTOR{ _dst_root_parameter_index, _src_view, _src_view_buffer_offset });
        return *this;
    }

    void Finalize(buma3d::IDescriptorSet* _dst_set)
    {
        write_set.dst_set = _dst_set;

        b3d_tables->resize(tables->size());
        size_t cnt = 0;
        for (auto& i : *tables)
            b3d_tables->data()[cnt++] = i.Get();
    }
    const buma3d::WRITE_DESCRIPTOR_SET& Get()
    {
        write_set.num_descriptor_tables   = (uint32_t)b3d_tables->size();
        write_set.descriptor_tables       =           b3d_tables->data();
        write_set.num_dynamic_descriptors = (uint32_t)dynamic_descriptors->size();
        write_set.dynamic_descriptors     =           dynamic_descriptors->data();
        return write_set;
    }

private:
    buma3d::WRITE_DESCRIPTOR_SET                                    write_set;
    std::shared_ptr<std::vector<WriteDescriptorTable>>              tables;
    std::shared_ptr<std::vector<buma3d::WRITE_DESCRIPTOR_TABLE>>    b3d_tables;
    std::shared_ptr<std::vector<buma3d::WRITE_DYNAMIC_DESCRIPTOR>>  dynamic_descriptors;

};

class CopyDescriptorTable
{
public:
    CopyDescriptorTable()
        : table             {}
        , src_ranges        {}
        , dst_ranges        {}
        , num_descriptors   {}
    {
        src_ranges      = std::make_shared<std::vector<buma3d::COPY_DESCRIPTOR_RANGE>>();
        dst_ranges      = std::make_shared<std::vector<buma3d::COPY_DESCRIPTOR_RANGE>>();
        num_descriptors = std::make_shared<std::vector<uint32_t>                     >();
    }

    ~CopyDescriptorTable()
    {
    }

    CopyDescriptorTable& SetRootParameterIndex(uint32_t _src_root_parameter_index, uint32_t _dst_root_parameter_index)
    {
        table.src_root_parameter_index = _src_root_parameter_index;
        table.dst_root_parameter_index = _dst_root_parameter_index;
        return *this;
    }
    CopyDescriptorTable& AddRange(  const buma3d::COPY_DESCRIPTOR_RANGE& _src_ranges
                                  , const buma3d::COPY_DESCRIPTOR_RANGE& _dst_ranges
                                  , uint32_t                             _num_descriptors)
    {
        src_ranges      ->emplace_back(_src_ranges);
        dst_ranges      ->emplace_back(_dst_ranges);
        num_descriptors ->emplace_back(_num_descriptors);
        return *this;
    }
    CopyDescriptorTable& AddRange(  uint32_t _src_range_index, uint32_t _src_first_array_element
                                  , uint32_t _dst_range_index, uint32_t _dst_first_array_element
                                  , uint32_t _num_descriptors)
    {
        src_ranges      ->emplace_back(buma3d::COPY_DESCRIPTOR_RANGE{ _src_range_index, _src_first_array_element });
        dst_ranges      ->emplace_back(buma3d::COPY_DESCRIPTOR_RANGE{ _dst_range_index, _dst_first_array_element });
        num_descriptors ->emplace_back(_num_descriptors);
        return *this;
    }
    const buma3d::COPY_DESCRIPTOR_TABLE& Get()
    {
        table.num_ranges        = (uint32_t)src_ranges->size();
        table.src_ranges        = src_ranges->data();
        table.dst_ranges        = dst_ranges->data();
        table.num_descriptors   = num_descriptors->data();
        return table;
    }

private:
    buma3d::COPY_DESCRIPTOR_TABLE                                table;
    std::shared_ptr<std::vector<buma3d::COPY_DESCRIPTOR_RANGE>>  src_ranges;
    std::shared_ptr<std::vector<buma3d::COPY_DESCRIPTOR_RANGE>>  dst_ranges;
    std::shared_ptr<std::vector<uint32_t>>                       num_descriptors;

};

class CopyDescriptorSet
{
public:
    CopyDescriptorSet()
        : copy_set              {}
        , tables                {}
        , b3d_tables            {}
        , dynamic_descriptors   {}
    {
        tables                = std::make_shared<std::vector<CopyDescriptorTable>              >();
        b3d_tables            = std::make_shared<std::vector<buma3d::COPY_DESCRIPTOR_TABLE>    >();
        dynamic_descriptors   = std::make_shared<std::vector<buma3d::COPY_DYNAMIC_DESCRIPTOR>  >();
    }

    ~CopyDescriptorSet()
    {
    }

    CopyDescriptorTable& AddNewCopyTable()
    {
        return tables->emplace_back();
    }
    CopyDescriptorSet& AddCopyDynamicDescriptor(uint32_t _src_root_parameter_index, uint32_t _dst_root_parameter_index)
    {
        dynamic_descriptors->emplace_back(buma3d::COPY_DYNAMIC_DESCRIPTOR{ _src_root_parameter_index, _dst_root_parameter_index });
        return *this;
    }
    void Finalize(buma3d::IDescriptorSet* _src_set, buma3d::IDescriptorSet* _dst_set)
    {
        b3d_tables->resize(tables->size());
        size_t cnt = 0;
        for (auto& i : *tables)
            b3d_tables->data()[cnt++] = i.Get();

        copy_set.src_set = _src_set;
        copy_set.dst_set = _dst_set;
    }
    const buma3d::COPY_DESCRIPTOR_SET& Get()
    {
        copy_set.num_descriptor_tables   = (uint32_t)b3d_tables->size();
        copy_set.descriptor_tables       =           b3d_tables->data();
        copy_set.num_dynamic_descriptors = (uint32_t)dynamic_descriptors->size();
        copy_set.dynamic_descriptors     =           dynamic_descriptors->data();
        return copy_set;
    }

private:
    buma3d::COPY_DESCRIPTOR_SET                                     copy_set;
    std::shared_ptr<std::vector<CopyDescriptorTable>>               tables;
    std::shared_ptr<std::vector<buma3d::COPY_DESCRIPTOR_TABLE>>     b3d_tables;
    std::shared_ptr<std::vector<buma3d::COPY_DYNAMIC_DESCRIPTOR>>   dynamic_descriptors;

};

class UpdateDescriptorSetDesc
{
public:
    UpdateDescriptorSetDesc()
        : update_desc               {}
        , write_descriptor_sets     {}
        , copy_descriptor_sets      {}
        , b3d_write_descriptor_sets {}
        , b3d_copy_descriptor_sets  {}
    {
        write_descriptor_sets     = std::make_shared<std::vector<WriteDescriptorSet>>();
        copy_descriptor_sets      = std::make_shared<std::vector<CopyDescriptorSet>>();
        b3d_write_descriptor_sets = std::make_shared<std::vector<buma3d::WRITE_DESCRIPTOR_SET>>();
        b3d_copy_descriptor_sets  = std::make_shared<std::vector<buma3d::COPY_DESCRIPTOR_SET>>();
    }

    ~UpdateDescriptorSetDesc()
    {
    }

    WriteDescriptorSet& AddNewWriteDescriptorSets()
    {
        return write_descriptor_sets->emplace_back();
    }
    CopyDescriptorSet& AddNewCopyDescriptorSets()
    {
        return copy_descriptor_sets->emplace_back();
    }

    void Reset()
    {
        update_desc = {};
        write_descriptor_sets       ->clear();
        copy_descriptor_sets        ->clear();
        b3d_write_descriptor_sets   ->clear();
        b3d_copy_descriptor_sets    ->clear();
    }

    void Finalize()
    {
        b3d_write_descriptor_sets->resize(write_descriptor_sets->size());
        b3d_copy_descriptor_sets->resize(copy_descriptor_sets->size());

        size_t cnt = 0;
        for (auto& i : *write_descriptor_sets)
            b3d_write_descriptor_sets->data()[cnt++] = i.Get();

        cnt = 0;
        for (auto& i : *copy_descriptor_sets)
            b3d_copy_descriptor_sets->data()[cnt++] = i.Get();
    }

    const buma3d::UPDATE_DESCRIPTOR_SET_DESC& Get()
    {
        update_desc.num_write_descriptor_sets   = (uint32_t)b3d_write_descriptor_sets->size();
        update_desc.write_descriptor_sets       =           b3d_write_descriptor_sets->data();
        update_desc.num_copy_descriptor_sets    = (uint32_t)b3d_copy_descriptor_sets->size();
        update_desc.copy_descriptor_sets        =           b3d_copy_descriptor_sets->data();
        return update_desc;
    }

private:
    buma3d::UPDATE_DESCRIPTOR_SET_DESC                          update_desc;
    std::shared_ptr<std::vector<WriteDescriptorSet>>            write_descriptor_sets;
    std::shared_ptr<std::vector<CopyDescriptorSet>>             copy_descriptor_sets;
    std::shared_ptr<std::vector<buma3d::WRITE_DESCRIPTOR_SET>>  b3d_write_descriptor_sets;
    std::shared_ptr<std::vector<buma3d::COPY_DESCRIPTOR_SET>>   b3d_copy_descriptor_sets;

};

#pragma endregion descriptor update

#pragma region input layout builder

class InputElementDesc
{
public:
    InputElementDesc(const char* _semantic_name, uint32_t _semantic_index, buma3d::RESOURCE_FORMAT _format, uint32_t _aligned_byte_offset = buma3d::B3D_APPEND_ALIGNED_ELEMENT)
        : desc          { nullptr        , _semantic_index, _format, _aligned_byte_offset }
        , semantic_name { _semantic_name }
    {
        desc.semantic_name = semantic_name.c_str();
    }

    ~InputElementDesc()
    {
    }

    void Reset()
    {
        semantic_name.clear();
        desc = {};
    }

    void Set(const char* _semantic_name, uint32_t _semantic_index, buma3d::RESOURCE_FORMAT _format, uint32_t _aligned_byte_offset = buma3d::B3D_APPEND_ALIGNED_ELEMENT)
    {
        semantic_name   = _semantic_name;
        desc            = { nullptr, _semantic_index, _format, _aligned_byte_offset };
        desc.semantic_name = semantic_name.c_str();
    }
    const buma3d::INPUT_ELEMENT_DESC& Get() { return desc; }

private:
    std::string                 semantic_name;
    buma3d::INPUT_ELEMENT_DESC  desc;

};

class InputSlotDesc
{
public:
    InputSlotDesc(const char* _semantic_name, uint32_t _semantic_index, buma3d::RESOURCE_FORMAT _format, uint32_t _aligned_byte_offset = buma3d::B3D_APPEND_ALIGNED_ELEMENT)
        : desc          {}
        , elements      {}
        , b3d_elements  {}
    {
        elements     = std::make_shared<std::vector<InputElementDesc>>();
        b3d_elements = std::make_shared<std::vector<buma3d::INPUT_ELEMENT_DESC>>();
    }

    ~InputSlotDesc()
    {
    }

    void Reset()
    {
        desc = {};
        for (auto& i : *elements)
            i.Reset();
    }

    InputSlotDesc& SetSlotNumber          (uint32_t                     _slot_number)     { desc.slot_number             = _slot_number;     return *this; }
    InputSlotDesc& SetStrideInBytes       (uint32_t                     _stride_in_bytes) { desc.stride_in_bytes         = _stride_in_bytes; return *this; }
    InputSlotDesc& SetClassification      (buma3d::INPUT_CLASSIFICATION _classification)  { desc.classification          = _classification;  return *this; }
    InputSlotDesc& SetInstanceDataStepRate(uint32_t                     _step_rate)       { desc.instance_data_step_rate = _step_rate;       return *this; }

    InputElementDesc& AddNewInputElement()
    {
        Resize(desc.num_elements + 1);
        return elements->data()[desc.num_elements++];
    }
    const buma3d::INPUT_SLOT_DESC& Get()
    {
        auto e    = elements->data();
        auto b3de = b3d_elements->data();
        for (uint32_t i = 0; i < desc.num_elements; i++)
            b3de[i] = e[i].Get();

        return desc;
    }

private:
    void Resize(uint32_t _num_elements)
    {
        if (_num_elements > desc.num_elements)
        {
            elements->resize(_num_elements);
            b3d_elements->resize(_num_elements);
            desc.elements = b3d_elements->data();
        }
    }

private:
    buma3d::INPUT_SLOT_DESC                                     desc;
    std::shared_ptr<std::vector<InputElementDesc>>              elements;
    std::shared_ptr<std::vector<buma3d::INPUT_ELEMENT_DESC>>    b3d_elements;

};

class InputLayoutDesc
{
public:
    InputLayoutDesc()
        : desc      {}
        , slots     {}
        , b3d_slots {}
    {
        slots     = std::make_shared<std::vector<InputSlotDesc>>();
        b3d_slots = std::make_shared<std::vector<buma3d::INPUT_SLOT_DESC>>();
    }

    ~InputLayoutDesc()
    {

    }

    void Reset()
    {
        desc = {};
        for (auto& i : *slots)
            i.Reset();
    }

    InputSlotDesc& AddNewInputSlot()
    {
        Resize(desc.num_input_slots + 1);
        return slots->data()[desc.num_input_slots++];
    }

    const buma3d::INPUT_LAYOUT_DESC& Get()
    {
        auto s    = slots->data();
        auto b3ds = b3d_slots->data();
        for (uint32_t i = 0; i < desc.num_input_slots; i++)
            b3ds[i] = s[i].Get();

        return desc;
    }

private:
    void Resize(uint32_t _num_slots)
    {
        if (_num_slots > desc.num_input_slots)
        {
            slots->resize(_num_slots);
            b3d_slots->resize(_num_slots);
            desc.input_slots = b3d_slots->data();
        }
    }

private:
    buma3d::INPUT_LAYOUT_DESC                               desc;
    std::shared_ptr<std::vector<InputSlotDesc>>             slots;
    std::shared_ptr<std::vector<buma3d::INPUT_SLOT_DESC>>   b3d_slots;

};

#pragma endregion input layout builder

#pragma blend state

class RenderTargetBlendDesc
{
public:
    RenderTargetBlendDesc()
        : desc{}
    {
    }

    ~RenderTargetBlendDesc()
    {
    }

    RenderTargetBlendDesc& Src      (buma3d::BLEND_FACTOR _factor) { desc.src_blend = _factor; return *this; }
    RenderTargetBlendDesc& Op       (buma3d::BLEND_OP     _op)     { desc.blend_op  = _op;     return *this; }
    RenderTargetBlendDesc& Dst      (buma3d::BLEND_FACTOR _factor) { desc.dst_blend = _factor; return *this; }

    RenderTargetBlendDesc& SrcAlpha (buma3d::BLEND_FACTOR _factor) { desc.src_blend_alpha = _factor; return *this; }
    RenderTargetBlendDesc& OpAlpha  (buma3d::BLEND_OP     _op)     { desc.blend_op_alpha  = _op;     return *this; }
    RenderTargetBlendDesc& DstAlpha (buma3d::BLEND_FACTOR _factor) { desc.dst_blend_alpha = _factor; return *this; }

    RenderTargetBlendDesc& ColorWriteMask(buma3d::COLOR_WRITE_FLAGS _color_write_mask = buma3d::COLOR_WRITE_FLAG_ALL) { desc.color_write_mask = _color_write_mask; return *this; }

    void BlendDisabled(buma3d::COLOR_WRITE_FLAGS _color_write_mask = buma3d::COLOR_WRITE_FLAG_ALL)
    {
        desc = {};
        desc.is_enabled_blend = false;
        desc.color_write_mask = _color_write_mask;
    }
    void BlendAdditive(buma3d::COLOR_WRITE_FLAGS _color_write_mask = buma3d::COLOR_WRITE_FLAG_ALL)
    {
        desc.is_enabled_blend = true;
        desc.src_blend        = buma3d::BLEND_FACTOR_SRC_ALPHA;
        desc.dst_blend        = buma3d::BLEND_FACTOR_ONE;
        desc.blend_op         = buma3d::BLEND_OP_ADD;
        desc.src_blend_alpha  = buma3d::BLEND_FACTOR_ZERO;
        desc.dst_blend_alpha  = buma3d::BLEND_FACTOR_ONE;
        desc.blend_op_alpha   = buma3d::BLEND_OP_ADD;
        desc.color_write_mask = _color_write_mask;
    }
    void BlendSubtractive(buma3d::COLOR_WRITE_FLAGS _color_write_mask = buma3d::COLOR_WRITE_FLAG_ALL)
    {
        desc.is_enabled_blend = true;
        desc.src_blend        = buma3d::BLEND_FACTOR_SRC_ALPHA;
        desc.dst_blend        = buma3d::BLEND_FACTOR_ONE;
        desc.blend_op         = buma3d::BLEND_OP_REVERSE_SUBTRACT;
        desc.src_blend_alpha  = buma3d::BLEND_FACTOR_ZERO;
        desc.dst_blend_alpha  = buma3d::BLEND_FACTOR_ONE;
        desc.blend_op_alpha   = buma3d::BLEND_OP_ADD;
        desc.color_write_mask = _color_write_mask;
    }
    void BlendAlpha(buma3d::COLOR_WRITE_FLAGS _color_write_mask = buma3d::COLOR_WRITE_FLAG_ALL)
    {
        desc.is_enabled_blend = true;
        desc.src_blend        = buma3d::BLEND_FACTOR_SRC_ALPHA; // src.rgb * src.a
        desc.dst_blend        = buma3d::BLEND_FACTOR_SRC_ALPHA_INVERTED;
        desc.blend_op         = buma3d::BLEND_OP_ADD;
        desc.src_blend_alpha  = buma3d::BLEND_FACTOR_ONE;
        desc.dst_blend_alpha  = buma3d::BLEND_FACTOR_SRC_ALPHA_INVERTED;
        desc.blend_op_alpha   = buma3d::BLEND_OP_ADD;
        desc.color_write_mask = _color_write_mask;
    }
    void BlendPMA(buma3d::COLOR_WRITE_FLAGS _color_write_mask = buma3d::COLOR_WRITE_FLAG_ALL)
    {
        desc.is_enabled_blend = true;
        desc.src_blend        = buma3d::BLEND_FACTOR_ONE; // 事前乗算済み(src.rgb * src.aの結果を画像に焼き込むためsrc.aが1だと指定可能) 加算合成として振る舞うことも可能です。
        desc.dst_blend        = buma3d::BLEND_FACTOR_SRC_ALPHA_INVERTED;
        desc.blend_op         = buma3d::BLEND_OP_ADD;
        desc.src_blend_alpha  = buma3d::BLEND_FACTOR_ONE;
        desc.dst_blend_alpha  = buma3d::BLEND_FACTOR_SRC_ALPHA_INVERTED;
        desc.blend_op_alpha   = buma3d::BLEND_OP_ADD;
        desc.color_write_mask = _color_write_mask;
    }

    const buma3d::RENDER_TARGET_BLEND_DESC& Get() const { return desc; }

private:
    buma3d::RENDER_TARGET_BLEND_DESC desc;

};

class BlendStateDesc
{
public:
    BlendStateDesc()
        : desc      {}
        , blend     {}
        , b3d_blend {}
    {
        Reset();
    }

    ~BlendStateDesc()
    {
    }

    const buma3d::BLEND_STATE_DESC& Reset()
    {
        for (auto& i : blend)
            i.BlendDisabled();

        desc.is_enabled_independent_blend = false;
        desc.is_enabled_logic_op          = false;
        desc.logic_op                     = buma3d::LOGIC_OP_CLEAR;
        desc.num_attachments              = 0;
        desc.blend_constants              = { 1,1,1,1 };
    }

    BlendStateDesc&         SetLogicOp                  (buma3d::LOGIC_OP _logic_op)       { desc.logic_op = _logic_op; desc.is_enabled_logic_op = true; return *this; }

    BlendStateDesc&         SetBlendConstants           (const buma3d::COLOR4& _constants) { desc.blend_constants = _constants; return *this; }
    BlendStateDesc&         SetIndependentBlendEnabled  (bool _is_enabled)                 { desc.is_enabled_independent_blend = _is_enabled; desc.is_enabled_logic_op = false; return *this; }
    BlendStateDesc&         SetNumAttachmemns           (uint32_t _num_attachments)        { desc.num_attachments = _num_attachments; return *this; Resize(_num_attachments); }
    RenderTargetBlendDesc&  GetBlendDesc                (uint32_t _index)                  { return blend[_index]; }

    const buma3d::BLEND_STATE_DESC& Get()
    {
        auto b    = blend.data();
        auto b3db = b3d_blend.data();
        auto c = desc.is_enabled_independent_blend ? desc.num_attachments : std::min(desc.num_attachments, 1u);
        for (uint32_t i = 0; i < c; i++)
            b3db[i] = b[i].Get();

        return desc;
    }

private:
    void Resize(uint32_t _num_attachments)
    {
        if (_num_attachments > desc.num_attachments)
        {
            blend.resize(_num_attachments);
            b3d_blend.resize(_num_attachments);
            desc.attachments = b3d_blend.data();
        }
    }

private:
    buma3d::BLEND_STATE_DESC                        desc;
    std::vector<RenderTargetBlendDesc>              blend;
    std::vector<buma3d::RENDER_TARGET_BLEND_DESC>   b3d_blend;

};

#pragma endregion blend state

#pragma region pipeline shader stages

struct PIPELINE_SHADER_STAGE_DESC
{
    buma3d::PIPELINE_SHADER_STAGE_FLAGS flags;
    buma3d::SHADER_STAGE_FLAG           stage;  
    buma3d::IShaderModule*              module;
    const char*                         entry_point_name;
};

class PipelineShaderStageDescs
{
public:
    PipelineShaderStageDescs()
        : descs{}
    {
    }

    ~PipelineShaderStageDescs()
    {
    }

    void Reset()
    {
        num_shader_stages = 0;
    }

    uint32_t                                    GetSize() const { return num_shader_stages; }
    const buma3d::PIPELINE_SHADER_STAGE_DESC*   Get()     const { return descs.data(); }

    void AddStage(  buma3d::SHADER_STAGE_FLAG           _stage
                  , buma3d::IShaderModule*              _module
                  , const char*                         _entry_point_name
                  , buma3d::PIPELINE_SHADER_STAGE_FLAGS _flags = buma3d::PIPELINE_SHADER_STAGE_FLAG_NONE)
    {
        assert(!(stages & _stage));
        stages |= _stage;
        Resize(num_shader_stages + 1);
        entry_point_names.data()[num_shader_stages].assign(_entry_point_name);
        descs            .data()[num_shader_stages] = {_flags, _stage, _module, entry_point_names.data()[num_shader_stages+1].c_str() };
        num_shader_stages++;
    }

    void AddVS(buma3d::IShaderModule* _module, const char* _entry_point_name, buma3d::PIPELINE_SHADER_STAGE_FLAGS _flags = buma3d::PIPELINE_SHADER_STAGE_FLAG_NONE) { AddStage(buma3d::SHADER_STAGE_FLAG_VERTEX   , _module, _entry_point_name, _flags); }
    void AddHS(buma3d::IShaderModule* _module, const char* _entry_point_name, buma3d::PIPELINE_SHADER_STAGE_FLAGS _flags = buma3d::PIPELINE_SHADER_STAGE_FLAG_NONE) { AddStage(buma3d::SHADER_STAGE_FLAG_HULL     , _module, _entry_point_name, _flags); }
    void AddDS(buma3d::IShaderModule* _module, const char* _entry_point_name, buma3d::PIPELINE_SHADER_STAGE_FLAGS _flags = buma3d::PIPELINE_SHADER_STAGE_FLAG_NONE) { AddStage(buma3d::SHADER_STAGE_FLAG_DOMAIN   , _module, _entry_point_name, _flags); }
    void AddGS(buma3d::IShaderModule* _module, const char* _entry_point_name, buma3d::PIPELINE_SHADER_STAGE_FLAGS _flags = buma3d::PIPELINE_SHADER_STAGE_FLAG_NONE) { AddStage(buma3d::SHADER_STAGE_FLAG_GEOMETRY , _module, _entry_point_name, _flags); }
    void AddPS(buma3d::IShaderModule* _module, const char* _entry_point_name, buma3d::PIPELINE_SHADER_STAGE_FLAGS _flags = buma3d::PIPELINE_SHADER_STAGE_FLAG_NONE) { AddStage(buma3d::SHADER_STAGE_FLAG_PIXEL    , _module, _entry_point_name, _flags); }
    void AddCS(buma3d::IShaderModule* _module, const char* _entry_point_name, buma3d::PIPELINE_SHADER_STAGE_FLAGS _flags = buma3d::PIPELINE_SHADER_STAGE_FLAG_NONE) { AddStage(buma3d::SHADER_STAGE_FLAG_COMPUTE  , _module, _entry_point_name, _flags); }

private:
    void Resize(uint32_t _num_shader_stages)
    {
        if (_num_shader_stages > num_shader_stages)
        {
            descs.resize(_num_shader_stages);
            entry_point_names.resize(_num_shader_stages);
        }
    }

private:
    uint32_t                                        num_shader_stages;
    std::vector<buma3d::PIPELINE_SHADER_STAGE_DESC> descs;
    std::vector<std::string>                        entry_point_names;
    buma3d::SHADER_STAGE_FLAGS                      stages;

};


#pragma endregion pipeline shader stages

#pragma region valhelper

template <typename T>
inline constexpr T AlignUpWithMask(T _value, size_t _mask)
{
    return static_cast<T>((static_cast<size_t>(_value) + _mask) & ~_mask);
}

template <typename T>
inline constexpr T AlignDownWithMask(T _value, size_t _mask)
{
    return static_cast<T>(static_cast<size_t>(_value) & ~_mask);
}

template <typename T>
inline constexpr T AlignUp(T _value, size_t _alignment)
{
    return AlignUpWithMask(_value, _alignment - 1);
}

template <typename T>
inline constexpr T AlignDown(T _value, size_t _alignment)
{
    return AlignDownWithMask(_value, _alignment - 1);
}

template <typename T>
inline constexpr bool IsAligned(T _value, size_t _alignment)
{
    return (static_cast<size_t>(_value) & (_alignment - 1)) == 0;
}

template <typename T>
inline constexpr T DivideByMultiple(T _value, size_t _alignment)
{
    return static_cast<T>((static_cast<size_t>(_value) + _alignment - 1) / _alignment);
}

template<typename T, typename RetT = size_t>
inline constexpr RetT Get32BitValues()
{
    return AlignUp(sizeof(T), 4) / 4;
}

uint8_t Buma3DBitScanForward(unsigned long* _result_index, uint32_t _bitmask);
uint8_t Buma3DBitScanForward(unsigned long* _result_index, uint64_t _bitmask);
uint8_t Buma3DBitScanReverse(unsigned long* _result_index, uint32_t _bitmask);
uint8_t Buma3DBitScanReverse(unsigned long* _result_index, uint64_t _bitmask);

template <typename T, std::enable_if_t<sizeof(T) == sizeof(uint32_t), int> = 0>
inline int GetFirstBitIndex(T _bits)
{
    unsigned long index = 0;
    auto res = Buma3DBitScanForward(&index, static_cast<uint32_t>(_bits));
    return res ? static_cast<int>(index) : -1;
}

template <typename T, std::enable_if_t<sizeof(T) == sizeof(uint64_t), int> = 0>
inline int GetFirstBitIndex(T _bits)
{
    unsigned long index = 0;
    auto res = Buma3DBitScanForward(&index, static_cast<uint64_t>(_bits));
    return res ? static_cast<int>(index) : -1;
}

template <typename T, std::enable_if_t<sizeof(T) == sizeof(uint32_t), int> = 0>
inline int GetLastBitIndex(T _bits)
{
    unsigned long index = 0;
    auto res = Buma3DBitScanReverse(&index, static_cast<uint32_t>(_bits));
    return res ? static_cast<int>(index) : -1;
}

template <typename T, std::enable_if_t<sizeof(T) == sizeof(uint64_t), int> = 0>
inline int GetLastBitIndex(T _bits)
{
    unsigned long index = 0;
    auto res = Buma3DBitScanReverse(&index, static_cast<uint64_t>(_bits));
    return res ? static_cast<int>(index) : -1;
}

template<typename T>
inline T Log2(T _value)
{
    if (!_value) return 0;

    int mssb = GetLastBitIndex(_value);  // most significant set bit
    int lssb = GetFirstBitIndex(_value); // least significant set bit
    if (mssb == -1 || lssb == -1)
        return 0;

    // 2の累乗（1セットビットのみ）の場合、ビットのインデックスを返します。
    // それ以外の場合は、最上位のセットビットのインデックスに1を加算して、小数ログを切り上げます。
    return static_cast<T>(mssb) + static_cast<T>(mssb == lssb ? 0 : 1);
}

template<typename T>
inline constexpr T Log2Cexpr(T _value)
{
    if (!_value) return 0;
    int mssb = 0, lssb = 0, cnt = 0;

    cnt = (sizeof(T) * 8) - 1;
    while (cnt != -1) { if (_value & static_cast<T>(1ull << cnt)) break; cnt--; }
    mssb = cnt;

    cnt = 0;
    while (cnt < sizeof(T) * 8) { if (_value & static_cast<T>(1ull << cnt)) break; cnt++; }
    lssb = cnt;

    return static_cast<T>(mssb) + static_cast<T>(mssb == lssb ? 0 : 1);
}

template <typename T>
inline T NextPow2(T _value)
{
    return _value == 0 ? 0 : 1 << Log2(_value);
}

template <typename T>
inline bool IsPowOfTwo(T _value)
{
    return _value > 0 && (_value & (_value - 1)) == 0;
}

inline constexpr size_t Kib(size_t _x) { return 1024 * _x; }
inline constexpr size_t Mib(size_t _x) { return Kib(1024) * _x; }
inline constexpr size_t Gib(size_t _x) { return Mib(1024) * _x; }


#pragma endregion valhelper

inline bool IsSucceeded(buma3d::BMRESULT _bmr)
{
    return _bmr < buma3d::BMRESULT_FAILED;
}

inline bool IsFailed(buma3d::BMRESULT _bmr)
{
    return _bmr >= buma3d::BMRESULT_FAILED;
}

}// namespace util
}// namespace buma
