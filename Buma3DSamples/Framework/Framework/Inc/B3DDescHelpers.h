#pragma once
#include <Buma3D.h>
#include <Util/Buma3DPtr.h>

#include <cassert>
#include <vector>
#include <memory>

namespace buma
{
namespace util
{

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
    uint64_t value;
    FENCE_VALUES& operator++()    { ++value; return *this; } 
    FENCE_VALUES  operator++(int) { auto tmp = *this; value++; return tmp; }
    uint64_t wait  () const { return value; }
    uint64_t signal() const { return value + 1; }
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

#pragma region pipeline layout

class DescriptorSetLayoutDesc
{
public:
    DescriptorSetLayoutDesc()
        : desc      {}
        , bindings  {}
    {}
    DescriptorSetLayoutDesc(uint32_t _num_bindings_reserve)
        : desc      {}
        , bindings  { _num_bindings_reserve }
    {}
    DescriptorSetLayoutDesc(const DescriptorSetLayoutDesc&) = delete;
    ~DescriptorSetLayoutDesc()
    {}

    DescriptorSetLayoutDesc& Reset()
    {
        desc = {};
        for (auto& i : bindings) i = {};
        return *this;
    }

    DescriptorSetLayoutDesc& AddNewBinding(  buma3d::DESCRIPTOR_TYPE     _descriptor_type
                                           , uint32_t                    _base_shader_register
                                           , uint32_t                    _num_descriptors   = 1
                                           , buma3d::SHADER_VISIBILITY   _shader_visibility = buma3d::SHADER_VISIBILITY_ALL_GRAPHICS_COMPUTE
                                           , buma3d::DESCRIPTOR_FLAGS    _flags             = buma3d::DEPENDENCY_FLAG_NONE)
    {
        Resize(desc.num_bindings + 1);
        auto&& b = bindings.data()[desc.num_bindings++];
        b.descriptor_type      = _descriptor_type;
        b.base_shader_register = _base_shader_register;
        b.num_descriptors      = _num_descriptors;
        b.shader_visibility    = _shader_visibility;
        b.flags                = _flags;
        b.static_sampler       = nullptr;
        return *this;
    }
    DescriptorSetLayoutDesc& AddNewBinding(uint32_t                     _base_shader_register
                                           , buma3d::ISamplerView*      _static_sampler
                                           , buma3d::SHADER_VISIBILITY  _shader_visibility = buma3d::SHADER_VISIBILITY_ALL_GRAPHICS_COMPUTE)
    {
        Resize(desc.num_bindings + 1);
        auto&& b = bindings.data()[desc.num_bindings++];
        b.descriptor_type      = buma3d::DESCRIPTOR_TYPE_SAMPLER;
        b.base_shader_register = _base_shader_register;
        b.num_descriptors      = 1;
        b.shader_visibility    = _shader_visibility;
        b.flags                = buma3d::DEPENDENCY_FLAG_NONE;
        b.static_sampler       = _static_sampler;
        return *this;
    }
    DescriptorSetLayoutDesc& SetFlags(buma3d::DESCRIPTOR_SET_LAYOUT_FLAGS _flags)
    {
        desc.flags = _flags;
        return *this;
    }
    void Finalize()
    {
        desc.bindings = bindings.data();
    }

    const buma3d::DESCRIPTOR_SET_LAYOUT_DESC& Get() const { return desc; }

private:
    void Resize(uint32_t _num_bindings)
    {
        if (_num_bindings > bindings.size())
        {
            bindings.resize(_num_bindings);
        }
    }

private:
    buma3d::DESCRIPTOR_SET_LAYOUT_DESC                  desc;
    std::vector<buma3d::DESCRIPTOR_SET_LAYOUT_BINDING>  bindings;

};

class PipelineLayoutDesc
{
public:
    PipelineLayoutDesc()
        : desc              {}
        , set_layouts       {}
        , push_constants    {}
    {}
    PipelineLayoutDesc(uint32_t _num_set_layouts_reserve, uint32_t _num_push_descriptors_reserve)
        : desc              {}
        , set_layouts       { _num_set_layouts_reserve }
        , push_constants    { _num_push_descriptors_reserve }
    {}
    ~PipelineLayoutDesc()
    {}

    PipelineLayoutDesc& Reset()
    {
        desc = {};
        for (auto& i : set_layouts)    i = nullptr;
        for (auto& i : push_constants) i = {};
        return *this;
    }

    PipelineLayoutDesc& SetNumLayouts(uint32_t _num_descriptor_set_layouts)
    {
        desc.num_push_constants = _num_descriptor_set_layouts;
        if (_num_descriptor_set_layouts > (uint32_t)set_layouts.size())
            set_layouts.resize(_num_descriptor_set_layouts);
        return *this;
    }
    PipelineLayoutDesc& SetLayout(uint32_t _descriptor_set_layout_index, buma3d::IDescriptorSetLayout* _layout)
    {
        set_layouts.data()[_descriptor_set_layout_index] = _layout;
        return *this;
    }
    PipelineLayoutDesc& SetLayouts(uint32_t _offset, const std::vector<buma3d::IDescriptorSetLayout*>& _layouts)
    {
        auto set_layouts_data = set_layouts.data();
        for (auto& i : _layouts)
            set_layouts_data[_offset++] = i;

        return *this;
    }
    PipelineLayoutDesc& AddNewPushConstantParameter(  uint32_t                  _num_32bit_values
                                                    , uint32_t                  _shader_register
                                                    , buma3d::SHADER_VISIBILITY _visibility     = buma3d::SHADER_VISIBILITY_ALL_GRAPHICS_COMPUTE
                                                    , uint32_t                  _register_space = 0)
    {
        if (desc.num_push_constants + 1 > (uint32_t)push_constants.size())
            push_constants.resize(desc.num_push_constants + 1);

        auto&& p = push_constants.data()[desc.num_push_constants++];
        p.visibility       = _visibility;
        p.shader_register  = _shader_register;
        p.register_space   = _register_space;
        p.num_32bit_values = _num_32bit_values;

        return *this;
    }
    PipelineLayoutDesc& SetFlags(buma3d::PIPELINE_LAYOUT_FLAGS _flags)
    {
        desc.flags = _flags;
        return *this;
    }
    void Finalize()
    {
        desc.set_layouts = set_layouts.data();
        desc.push_constants = push_constants.data();
    }

    const buma3d::PIPELINE_LAYOUT_DESC& Get() const { return desc; }

private:
    buma3d::PIPELINE_LAYOUT_DESC                    desc;
    std::vector<buma3d::IDescriptorSetLayout*>      set_layouts;
    std::vector<buma3d::PUSH_CONSTANT_PARAMETER>    push_constants;

};


#pragma endregion pipeline layout

#pragma region deprecated descriptor set update

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

class WriteDescriptorSet0
{
public:
    WriteDescriptorSet0()
        : write_set             {}
        , tables                {}
        , b3d_tables            {}
        , dynamic_descriptors   {}
    {
        tables              = std::make_shared<std::vector<WriteDescriptorTable>>();
        b3d_tables          = std::make_shared<std::vector<buma3d::WRITE_DESCRIPTOR_TABLE>>();
        dynamic_descriptors = std::make_shared<std::vector<buma3d::WRITE_DYNAMIC_DESCRIPTOR0>>();
    }

    ~WriteDescriptorSet0()
    {
    }

    WriteDescriptorTable& AddNewWriteDescriptorTable()
    {
        return tables->emplace_back();
    }
    WriteDescriptorSet0& AddWriteDynamicDescriptor(  uint32_t          _dst_root_parameter_index
                                                   , buma3d::IView*    _src_view
                                                   , uint64_t          _src_view_buffer_offset)
    {
        dynamic_descriptors->emplace_back(buma3d::WRITE_DYNAMIC_DESCRIPTOR0{ _dst_root_parameter_index, _src_view, _src_view_buffer_offset });
        return *this;
    }

    void Finalize(buma3d::IDescriptorSet0* _dst_set)
    {
        write_set.dst_set = _dst_set;

        b3d_tables->resize(tables->size());
        size_t cnt = 0;
        for (auto& i : *tables)
            b3d_tables->data()[cnt++] = i.Get();
    }
    const buma3d::WRITE_DESCRIPTOR_SET0& Get()
    {
        write_set.num_descriptor_tables   = (uint32_t)b3d_tables->size();
        write_set.descriptor_tables       =           b3d_tables->data();
        write_set.num_dynamic_descriptors = (uint32_t)dynamic_descriptors->size();
        write_set.dynamic_descriptors     =           dynamic_descriptors->data();
        return write_set;
    }

private:
    buma3d::WRITE_DESCRIPTOR_SET0                                   write_set;
    std::shared_ptr<std::vector<WriteDescriptorTable>>              tables;
    std::shared_ptr<std::vector<buma3d::WRITE_DESCRIPTOR_TABLE>>    b3d_tables;
    std::shared_ptr<std::vector<buma3d::WRITE_DYNAMIC_DESCRIPTOR0>> dynamic_descriptors;

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

class CopyDescriptorSet0
{
public:
    CopyDescriptorSet0()
        : copy_set              {}
        , tables                {}
        , b3d_tables            {}
        , dynamic_descriptors   {}
    {
        tables                = std::make_shared<std::vector<CopyDescriptorTable>              >();
        b3d_tables            = std::make_shared<std::vector<buma3d::COPY_DESCRIPTOR_TABLE>    >();
        dynamic_descriptors   = std::make_shared<std::vector<buma3d::COPY_DYNAMIC_DESCRIPTOR>  >();
    }

    ~CopyDescriptorSet0()
    {
    }

    CopyDescriptorTable& AddNewCopyTable()
    {
        return tables->emplace_back();
    }
    CopyDescriptorSet0& AddCopyDynamicDescriptor(uint32_t _src_root_parameter_index, uint32_t _dst_root_parameter_index)
    {
        dynamic_descriptors->emplace_back(buma3d::COPY_DYNAMIC_DESCRIPTOR{ _src_root_parameter_index, _dst_root_parameter_index });
        return *this;
    }
    void Finalize(buma3d::IDescriptorSet0* _src_set, buma3d::IDescriptorSet0* _dst_set)
    {
        b3d_tables->resize(tables->size());
        size_t cnt = 0;
        for (auto& i : *tables)
            b3d_tables->data()[cnt++] = i.Get();

        copy_set.src_set = _src_set;
        copy_set.dst_set = _dst_set;
    }
    const buma3d::COPY_DESCRIPTOR_SET0& Get()
    {
        copy_set.num_descriptor_tables   = (uint32_t)b3d_tables->size();
        copy_set.descriptor_tables       =           b3d_tables->data();
        copy_set.num_dynamic_descriptors = (uint32_t)dynamic_descriptors->size();
        copy_set.dynamic_descriptors     =           dynamic_descriptors->data();
        return copy_set;
    }

private:
    buma3d::COPY_DESCRIPTOR_SET0                                    copy_set;
    std::shared_ptr<std::vector<CopyDescriptorTable>>               tables;
    std::shared_ptr<std::vector<buma3d::COPY_DESCRIPTOR_TABLE>>     b3d_tables;
    std::shared_ptr<std::vector<buma3d::COPY_DYNAMIC_DESCRIPTOR>>   dynamic_descriptors;

};

class UpdateDescriptorSetDesc0
{
public:
    UpdateDescriptorSetDesc0()
        : update_desc               {}
        , write_descriptor_sets     {}
        , copy_descriptor_sets      {}
        , b3d_write_descriptor_sets {}
        , b3d_copy_descriptor_sets  {}
    {
        write_descriptor_sets     = std::make_shared<std::vector<WriteDescriptorSet0>>();
        copy_descriptor_sets      = std::make_shared<std::vector<CopyDescriptorSet0>>();
        b3d_write_descriptor_sets = std::make_shared<std::vector<buma3d::WRITE_DESCRIPTOR_SET0>>();
        b3d_copy_descriptor_sets  = std::make_shared<std::vector<buma3d::COPY_DESCRIPTOR_SET0>>();
    }

    ~UpdateDescriptorSetDesc0()
    {
    }

    WriteDescriptorSet0& AddNewWriteDescriptorSets()
    {
        return write_descriptor_sets->emplace_back();
    }
    CopyDescriptorSet0& AddNewCopyDescriptorSets()
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

    const buma3d::UPDATE_DESCRIPTOR_SET_DESC0& Get()
    {
        update_desc.num_write_descriptor_sets   = (uint32_t)b3d_write_descriptor_sets->size();
        update_desc.write_descriptor_sets       =           b3d_write_descriptor_sets->data();
        update_desc.num_copy_descriptor_sets    = (uint32_t)b3d_copy_descriptor_sets->size();
        update_desc.copy_descriptor_sets        =           b3d_copy_descriptor_sets->data();
        return update_desc;
    }

private:
    buma3d::UPDATE_DESCRIPTOR_SET_DESC0                         update_desc;
    std::shared_ptr<std::vector<WriteDescriptorSet0>>           write_descriptor_sets;
    std::shared_ptr<std::vector<CopyDescriptorSet0>>            copy_descriptor_sets;
    std::shared_ptr<std::vector<buma3d::WRITE_DESCRIPTOR_SET0>> b3d_write_descriptor_sets;
    std::shared_ptr<std::vector<buma3d::COPY_DESCRIPTOR_SET0>>  b3d_copy_descriptor_sets;

};

#pragma endregion deprecated descriptor set update

#pragma region descriptor set update

class WriteDescriptorBinding;
class WriteDescriptorSet;
class CopyDescriptorBinding;
class CopyDescriptorSet;
class UpdateDescriptorSetDesc;

class WriteDescriptorBinding
{
public:
    WriteDescriptorBinding(WriteDescriptorSet* _parent)
        : parent    { _parent }
        , binding   {}
        , src_views {}
    {}
    ~WriteDescriptorBinding() {}

    WriteDescriptorBinding& Reset(WriteDescriptorSet* _parent)
    {
        parent = _parent;
        binding = {};
        for (auto& i : src_views) i = nullptr;
        return *this;
    }

    WriteDescriptorBinding& SetDstBinding(uint32_t _dst_binding_index, uint32_t _dst_first_array_element = 0)
    {
        binding.dst_binding_index = _dst_binding_index;
        binding.dst_first_array_element = _dst_first_array_element;
        return *this;
    }
    WriteDescriptorBinding& SetNumDescriptors(uint32_t _num_descriptors)
    {
        binding.num_descriptors = _num_descriptors;
        if (_num_descriptors < (uint32_t)src_views.size())
            src_views.resize(_num_descriptors);
        return *this;
    }
    template<typename T>
    WriteDescriptorBinding& SetSrcView(uint32_t _index, T* _view)
    {
        return SetSrcView(_index, static_cast<buma3d::IView*>(_view));
    }
    template<>
    WriteDescriptorBinding& SetSrcView(uint32_t _index, buma3d::IView* _view)
    {
        src_views.data()[_index] = _view;
        return *this;
    }
    WriteDescriptorBinding& SetSrcViews(uint32_t _offset, const std::initializer_list<buma3d::IView*>& _views)
    {
        size_t idx = _offset;
        for (auto& i : _views)
            src_views.data()[idx++] = i;
        return *this;
    }
    WriteDescriptorSet& Finalize()
    {
        binding.src_views = src_views.data();
        return *parent;
    }

    const buma3d::WRITE_DESCRIPTOR_BINDING& Get() const { return binding; }

private:
    WriteDescriptorSet*                 parent;
    buma3d::WRITE_DESCRIPTOR_BINDING    binding;
    std::vector<buma3d::IView*>         src_views;

};

class WriteDescriptorSet
{
public:
    WriteDescriptorSet(UpdateDescriptorSetDesc* _parent)
        : parent                { _parent }
        , write_set             {}
        , bindings              {}
        , b3d_bindings          {}
        , dynamic_descriptors   {}
    {}
    ~WriteDescriptorSet()
    {}

    WriteDescriptorSet& Reset(UpdateDescriptorSetDesc* _parent)
    {
        parent  = _parent;
        write_set = {};
        for (auto& i : bindings)            i.Reset(this);
        for (auto& i : b3d_bindings)        i = {};
        for (auto& i : dynamic_descriptors) i = {};
        return *this;
    }

    WriteDescriptorSet& SetDst(buma3d::IDescriptorSet* _dst_set)
    {
        write_set.dst_set = _dst_set;
        return *this;
    }
    WriteDescriptorBinding& AddNewWriteDescriptorBinding()
    {
        Resize(write_set.num_bindings + 1);
        return bindings.data()[write_set.num_bindings++];
    }
    WriteDescriptorSet& AddWriteDynamicDescriptor(  uint32_t          _dst_binding_index
                                                  , buma3d::IView*    _src_view
                                                  , int64_t           _src_view_buffer_offset = 0)
    {
        ResizeDynamicBindings(write_set.num_dynamic_bindings + 1);
        dynamic_descriptors.data()[write_set.num_dynamic_bindings++] = buma3d::WRITE_DYNAMIC_DESCRIPTOR_BINDING{ _dst_binding_index, _src_view, _src_view_buffer_offset };
        return *this;
    }
    UpdateDescriptorSetDesc& Finalize()
    {
        auto bindings_data = bindings.data();
        auto b3d_bindings_data = b3d_bindings.data();
        for (uint32_t i = 0; i < write_set.num_bindings; i++)
            b3d_bindings_data[i] = bindings_data[i].Get();

        write_set.bindings         = b3d_bindings_data;
        write_set.dynamic_bindings = dynamic_descriptors.data();

        return *parent;
    }

    const buma3d::WRITE_DESCRIPTOR_SET& Get() const { return write_set; }

private:
    void Resize(uint32_t _num_bindings)
    {
        if (_num_bindings > (uint32_t)bindings.size())
        {
            bindings.resize(_num_bindings, this);
            b3d_bindings.resize(_num_bindings);
        }
    }
    void ResizeDynamicBindings(uint32_t _num_bindings)
    {
        if (_num_bindings > (uint32_t)dynamic_descriptors.size())
        {
            dynamic_descriptors.resize(_num_bindings);
        }
    }

private:
    UpdateDescriptorSetDesc*                                parent;
    buma3d::WRITE_DESCRIPTOR_SET                            write_set;
    std::vector<WriteDescriptorBinding>                     bindings;
    std::vector<buma3d::WRITE_DESCRIPTOR_BINDING>           b3d_bindings;
    std::vector<buma3d::WRITE_DYNAMIC_DESCRIPTOR_BINDING>   dynamic_descriptors;

};

class CopyDescriptorBinding
{
public:
    CopyDescriptorBinding(CopyDescriptorSet* _parent)
        : parent{ _parent }, binding{}
    {}
    ~CopyDescriptorBinding() {}

    CopyDescriptorBinding& Reset(CopyDescriptorSet* _parent)
    {
        parent = _parent;
        binding = {};
        return *this;
    }

    CopyDescriptorBinding& SetSrcBinding(uint32_t _src_binding_index, uint32_t _src_first_array_element = 0)
    {
        binding.src_binding_index       = _src_binding_index;
        binding.src_first_array_element = _src_first_array_element;
        return *this;
    }
    CopyDescriptorBinding& SetDstBinding(uint32_t _dst_binding_index, uint32_t _dst_first_array_element = 0)
    {
        binding.dst_binding_index       = _dst_binding_index;
        binding.dst_first_array_element = _dst_first_array_element;
        return *this;
    }
    CopyDescriptorBinding& SetNumDescriptors(uint32_t _num_descriptors)
    {
        binding.num_descriptors = _num_descriptors;
        return *this;
    }
    CopyDescriptorSet& Finalize() { return *parent; }

    const buma3d::COPY_DESCRIPTOR_BINDING& Get() const { return binding; }

private:
    CopyDescriptorSet*              parent;
    buma3d::COPY_DESCRIPTOR_BINDING binding;

};

class CopyDescriptorSet
{
public:
    CopyDescriptorSet(UpdateDescriptorSetDesc* _parent)
        : parent        { _parent }
        , copy_set      {}
        , bindings      {}
        , b3d_bindings  {}
    {}
    ~CopyDescriptorSet() {}

    CopyDescriptorSet& Reset(UpdateDescriptorSetDesc* _parent)
    {
        parent = _parent;
        copy_set.num_bindings = 0;
        for (auto& i : bindings) i.Reset(this);
        for (auto& i : b3d_bindings) i = {};
        return *this;
    }

    CopyDescriptorSet& SetSrc(buma3d::IDescriptorSet* _src_set)
    {
        copy_set.src_set = _src_set;
        return *this;
    }
    CopyDescriptorSet& SetDst(buma3d::IDescriptorSet* _dst_set)
    {
        copy_set.dst_set = _dst_set;
        return *this;
    }
    CopyDescriptorBinding& AddNewCopyBinding()
    {
        Resize(copy_set.num_bindings + 1);
        return bindings.data()[copy_set.num_bindings++];
    }
    UpdateDescriptorSetDesc& Finalize()
    {
        auto bindings_data     = bindings.data();
        auto b3d_bindings_data = b3d_bindings.data();
        for (uint32_t i = 0; i < copy_set.num_bindings; i++)
            b3d_bindings_data[i] = bindings_data[i].Get();

        copy_set.bindings = b3d_bindings_data;
        return *parent;
    }

    const buma3d::COPY_DESCRIPTOR_SET& Get() const { return copy_set; }

private:
    void Resize(uint32_t _num_bindings)
    {
        if (_num_bindings > bindings.size())
        {
            bindings    .resize(_num_bindings, this);
            b3d_bindings.resize(_num_bindings);
        }
    }

private:
    UpdateDescriptorSetDesc*                        parent;
    buma3d::COPY_DESCRIPTOR_SET                     copy_set;
    std::vector<CopyDescriptorBinding>              bindings;
    std::vector<buma3d::COPY_DESCRIPTOR_BINDING>    b3d_bindings;

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
    {}
    ~UpdateDescriptorSetDesc()
    {}

    void Reset()
    {
        update_desc = {};
        for (auto& i : write_descriptor_sets)     i.Reset(this);
        for (auto& i : copy_descriptor_sets)      i.Reset(this);
        for (auto& i : b3d_write_descriptor_sets) i = {};
        for (auto& i : b3d_copy_descriptor_sets)  i = {};
    }

    WriteDescriptorSet& AddNewWriteDescriptorSet()
    {
        ResizeSets(write_descriptor_sets, b3d_write_descriptor_sets, update_desc.num_write_descriptor_sets + 1);
        return write_descriptor_sets.data()[update_desc.num_write_descriptor_sets++];
    }
    CopyDescriptorSet& AddNewCopyDescriptorSet()
    {
        ResizeSets(copy_descriptor_sets, b3d_copy_descriptor_sets, update_desc.num_copy_descriptor_sets + 1);
        return copy_descriptor_sets.data()[update_desc.num_copy_descriptor_sets++];
    }
    void Finalize()
    {
        auto write_sets_data        = write_descriptor_sets.data();
        auto copy_sets_data         = copy_descriptor_sets.data();
        auto b3d_write_sets_data    = b3d_write_descriptor_sets.data();
        auto b3d_copy_sets_data     = b3d_copy_descriptor_sets.data();

        for (uint32_t i = 0; i < update_desc.num_write_descriptor_sets; i++)
            b3d_write_sets_data[i] = write_sets_data[i].Get();

        for (uint32_t i = 0; i < update_desc.num_copy_descriptor_sets; i++)
            b3d_copy_sets_data[i] = copy_sets_data[i].Get();

        update_desc.write_descriptor_sets = b3d_write_sets_data;
        update_desc.copy_descriptor_sets  = b3d_copy_sets_data;
    }

    const buma3d::UPDATE_DESCRIPTOR_SET_DESC& Get() const { return update_desc; }

private:
    template <typename T, typename U> void ResizeSets(std::vector<T>& _target, std::vector<U>& _b3d_target, uint32_t _num_sets)
    {
        if (_num_sets > (uint32_t)_target.size())
        {
            _target.resize(_num_sets, this);
            _b3d_target.resize(_num_sets);
        }
    }

private:
    buma3d::UPDATE_DESCRIPTOR_SET_DESC          update_desc;
    std::vector<WriteDescriptorSet>             write_descriptor_sets;
    std::vector<CopyDescriptorSet>              copy_descriptor_sets;
    std::vector<buma3d::WRITE_DESCRIPTOR_SET>   b3d_write_descriptor_sets;
    std::vector<buma3d::COPY_DESCRIPTOR_SET>    b3d_copy_descriptor_sets;

};

#pragma endregion descriptor set update


#pragma region descriptor heap/pool

class DescriptorSizes
{
public:
    DescriptorSizes()
        : descriptor_sizes      {}
        , num_sizes             {}
        , total_multiply_count  {}
        , heap_sizes            {}
        , pool_sizes            {}
    {}
    DescriptorSizes(uint32_t _num_descriptor_sizes_reserve)
        : descriptor_sizes      {}
        , num_sizes             {}
        , total_multiply_count  {}
        , heap_sizes            { _num_descriptor_sizes_reserve }
        , pool_sizes            { _num_descriptor_sizes_reserve }
    {}
    ~DescriptorSizes()
    {}

    DescriptorSizes& Reset()
    {
        num_sizes = 0;
        total_multiply_count = 0;
        descriptor_sizes.fill(0);
        for (auto& i : heap_sizes) i = {};
        for (auto& i : pool_sizes) i = {};
        return *this;
    }
    DescriptorSizes& IncrementSize(buma3d::DESCRIPTOR_TYPE _type, uint32_t _size, uint32_t _num_descriptors_multiply = 1)
    {
        total_multiply_count += _num_descriptors_multiply;
        descriptor_sizes[_type] += _size * _num_descriptors_multiply;
        return *this;
    }
    DescriptorSizes& IncrementSizes(buma3d::IDescriptorSetLayout* _layout, uint32_t _num_descriptors_multiply = 1)
    {
        total_multiply_count += _num_descriptors_multiply;
        auto&& desc = _layout->GetDesc();
        for (uint32_t i = 0; i < desc.num_bindings; i++)
            descriptor_sizes[desc.bindings[i].descriptor_type] += desc.bindings[i].num_descriptors * _num_descriptors_multiply;
        return *this;
    }
    DescriptorSizes& DecrementSize(buma3d::DESCRIPTOR_TYPE _type, uint32_t _size, uint32_t _num_descriptors_multiply = 1)
    {
        total_multiply_count -= _num_descriptors_multiply;
        descriptor_sizes[_type] -= _size * _num_descriptors_multiply;
        return *this;
    }
    DescriptorSizes& DecrementSizes(buma3d::IDescriptorSetLayout* _layout, uint32_t _num_descriptors_multiply = 1)
    {
        total_multiply_count -= _num_descriptors_multiply;
        auto&& desc = _layout->GetDesc();
        for (uint32_t i = 0; i < desc.num_bindings; i++)
            descriptor_sizes[desc.bindings[i].descriptor_type] -= desc.bindings[i].num_descriptors * _num_descriptors_multiply;
        return *this;
    }
    DescriptorSizes& Finalize()
    {
        num_sizes = CalcDescriptorSizes();
        ConvertSizes(num_sizes, heap_sizes);
        ConvertSizes(num_sizes, pool_sizes);
        return *this;
    }

    uint32_t                                                       GetMaxSetsByTotalMultiplyCount()     const { return total_multiply_count; }
    uint32_t                                                       GetNumPoolSizes()                    const { return CalcDescriptorSizes(); }
    const std::array<uint32_t, buma3d::DESCRIPTOR_TYPE_NUM_TYPES>& GetDescriptorSizes()                 const { return descriptor_sizes; }

    buma3d::DESCRIPTOR_HEAP_DESC GetAsHeapDesc(buma3d::DESCRIPTOR_HEAP_FLAGS _flags = buma3d::DESCRIPTOR_HEAP_FLAG_NONE, buma3d::NodeMask _node_mask = buma3d::B3D_DEFAULT_NODE_MASK) const
    {
        return buma3d::DESCRIPTOR_HEAP_DESC{
              _flags                        // buma3d::DESCRIPTOR_HEAP_FLAGS       flags;
            , num_sizes                     // uint32_t                            num_heap_sizes;
            , heap_sizes.data()             // const buma3d::DESCRIPTOR_HEAP_SIZE* heap_sizes;
            , _node_mask                    // buma3d::NodeMask                    node_mask;
        };
    }
    buma3d::DESCRIPTOR_POOL_DESC GetAsPoolDesc(buma3d::IDescriptorHeap* _heap, uint32_t _max_sets_allocation_count, buma3d::DESCRIPTOR_POOL_FLAGS _flags = buma3d::DESCRIPTOR_POOL_FLAG_NONE) const
    {
        return buma3d::DESCRIPTOR_POOL_DESC{
              _heap                         // buma3d::IDescriptorHeap*            heap;
            , _flags                        // buma3d::DESCRIPTOR_POOL_FLAGS       flags;
            , _max_sets_allocation_count    // uint32_t                            max_sets_allocation_count;
            , num_sizes                     // uint32_t                            num_pool_sizes;
            , pool_sizes.data()             // const buma3d::DESCRIPTOR_POOL_SIZE* pool_sizes;
        };
    }

private:
    uint32_t CalcDescriptorSizes() const 
    {
        uint32_t cnt = 0;
        for (auto& i : descriptor_sizes)
            if (i != 0) cnt++;
        return cnt;
    }
    template<typename T>
    void ConvertSizes(uint32_t _num_sizes, std::vector<T>& _target)
    {
        if (_num_sizes > (uint32_t)_target.size())
            _target.resize(_num_sizes);

        _num_sizes = 0;
        uint32_t type = (uint32_t)buma3d::DESCRIPTOR_TYPE_CBV;
        auto _target_data = _target.data();
        for (auto& i : descriptor_sizes)
        {
            if (i != 0)
            {
                auto&& s = _target_data[_num_sizes++];
                s.type = static_cast<buma3d::DESCRIPTOR_TYPE>(type);
                s.num_descriptors += i;
            }
            type++;
        }
    }

private:
    std::array<uint32_t, buma3d::DESCRIPTOR_TYPE_NUM_TYPES> descriptor_sizes;
    uint32_t                                                num_sizes; // 非ゼロサイズのディスクリプタサイズ数
    uint32_t                                                total_multiply_count;
    std::vector<buma3d::DESCRIPTOR_HEAP_SIZE>               heap_sizes;
    std::vector<buma3d::DESCRIPTOR_POOL_SIZE>               pool_sizes;

};


class DescriptorSetAllocateDesc
{
public:
    DescriptorSetAllocateDesc()
        : desc      {}
        , layouts   {}
        , dst_sets  {}
    {}
    DescriptorSetAllocateDesc(uint32_t _num_descriptor_sets_reserve)
        : desc      {}
        , layouts   { _num_descriptor_sets_reserve }
        , dst_sets  { _num_descriptor_sets_reserve }
    {}
    ~DescriptorSetAllocateDesc()
    {
        Reset();
    }

    DescriptorSetAllocateDesc& Reset()
    {
        desc = {};
        for (auto& i : layouts)  i = nullptr;
        for (auto& i : dst_sets) util::SafeRelease(i);
        return *this;
    }
    DescriptorSetAllocateDesc& SetNumDescriptorSets(uint32_t _num_descriptor_sets)
    {
        desc.num_descriptor_sets = _num_descriptor_sets;
        if (_num_descriptor_sets > dst_sets.size())
        {
            layouts.resize(_num_descriptor_sets);
            dst_sets.resize(_num_descriptor_sets);
        }
        return *this;
    }
    DescriptorSetAllocateDesc& SetDescriptorSetLayout(uint32_t _index, buma3d::IDescriptorSetLayout* _layout)
    {
        layouts.data()[_index] = _layout;
        return *this;
    }
    DescriptorSetAllocateDesc& SetDescriptorSetLayouts(uint32_t _offset, const std::vector<buma3d::IDescriptorSetLayout*>& _layouts)
    {
        auto layouts_data = layouts.data();
        for (auto& i : _layouts)
            layouts_data[_offset++] = i;
        return *this;
    }
    DescriptorSetAllocateDesc& Finalize()
    {
        desc.set_layouts = layouts.data();
        return *this;
    }

    const buma3d::DESCRIPTOR_SET_ALLOCATE_DESC& Get() const 
    {
        return desc;
    }
    std::pair<uint32_t, buma3d::IDescriptorSet**> GetDst()
    {
        return { desc.num_descriptor_sets, dst_sets.data() };
    }
    std::vector<buma3d::util::Ptr<buma3d::IDescriptorSet>> ConvertDst()
    {
        std::vector<buma3d::util::Ptr<buma3d::IDescriptorSet>> result(desc.num_descriptor_sets);
        auto result_data = result.data();
        auto dst_sets_data = dst_sets.data();
        for (uint32_t i = 0; i < desc.num_descriptor_sets; i++)
            result_data[i] = dst_sets_data[i];

        return result;
    }

private:
    buma3d::DESCRIPTOR_SET_ALLOCATE_DESC        desc;
    std::vector<buma3d::IDescriptorSetLayout*>  layouts;
    std::vector<buma3d::IDescriptorSet*>        dst_sets;

};



#pragma endregion descriptor heap/pool


}// namespace util
}// namespace buma
