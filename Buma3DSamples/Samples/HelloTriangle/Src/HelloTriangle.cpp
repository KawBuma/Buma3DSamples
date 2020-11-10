#include "pch.h"
#include "HelloTriangle.h"

#include <cassert>

namespace buma
{

namespace b = buma3d;

template<typename T>
using Ptr = buma3d::util::Ptr<T>;

HelloTriangle::HelloTriangle()
    : ApplicationBase()
{

}

HelloTriangle::~HelloTriangle()
{

}

HelloTriangle* HelloTriangle::Create()
{
    return new HelloTriangle();
}

void HelloTriangle::Prepare(PlatformBase& _platform)
{
    platform     = &_platform;
    deivce       = dr->GetDevice();
    back_buffers = &platform->GetWindow()->GetBuffers();
}

void HelloTriangle::Init()
{

}

void HelloTriangle::LoadAssets()
{
    b::BMRESULT bmr{};
    // ルートシグネチャの作成
    {
        b::ROOT_SIGNATURE_DESC rsdesc{};
        //b::ROOT_PARAMETER parameters[1]{};

        rsdesc.flags                          = b::ROOT_SIGNATURE_FLAG_NONE;
        rsdesc.raytracing_shader_visibilities = b::RAY_TRACING_SHADER_VISIBILITY_FLAG_NONE;
        rsdesc.num_parameters                 = 0;
        rsdesc.parameters                     = nullptr;
        rsdesc.num_static_samplers            = 0;
        rsdesc.static_samplers                = nullptr;
        rsdesc.num_register_shifts            = 0;
        rsdesc.register_shifts                = nullptr;

        bmr = device->CreateRootSignature(rsdesc, &signature);
        assert(bmr == b::BMRESULT_SUCCEED);
    }

    // RTVの作成
    std::vector<Ptr<b::IRenderTargetView>> back_buffer_rtvs(swapchain->GetDesc().buffer.count);
    {
        auto&& back_buffer_desc = swapchain->GetDesc().buffer;
        b::RENDER_TARGET_VIEW_DESC rtv_desc{};
        rtv_desc.view.type          = b::VIEW_TYPE_RENDER_TARGET;
        rtv_desc.view.format        = sfs_format.format;
        rtv_desc.view.dimension     = b::VIEW_DIMENSION_TEXTURE_2D;
        rtv_desc.texture.components                           = { b::COMPONENT_SWIZZLE_IDENTITY, b::COMPONENT_SWIZZLE_IDENTITY, b::COMPONENT_SWIZZLE_IDENTITY, b::COMPONENT_SWIZZLE_IDENTITY };
        rtv_desc.texture.subresource_range.offset.aspect      = b::TEXTURE_ASPECT_FLAG_COLOR;
        rtv_desc.texture.subresource_range.offset.mip_slice   = 0;
        rtv_desc.texture.subresource_range.offset.array_slice = 0;
        rtv_desc.texture.subresource_range.array_size         = 1;
        rtv_desc.texture.subresource_range.mip_levels         = 1;
        rtv_desc.flags                                        = b::RENDER_TARGET_VIEW_FLAG_NONE;

        for (uint32_t i = 0; i < back_buffer_desc.count; i++)
        {
            bmr = device->CreateRenderTargetView((*back_buffers)[i].tex.Get(), rtv_desc, &back_buffer_rtvs[i]);
            assert(bmr == b::BMRESULT_SUCCEED);
        }
    }

    // レンダーパスの作成
    Ptr<b::IRenderPass> render_pass{};
    {
        b::RENDER_PASS_DESC render_pass_desc{};

        b::ATTACHMENT_DESC attachment{};
        attachment.flags               = b::ATTACHMENT_FLAG_NONE;
        attachment.format              = sfs_format.format;
        attachment.sample_count        = 1;
        attachment.load_op             = b::ATTACHMENT_LOAD_OP_CLEAR;
        attachment.store_op            = b::ATTACHMENT_STORE_OP_STORE;
        attachment.begin_state         = b::RESOURCE_STATE_COLOR_ATTACHMENT_WRITE;
        attachment.end_state           = b::RESOURCE_STATE_PRESENT;

        b::ATTACHMENT_REFERENCE color_attachment_ref{};
        color_attachment_ref.attachment_index             = 0;
        color_attachment_ref.state_at_pass                = b::RESOURCE_STATE_COLOR_ATTACHMENT_READ_WRITE;
        color_attachment_ref.stencil_state_at_pass        = {};
        color_attachment_ref.input_attachment_aspect_mask = b::TEXTURE_ASPECT_FLAG_COLOR;

        b::SUBPASS_DESC subpass_desc{};
        subpass_desc.flags                    = b::SUBPASS_FLAG_NONE;
        subpass_desc.pipeline_bind_point      = b::PIPELINE_BIND_POINT_GRAPHICS;
        subpass_desc.view_mask                = 0x0;
        subpass_desc.num_color_attachments    = 1;
        subpass_desc.color_attachments        = &color_attachment_ref;
        subpass_desc.resolve_attachments      = nullptr;
        subpass_desc.depth_stencil_attachment = nullptr;

        b::SUBPASS_DEPENDENCY dependencies[] = { {},{} };
        dependencies[0].src_subpass      = b::B3D_SUBPASS_EXTERNAL;
        dependencies[0].dst_subpass      = 0;
        dependencies[0].src_stage_mask   = b::PIPELINE_STAGE_FLAG_TOP_OF_PIPE;
        dependencies[0].dst_stage_mask   = b::PIPELINE_STAGE_FLAG_COLOR_ATTACHMENT_OUTPUT;
        dependencies[0].src_access       = b::RESOURCE_ACCESS_FLAG_NONE;
        dependencies[0].dst_access       = b::RESOURCE_ACCESS_FLAG_COLOR_ATTACHMENT_READ | b::RESOURCE_ACCESS_FLAG_COLOR_ATTACHMENT_WRITE;
        dependencies[0].dependency_flags = b::DEPENDENCY_FLAG_NONE;
        dependencies[0].view_offset      = 0;

        dependencies[1].src_subpass      = 0;
        dependencies[1].dst_subpass      = b::B3D_SUBPASS_EXTERNAL;
        dependencies[1].src_stage_mask   = b::PIPELINE_STAGE_FLAG_COLOR_ATTACHMENT_OUTPUT;
        dependencies[1].dst_stage_mask   = b::PIPELINE_STAGE_FLAG_BOTTOM_OF_PIPE;
        dependencies[1].src_access       = b::RESOURCE_ACCESS_FLAG_COLOR_ATTACHMENT_READ | b::RESOURCE_ACCESS_FLAG_COLOR_ATTACHMENT_WRITE;
        dependencies[1].dst_access       = b::RESOURCE_ACCESS_FLAG_NONE;
        dependencies[1].dependency_flags = b::DEPENDENCY_FLAG_NONE;
        dependencies[1].view_offset      = 0;

        render_pass_desc.flags                     = b::RENDER_PASS_FLAG_NONE;
        render_pass_desc.num_attachments           = 1;
        render_pass_desc.attachments               = &attachment;
        render_pass_desc.num_subpasses             = 1;
        render_pass_desc.subpasses                 = &subpass_desc;
        render_pass_desc.num_dependencies          = _countof(dependencies);
        render_pass_desc.dependencies              = dependencies;
        render_pass_desc.num_correlated_view_masks = 0;
        render_pass_desc.correlated_view_masks     = nullptr;

        auto bmr = device->CreateRenderPass(render_pass_desc, &render_pass);
        assert(bmr == b::BMRESULT_SUCCEED);
    }

    // フレームバッファの作成
    std::vector<Ptr<b::IFramebuffer>> framebuffers(BACK_BUFFER_COUNT);
    {
        b::FRAMEBUFFER_DESC fb_desc{};
        for (uint32_t i = 0; i < BACK_BUFFER_COUNT; i++)
        {
            b::IView* attachment = back_buffer_rtvs[i].Get();
            fb_desc.flags           = b::FRAMEBUFFER_FLAG_NONE;
            fb_desc.render_pass     = render_pass.Get();
            fb_desc.num_attachments = 1;
            fb_desc.attachments     = &attachment;

            bmr = device->CreateFramebuffer(fb_desc, &framebuffers[i]);
            assert(bmr == b::BMRESULT_SUCCEED);
        }
    }

    // シェーダモジュールを作成
    std::vector<Ptr<b::IShaderModule>> shader_modules(2);
    {
        LOAD_SHADER_DESC desc{};
        desc.options.packMatricesInRowMajor     = true;        // Experimental: Decide how a matrix get packed
        desc.options.enable16bitTypes           = false;       // Enable 16-bit types, such as half, uint16_t. Requires shader model 6.2+
        desc.options.enableDebugInfo            = false;       // Embed debug info into the binary
        desc.options.disableOptimizations       = false;       // Force to turn off optimizations. Ignore optimizationLevel below.

        desc.options.optimizationLevel          = 3; // 0 to 3, no optimization to most optimization
        desc.options.shaderModel                = { 6, 2 };

        desc.options.shiftAllTexturesBindings   = 0;
        desc.options.shiftAllSamplersBindings   = 0;
        desc.options.shiftAllCBuffersBindings   = 0;
        desc.options.shiftAllUABuffersBindings  = 0;

        // vs
        {
            desc.entry_point    = "main";
            desc.filename       = "./VertexShader.hlsl";
            desc.defines        = {};
            desc.stage          = { ShaderConductor::ShaderStage::VertexShader };
            std::vector<uint8_t> bytecode;
            LoadShaderFromHLSL(desc, &bytecode);
            assert(!bytecode.empty());

            b::SHADER_MODULE_DESC module_desc{};
            module_desc.flags                    = b::SHADER_MODULE_FLAG_NONE;
            module_desc.bytecode.bytecode_length = bytecode.size();
            module_desc.bytecode.shader_bytecode = bytecode.data();
            bmr = device->CreateShaderModule(module_desc, &shader_modules[0]);
            assert(bmr == b::BMRESULT_SUCCEED);
        }

        // ps
        {
            desc.entry_point    = "main";
            desc.filename       = "./PixelShader.hlsl";
            desc.defines        = {};
            desc.stage          = { ShaderConductor::ShaderStage::PixelShader };
            std::vector<uint8_t> bytecode;
            LoadShaderFromHLSL(desc, &bytecode);
            assert(!bytecode.empty());

            b::SHADER_MODULE_DESC module_desc{};
            module_desc.flags                    = b::SHADER_MODULE_FLAG_NONE;
            module_desc.bytecode.bytecode_length = bytecode.size();
            module_desc.bytecode.shader_bytecode = bytecode.data();
            bmr = device->CreateShaderModule(module_desc, &shader_modules[1]);
            assert(bmr == b::BMRESULT_SUCCEED);
        }
    }

    // グラフィックスパイプラインの作成
    Ptr<b::IPipelineState> pipeline{};
    {
        b::GRAPHICS_PIPELINE_STATE_DESC pso_desc{};

        pso_desc.root_signature       = signature.Get();
        pso_desc.render_pass          = render_pass.Get();
        pso_desc.subpass              = 0;
        pso_desc.node_mask            = b::B3D_DEFAULT_NODE_MASK;
        pso_desc.flags                = b::PIPELINE_STATE_FLAG_NONE;

        b::PIPELINE_SHADER_STAGE_DESC shader_stages[2]{};
        {
            shader_stages[0].stage            = b::SHADER_STAGE_FLAG_VERTEX;
            shader_stages[0].entry_point_name = "main";
            shader_stages[0].flags            = b::PIPELINE_SHADER_STAGE_FLAG_NONE;
            shader_stages[0].module           = shader_modules[0].Get();

            shader_stages[1].stage            = b::SHADER_STAGE_FLAG_PIXEL;
            shader_stages[1].entry_point_name = "main";
            shader_stages[1].flags            = b::PIPELINE_SHADER_STAGE_FLAG_NONE;
            shader_stages[1].module           = shader_modules[1].Get();

            pso_desc.num_shader_stages    = 2;
            pso_desc.shader_stages        = shader_stages;
        }

        // 入力レイアウト
        b::INPUT_LAYOUT_DESC  input_layout{};
        b::INPUT_SLOT_DESC    input_slot{};
        b::INPUT_ELEMENT_DESC input_elements[2]{};
        {
            //                  { semantic_name, semantic_index, format                               , aligned_byte_offset           }
            input_elements[0] = { "POSITION"   , 0             , b::RESOURCE_FORMAT_R32G32B32A32_FLOAT, b::B3D_APPEND_ALIGNED_ELEMENT };
            input_elements[1] = { "COLOR"      , 0             , b::RESOURCE_FORMAT_R32G32B32A32_FLOAT, b::B3D_APPEND_ALIGNED_ELEMENT };

            input_slot.slot_number              = 0;
            input_slot.stride_in_bytes          = sizeof(float) * 8;// == RESOURCE_FORMAT_R32G32B32A32_FLOAT * 2
            input_slot.classification           = b::INPUT_CLASSIFICATION_PER_VERTEX_DATA;
            input_slot.instance_data_step_rate  = 0;
            input_slot.num_elements             = _countof(input_elements);
            input_slot.elements                 = input_elements;

            input_layout.num_input_slots  = 1;
            input_layout.input_slots      = &input_slot;

            pso_desc.input_layout = &input_layout;
        }

        b::INPUT_ASSEMBLY_STATE_DESC ia{};
        {
            ia.topology = b::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            pso_desc.input_assembly_state = &ia;
        }

        pso_desc.tessellation_state   = nullptr;
        pso_desc.viewport_state       = nullptr;

        b::RASTERIZATION_STATE_DESC rs{};
        {
            rs.fill_mode                     = b::FILL_MODE_SOLID;
            rs.cull_mode                     = b::CULL_MODE_BACK;
            rs.is_front_counter_clockwise    = false;
            rs.is_enable_depth_clip          = false;
            rs.is_enable_depth_bias          = false;
            rs.depth_bias_scale              = 0;
            rs.depth_bias_clamp              = 0.f;
            rs.depth_bias_slope_scale        = 0.f;
            rs.is_enable_conservative_raster = false;
            rs.line_rasterization_mode       = b::LINE_RASTERIZATION_MODE_DEFAULT;
            rs.line_width                    = 1.f;
            
            pso_desc.rasterization_state  = &rs;
        }

        pso_desc.stream_output        = nullptr;

        b::MULTISAMPLE_STATE_DESC ms{};
        {
            ms.is_enable_alpha_to_coverage      = false;
            ms.is_enable_sample_rate_shading    = false;
            ms.rasterization_samples            = 1;
            ms.sample_masks                     = b::B3D_DEFAULT_SAMPLE_MASK;
            ms.sample_position_state.is_enable  = false;
            ms.sample_position_state.desc       = nullptr;

            pso_desc.multisample_state = &ms;
        }

        b::DEPTH_STENCIL_STATE_DESC ds{};
        {
            ds.is_enable_depth_test        = false;
            ds.is_enable_depth_write       = false;
            ds.depth_comparison_func       = b::COMPARISON_FUNC_NEVER;
            ds.is_enable_depth_bounds_test = false;
            ds.min_depth_bounds            = 0;
            ds.max_depth_bounds            = 1;
            ds.is_enable_stencil_test      = false;
            
            ds.stencil_front_face.fail_op         = b::STENCIL_OP_KEEP;
            ds.stencil_front_face.depth_fail_op   = b::STENCIL_OP_KEEP;
            ds.stencil_front_face.pass_op         = b::STENCIL_OP_KEEP;
            ds.stencil_front_face.comparison_func = b::COMPARISON_FUNC_NEVER;
            ds.stencil_front_face.compare_mask    = b::B3D_DEFAULT_STENCIL_COMPARE_MASK;
            ds.stencil_front_face.write_mask      = b::B3D_DEFAULT_STENCIL_WRITE_MASK;
            ds.stencil_front_face.reference       = b::B3D_DEFAULT_STENCIL_REFERENCE;
            
            ds.stencil_back_face.fail_op         = b::STENCIL_OP_KEEP;
            ds.stencil_back_face.depth_fail_op   = b::STENCIL_OP_KEEP;
            ds.stencil_back_face.pass_op         = b::STENCIL_OP_KEEP;
            ds.stencil_back_face.comparison_func = b::COMPARISON_FUNC_NEVER;
            ds.stencil_back_face.compare_mask    = b::B3D_DEFAULT_STENCIL_COMPARE_MASK;
            ds.stencil_back_face.write_mask      = b::B3D_DEFAULT_STENCIL_WRITE_MASK;
            ds.stencil_back_face.reference       = b::B3D_DEFAULT_STENCIL_REFERENCE;
            
            pso_desc.depth_stencil_state = &ds;
        }

        b::BLEND_STATE_DESC bs{};
        b::RENDER_TARGET_BLEND_DESC attachments{};
        {
            attachments.is_enable_blend  = false;
            attachments.src_blend        = b::BLEND_FACTOR_ONE;
            attachments.dst_blend        = b::BLEND_FACTOR_ONE;
            attachments.blend_op         = b::BLEND_OP_ADD;
            attachments.src_blend_alpha  = b::BLEND_FACTOR_ONE;
            attachments.dst_blend_alpha  = b::BLEND_FACTOR_ONE;
            attachments.blend_op_alpha   = b::BLEND_OP_ADD;
            attachments.color_write_mask = b::COLOR_WRITE_FLAG_ALL;

            bs.is_enable_independent_blend = false;
            bs.is_enable_logic_op          = false;
            bs.logic_op                    = b::LOGIC_OP_SET;
            bs.num_attachments             = 1;
            bs.attachments                 = &attachments;
            bs.blend_constants             = { 1.f, 1.f, 1.f, 1.f };

            pso_desc.blend_state = &bs;
        }

        b::VIEWPORT_STATE_DESC vp{};
        {
            vp.num_viewports        = 1;
            vp.num_scissor_rects    = 1;
            vp.viewports            = nullptr;
            vp.scissor_rects        = nullptr;

            pso_desc.viewport_state = &vp;
        }

        b::DYNAMIC_STATE_DESC   dynamic_state_desc{};
        b::DYNAMIC_STATE        dynamic_states[] = { b::DYNAMIC_STATE_VIEWPORT, b::DYNAMIC_STATE_SCISSOR };
        {
            pso_desc.dynamic_state = &dynamic_state_desc;
            dynamic_state_desc.num_dynamic_states = _countof(dynamic_states);

            dynamic_state_desc.dynamic_states = dynamic_states;
        }

        bmr = device->CreateGraphicsPipelineState(pso_desc, &pipeline);
        assert(bmr == b::BMRESULT_SUCCEED);
    }

    // コマンドアロケータを作成
    std::vector<Ptr<b::ICommandAllocator>> cmd_allocator(BACK_BUFFER_COUNT);
    {
        for (auto& i : cmd_allocator)
        {
            b::COMMAND_ALLOCATOR_DESC cad{};
            cad.type    = b::COMMAND_TYPE_DIRECT;
            cad.level   = b::COMMAND_LIST_LEVEL_PRIMARY;
            cad.flags   = b::COMMAND_ALLOCATOR_FLAG_NONE | b::COMMAND_ALLOCATOR_FLAG_TRANSIENT;

            bmr = device->CreateCommandAllocator(cad, &i);
            assert(bmr == b::BMRESULT_SUCCEED);
        }
    }


    // コマンドリストを作成
    std::vector<Ptr<b::ICommandList>> cmd_lists(BACK_BUFFER_COUNT);
    {
        b::COMMAND_LIST_DESC cld{};
        cld.type      = b::COMMAND_TYPE_DIRECT;
        cld.level     = b::COMMAND_LIST_LEVEL_PRIMARY;
        cld.node_mask = b::B3D_DEFAULT_NODE_MASK;
        uint32_t cnt = 0;
        for (auto& i : cmd_lists)
        {
            cld.allocator = cmd_allocator[cnt].Get();
            bmr = device->AllocateCommandList(cld, &i);
            assert(bmr == b::BMRESULT_SUCCEED);
            i->SetName(std::string("CommandList " + std::to_string(cnt++)).c_str());
        }
    }

    // フェンスを作成
    struct FENCE_VALUES
    {
        FENCE_VALUES& operator++()    { ++wait; ++signal; return *this; } 
        FENCE_VALUES  operator++(int) { auto tmp = *this; wait++; signal++; return tmp; }
        uint64_t wait   = 0;
        uint64_t signal = 1;
    };
    enum SCF { PRESENT_COMPLETE, RENDER_COMPLETE, SWAPCHAIN_FENCE_NUM };
    Ptr<b::IFence>                  util_fence;
    FENCE_VALUES                    fence_values    [BACK_BUFFER_COUNT];
    std::vector<Ptr<b::IFence>>     cmd_fences      (BACK_BUFFER_COUNT);
    std::vector<Ptr<b::IFence>>     swapchain_fences(SWAPCHAIN_FENCE_NUM);
    {
        b::FENCE_DESC fd{};
        fd.flags         = b::FENCE_FLAG_NONE;
        fd.initial_value = 0;

        fd.type = b::FENCE_TYPE_BINARY_GPU_TO_CPU;
        bmr = device->CreateFence(fd, &util_fence);
        assert(bmr == b::BMRESULT_SUCCEED);
        util_fence->SetName("util_fence");

        fd.type = b::FENCE_TYPE_TIMELINE;
        uint32_t cnt = 0;
        for (auto& i : cmd_fences)
        {
            bmr = device->CreateFence(fd, &i);
            assert(bmr == b::BMRESULT_SUCCEED);
            i->SetName(std::string("cmd_fences" + std::to_string(cnt++)).c_str());
        }

        fd.type = b::FENCE_TYPE_BINARY_GPU_TO_GPU;
        cnt = 0;
        for (auto& i : swapchain_fences)
        {
            bmr = device->CreateFence(fd, &i);
            assert(bmr == b::BMRESULT_SUCCEED);
        }
        swapchain_fences[0]->SetName("present_complete_fence");
        swapchain_fences[1]->SetName("render_complete_fence");
    }
    
    struct VERTEX
    {
        b::FLOAT4 position;
        b::FLOAT4 color;
    };
    std::vector<VERTEX> triangle =
    {
          { {  0.0f ,  0.25f * aspect_ratio, 0.0f, 1.f }, { 1.f, 0.f, 0.f, 1.f} }
        , { {  0.25f, -0.25f * aspect_ratio, 0.0f, 1.f }, { 0.f, 1.f, 0.f, 1.f} }
        , { { -0.25f, -0.25f * aspect_ratio, 0.0f, 1.f }, { 0.f, 0.f, 1.f, 1.f} }
    };
    std::vector<uint16_t> index = { 0,1,2 };

    // 頂点バッファリソースの器を作成
    Ptr<b::IBuffer> vertex_buffer{};
    {
        bmr = device->CreatePlacedResource(binit::BufferResourceDesc(sizeof(VERTEX) * triangle.size(), binit::BUF_COPYABLE_FLAGS | b::BUFFER_USAGE_FLAG_VERTEX_BUFFER), &vertex_buffer);
        assert(bmr == b::BMRESULT_SUCCEED);
        vertex_buffer->SetName("Vertex buffer");
    }

    // インデックスバッファリソースの器を作成
    Ptr<b::IBuffer> index_buffer{};
    {
        bmr = device->CreatePlacedResource(binit::BufferResourceDesc(sizeof(decltype(index)::value_type) * index.size(), binit::BUF_COPYABLE_FLAGS | b::BUFFER_USAGE_FLAG_INDEX_BUFFER), &index_buffer);
        assert(bmr == b::BMRESULT_SUCCEED);
        index_buffer->SetName("Index buffer");  
    }

    // バッファのサイズ要件を取得。
    b::RESOURCE_HEAP_ALLOCATION_INFO         heap_alloc_info{};
    std::vector<b::RESOURCE_ALLOCATION_INFO> alloc_infos(2);
    {
        b::IResource* resources[] = { vertex_buffer.Get(), index_buffer.Get() };
        bmr = device->GetResourceAllocationInfo(2, resources, alloc_infos.data(), &heap_alloc_info);
        assert(bmr == b::BMRESULT_SUCCEED);
    }

    // ヒーププロパティを取得
    std::vector<b::RESOURCE_HEAP_PROPERTIES> heap_props;
    {
        heap_props.resize(device->GetResourceHeapProperties(nullptr));
        device->GetResourceHeapProperties(heap_props.data());
    }

    auto FindMappableHeap = [&]() {
        for (auto& i : heap_props) {
            if (i.flags & b::RESOURCE_HEAP_PROPERTY_FLAG_HOST_WRITABLE &&
                i.flags & b::RESOURCE_HEAP_PROPERTY_FLAG_HOST_COHERENT &&
                !(i.flags & b::RESOURCE_HEAP_PROPERTY_FLAG_ACCESS_GENERIC_MEMORY_READ_FIXED))
                return &i;
        }
        return (b::RESOURCE_HEAP_PROPERTIES*)nullptr;
    };
    auto FindDeviceLocalHeap = [&]() {
        for (auto& i : heap_props) {
            if (i.flags & b::RESOURCE_HEAP_PROPERTY_FLAG_DEVICE_LOCAL)
                return &i;
        }
        return (b::RESOURCE_HEAP_PROPERTIES*)nullptr;
    };

    // 頂点、インデックスバッファ用リソースヒープを作成
    Ptr<b::IResourceHeap> resource_heap{};
    {
        auto heap_prop = FindDeviceLocalHeap();
        assert(heap_prop);
        assert(heap_alloc_info.heap_type_bits & (1 << heap_prop->heap_index));

        b::RESOURCE_HEAP_DESC heap_desc{};
        heap_desc.heap_index         = heap_prop->heap_index;
        heap_desc.size_in_bytes      = heap_alloc_info.total_size_in_bytes;
        heap_desc.alignment          = heap_alloc_info.required_alignment;
        heap_desc.flags              = b::RESOURCE_HEAP_FLAG_NONE;
        heap_desc.creation_node_mask = b::B3D_DEFAULT_NODE_MASK;
        heap_desc.visible_node_mask  = b::B3D_DEFAULT_NODE_MASK;

        bmr = device->CreateResourceHeap(heap_desc, &resource_heap);
        assert(bmr == b::BMRESULT_SUCCEED);
        resource_heap->SetName("Device local heap");
    }

    // 頂点、インデックスバッファをバインド
    {
        b::BIND_RESOURCE_HEAP_INFO info{};
        info.src_heap            = resource_heap.Get();
        info.num_bind_node_masks = 0;
        info.bind_node_masks     = nullptr;

        // 頂点バッファ
        info.src_heap_offset = alloc_infos[0].heap_offset;
        info.dst_resource    = vertex_buffer.Get();
        bmr = device->BindResourceHeaps(1, &info);
        assert(bmr == b::BMRESULT_SUCCEED);

        // インデックスバッファ
        info.src_heap_offset = alloc_infos[1].heap_offset;
        info.dst_resource    = index_buffer.Get();
        bmr = device->BindResourceHeaps(1, &info);
        assert(bmr == b::BMRESULT_SUCCEED);
    }

    // コピー用ヒープと、コピー用頂点、インデックスバッファを作成
    Ptr<b::IBuffer> vertex_buffer_src{};
    Ptr<b::IBuffer> index_buffer_src {};
    {
        auto heap_prop = FindMappableHeap();
        assert(heap_prop);

        b::COMMITTED_RESOURCE_DESC comitted_desc{};
        comitted_desc.heap_index                    = heap_prop->heap_index;
        comitted_desc.heap_flags                    = b::RESOURCE_HEAP_FLAG_NONE;
        comitted_desc.creation_node_mask            = b::B3D_DEFAULT_NODE_MASK;
        comitted_desc.visible_node_mask             = b::B3D_DEFAULT_NODE_MASK;   
        comitted_desc.resource_desc                 = {};
        comitted_desc.resource_desc.dimension       = b::RESOURCE_DIMENSION_BUFFER;
        comitted_desc.resource_desc.flags           = b::RESOURCE_FLAG_NONE;
        comitted_desc.resource_desc.buffer.flags    = b::BUFFER_CREATE_FLAG_NONE;
        comitted_desc.num_bind_node_masks           = 0;
        comitted_desc.bind_node_masks               = nullptr;

        // コピー用頂点バッファリソースを作成
        {
            auto&& vertex_buffer_desc = comitted_desc.resource_desc;
            vertex_buffer_desc.buffer.usage         = b::BUFFER_USAGE_FLAG_COPY_SRC | b::BUFFER_USAGE_FLAG_COPY_DST;
            vertex_buffer_desc.buffer.size_in_bytes = sizeof(VERTEX) * triangle.size();

            bmr = device->CreateCommittedResource(comitted_desc, &vertex_buffer_src);
            assert(bmr == b::BMRESULT_SUCCEED);
            vertex_buffer_src->SetName("Vertex buffer for copy");
            vertex_buffer_src->GetHeap()->SetName("Vertex buffer heap for copy");

            // データを書き込む
            {
                Mapper map(vertex_buffer_src->GetHeap());
                memcpy_s(map.Get<VERTEX>(), vertex_buffer_desc.buffer.size_in_bytes, triangle.data(), vertex_buffer_desc.buffer.size_in_bytes);
            }
        }

        // コピー用インデックスバッファリソースを作成
        {
            auto&& index_buffer_desc = comitted_desc.resource_desc;
            index_buffer_desc.buffer.usage         = b::BUFFER_USAGE_FLAG_COPY_SRC | b::BUFFER_USAGE_FLAG_COPY_DST;
            index_buffer_desc.buffer.size_in_bytes = sizeof(uint16_t) * index.size();

            bmr = device->CreateCommittedResource(comitted_desc, &index_buffer_src);
            assert(bmr == b::BMRESULT_SUCCEED);
            index_buffer_src->SetName("Index buffer for copy");
            index_buffer_src->GetHeap()->SetName("Index buffer heap for copy");

            // データを書き込む
            {
                Mapper map(index_buffer_src->GetHeap());
                memcpy_s(map.Get<uint16_t>(), index_buffer_desc.buffer.size_in_bytes, index.data(), sizeof(uint16_t) * index.size());
            }
        }

        // 頂点、インデックスバッファデータをデバイスローカルバッファへコピー
        {
            auto&& l = cmd_lists[0];
            b::COMMAND_LIST_BEGIN_DESC begin{};
            begin.flags = b::COMMAND_LIST_BEGIN_FLAG_NONE;
            begin.inheritance_desc = nullptr;
            bmr = l->BeginRecord(begin);
            assert(bmr == b::BMRESULT_SUCCEED);

            // コピー宛先、ソースそれぞれにバリアを張る
            b::CMD_PIPELINE_BARRIER barreir{};
            b::BUFFER_BARRIER_DESC buffer_barreirs[4]{};
            {
                buffer_barreirs[0].src_queue_type = b::COMMAND_TYPE_DIRECT;
                buffer_barreirs[0].dst_queue_type = b::COMMAND_TYPE_DIRECT;
                buffer_barreirs[0].barrier_flags  = b::RESOURCE_BARRIER_FLAG_NONE;

                buffer_barreirs[0].buffer         = vertex_buffer.Get();
                buffer_barreirs[0].src_state      = b::RESOURCE_STATE_UNDEFINED;
                buffer_barreirs[0].dst_state      = b::RESOURCE_STATE_COPY_DST_WRITE;

                buffer_barreirs[1] = buffer_barreirs[0];
                buffer_barreirs[1].buffer         = vertex_buffer_src.Get();
                buffer_barreirs[1].src_state      = b::RESOURCE_STATE_HOST_READ_WRITE;
                buffer_barreirs[1].dst_state      = b::RESOURCE_STATE_COPY_SRC_READ;

                buffer_barreirs[2] = buffer_barreirs[0];
                buffer_barreirs[3] = buffer_barreirs[1];
                buffer_barreirs[2].buffer         = index_buffer.Get();
                buffer_barreirs[3].buffer         = index_buffer_src.Get();

                barreir.src_stages           = b::PIPELINE_STAGE_FLAG_HOST;
                barreir.dst_stages           = b::PIPELINE_STAGE_FLAG_COPY_RESOLVE;
                barreir.dependency_flags     = b::DEPENDENCY_FLAG_NONE;
                barreir.num_buffer_barriers  = _countof(buffer_barreirs);
                barreir.buffer_barriers      = buffer_barreirs;
                barreir.num_texture_barriers = 0;
                barreir.texture_barriers     = nullptr;

                l->PipelineBarrier(barreir);
            }

            // バッファをコピー
            b::CMD_COPY_BUFFER_REGION copy_buffer{};
            b::BUFFER_COPY_REGION copy_region{};
            {
                copy_buffer.num_regions = 1;
                copy_buffer.regions = &copy_region;

                copy_region.src_offset      = 0;
                copy_region.dst_offset      = 0;
                copy_region.size_in_bytes   = vertex_buffer->GetDesc().buffer.size_in_bytes;
                copy_buffer.src_buffer      = vertex_buffer_src.Get();
                copy_buffer.dst_buffer      = vertex_buffer.Get();
                l->CopyBufferRegion(copy_buffer);

                copy_region.src_offset      = 0;
                copy_region.dst_offset      = 0;
                copy_region.size_in_bytes   = index_buffer->GetDesc().buffer.size_in_bytes;
                copy_buffer.src_buffer      = index_buffer_src.Get();
                copy_buffer.dst_buffer      = index_buffer.Get();
                l->CopyBufferRegion(copy_buffer);
            }

            // 頂点バッファ、インデックスバッファとして使用するバリアを用意
            {
                buffer_barreirs[0].buffer         = vertex_buffer.Get();
                buffer_barreirs[0].src_state      = b::RESOURCE_STATE_COPY_DST_WRITE;
                buffer_barreirs[0].dst_state      = b::RESOURCE_STATE_UNDEFINED;

                buffer_barreirs[1] = buffer_barreirs[0];
                buffer_barreirs[1].buffer         = index_buffer.Get();
                buffer_barreirs[1].src_state      = b::RESOURCE_STATE_COPY_DST_WRITE;
                buffer_barreirs[1].dst_state      = b::RESOURCE_STATE_UNDEFINED;

                barreir.src_stages           = b::PIPELINE_STAGE_FLAG_COPY_RESOLVE;
                barreir.dst_stages           = b::PIPELINE_STAGE_FLAG_BOTTOM_OF_PIPE;
                barreir.dependency_flags     = b::DEPENDENCY_FLAG_NONE;
                barreir.num_buffer_barriers  = 2;
                barreir.buffer_barriers      = buffer_barreirs;
                barreir.num_texture_barriers = 0;
                barreir.texture_barriers     = nullptr;

                l->PipelineBarrier(barreir);
            }

            bmr = l->EndRecord();
            assert(bmr == b::BMRESULT_SUCCEED);

            // キューへ送信
            {
                b::SUBMIT_INFO submit_info{};
                submit_info.wait_fence.num_fences        = 0;
                submit_info.wait_fence.fences            = nullptr;
                submit_info.wait_fence.fence_values      = nullptr;
                submit_info.num_command_lists_to_execute = 1;
                submit_info.command_lists_to_execute     = l.GetAddressOf();
                submit_info.signal_fence.num_fences      = 0;
                submit_info.signal_fence.fences          = nullptr;
                submit_info.signal_fence.fence_values    = nullptr;

                b::SUBMIT_DESC submit{};
                submit.num_submit_infos    = 1;
                submit.submit_infos        = &submit_info;
                submit.signal_fence_to_cpu = util_fence.Get();
                bmr = command_queue->Submit(submit);
                assert(bmr == b::BMRESULT_SUCCEED);

                // 待機
                auto bmr = util_fence->Wait(0, 10);
                assert(bmr == b::BMRESULT_SUCCEED);

                // GPU_TO_CPUフェンスをリセット
                bmr = util_fence->Reset();
                assert(bmr == b::BMRESULT_SUCCEED);
            }
        }

    }

    // 頂点バッファビューを作成
    Ptr<b::IVertexBufferView> vertex_buffer_view{};
    {
        b::VERTEX_BUFFER_VIEW_DESC vbvdesc{};

        uint64_t buffer_offset      = 0;
        uint32_t sizes_in_bytes     = vertex_buffer->GetDesc().buffer.size_in_bytes;
        uint32_t strides_in_bytes   = sizeof(VERTEX);
        vbvdesc.num_input_slots     = 1;
        vbvdesc.buffer_offsets      = &buffer_offset;
        vbvdesc.sizes_in_bytes      = &sizes_in_bytes;
        vbvdesc.strides_in_bytes    = &strides_in_bytes;

        bmr = device->CreateVertexBufferView(vertex_buffer.Get(), vbvdesc, &vertex_buffer_view);
        assert(bmr == b::BMRESULT_SUCCEED);
    }

    // インデックスバッファビューを作成
    Ptr<b::IIndexBufferView> index_buffer_view{};
    {
        b::INDEX_BUFFER_VIEW_DESC ibvdesc{};
        ibvdesc.buffer_offset   = 0;
        ibvdesc.size_in_bytes   = index_buffer->GetDesc().buffer.size_in_bytes;
        ibvdesc.index_type      = b::INDEX_TYPE_UINT16;

        bmr = device->CreateIndexBufferView(index_buffer.Get(), ibvdesc, &index_buffer_view);
        assert(bmr == b::BMRESULT_SUCCEED);
    }

    // 描画コマンドを記録
    auto PrepareFrame = [&](uint32_t buffer_index)
    {
        uint32_t frame_count = buffer_index;
        //cmd_allocator->Reset(b::COMMAND_ALLOCATOR_RESET_FLAG_RELEASE_RESOURCES);
        cmd_allocator[buffer_index]->Reset(b::COMMAND_ALLOCATOR_RESET_FLAG_RELEASE_RESOURCES);
        //for (auto& l : cmd_lists)
        {
            auto&& l = cmd_lists[frame_count];
            b::COMMAND_LIST_BEGIN_DESC begin{};
            begin.flags            = b::COMMAND_LIST_BEGIN_FLAG_NONE;
            begin.inheritance_desc = nullptr;
            bmr = l->BeginRecord(begin);
            assert(bmr == b::BMRESULT_SUCCEED);

            b::CMD_PIPELINE_BARRIER barrier{};
            b::TEXTURE_BARRIER_DESC tb{};
            tb.type           = b::TEXTURE_BARRIER_TYPE_VIEW;
            tb.view           = back_buffer_rtvs[frame_count].Get();
            tb.src_state      = b::RESOURCE_STATE_UNDEFINED;
            tb.dst_state      = b::RESOURCE_STATE_COLOR_ATTACHMENT_WRITE;
            tb.src_queue_type = b::COMMAND_TYPE_DIRECT;
            tb.dst_queue_type = b::COMMAND_TYPE_DIRECT;
            tb.barrier_flags  = b::RESOURCE_BARRIER_FLAG_NONE;
            barrier.num_buffer_barriers  = 0;
            barrier.buffer_barriers      = nullptr;
            barrier.num_texture_barriers = 1;
            barrier.texture_barriers     = &tb;
            barrier.src_stages           = b::PIPELINE_STAGE_FLAG_TOP_OF_PIPE;
            barrier.dst_stages           = b::PIPELINE_STAGE_FLAG_COLOR_ATTACHMENT_OUTPUT;
            l->PipelineBarrier(barrier);

            l->SetPipelineState(pipeline.Get());
            l->SetRootSignature(b::PIPELINE_BIND_POINT_GRAPHICS, signature.Get());

            b::CLEAR_VALUE            clear_val{ b::CLEAR_RENDER_TARGET_VALUE{0.8f,0.32f,0.13f,1.f} };
            b::RENDER_PASS_BEGIN_DESC rpbd{ render_pass.Get(), framebuffers[frame_count].Get(), 1, &clear_val };
            b::SUBPASS_BEGIN_DESC     spbd{ b::SUBPASS_CONTENTS_INLINE };
            l->BeginRenderPass(rpbd, spbd);
            {
                b::VIEWPORT vp{ 0, 0, resolution.width, resolution.height, b::B3D_VIEWPORT_MIN_DEPTH, b::B3D_VIEWPORT_MAX_DEPTH };
                l->SetViewports(1, &vp);

                b::SCISSOR_RECT rect{ {0,0}, resolution };
                l->SetScissorRects(1, &rect);

                l->BindVertexBufferViews({ 0, 1, vertex_buffer_view.GetAddressOf() });
                l->BindIndexBufferView(index_buffer_view.Get());

                //             { index_count_per_instance, instance_count, start_index_location, base_vertex_location, start_instance_location }
                l->DrawIndexed({ (uint32_t)index.size()  , 1             , 0                   , 0                   , 0                       });
            }
            l->EndRenderPass({});

            bmr = l->EndRecord();
            assert(bmr == b::BMRESULT_SUCCEED);

            frame_count++;
        }
    };
    for (size_t i = 0; i < BACK_BUFFER_COUNT; i++)
    {
        PrepareFrame(i);
    }

    // プレゼント
    // キューへ送信
    {
        uint32_t back_buffer_index = 0;

        b::SWAP_CHAIN_ACQUIRE_NEXT_BUFFER_INFO acquire_info{};
        {
            // バックバッファのプレゼント完了時にシグナルされます。
            acquire_info.signal_fence        = swapchain_fences[SCF::PRESENT_COMPLETE].Get();
            acquire_info.signal_fence_to_cpu = util_fence.Get();
            acquire_info.timeout_millisec    = UINT32_MAX;
        }

        b::SUBMIT_INFO submit_info{};
        b::IFence*     submit_waits[]       = { cmd_fences[back_buffer_index].Get(), swapchain_fences[SCF::PRESENT_COMPLETE].Get() };
        uint64_t       submit_wait_vals[]   = { 0, 0 };
        b::IFence*     submit_signals[]     = { cmd_fences[back_buffer_index].Get(), swapchain_fences[SCF::RENDER_COMPLETE].Get() };
        uint64_t       submit_signal_vals[] = { 0, 0 };
        {
            // コマンドリストの実行はバックバッファの取得に依存します。
            submit_info.wait_fence.num_fences        = 2;
            submit_info.wait_fence.fences            = submit_waits;
            submit_info.wait_fence.fence_values      = submit_wait_vals;

            submit_info.num_command_lists_to_execute = 1;
            submit_info.command_lists_to_execute     = cmd_lists[back_buffer_index].GetAddressOf();

            // プレゼント実行用フェンス、コマンド用フェンスをシグナル
            submit_info.signal_fence.num_fences      = 2;
            submit_info.signal_fence.fences          = submit_signals;
            submit_info.signal_fence.fence_values    = submit_signal_vals;
        }

        b::SUBMIT_DESC submit{};
        {
            submit.num_submit_infos    = 1;
            submit.signal_fence_to_cpu = nullptr;
            submit.submit_infos        = &submit_info;
        }

        b::SWAP_CHAIN_PRESENT_INFO   present_info{};
        b::SCISSOR_RECT              present_region = { { 0, 0 }, resolution };
        {
            // プレゼントの実行はプレゼント実行用フェンスのシグナルに依存します。
            present_info.wait_fence          = swapchain_fences[SCF::RENDER_COMPLETE].Get();
            present_info.num_present_regions = 0;
            present_info.present_regions     = &present_region;
        }

        StepTimer timer{};
        // メイン メッセージ ループ:
        auto swapchain_fences_data = swapchain_fences.data();
        auto cmd_lists_data        = cmd_lists       .data();
        auto cmd_fences_data       = cmd_fences      .data();
        while (msg.message != WM_QUIT)
        {
            if (timer.Is1SecElapsed())
                SetWindowTextA(hWnd, std::string("FPS: " + std::to_string(timer.GetFramesPerSecond())).c_str());
            timer.Tick();

            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            // 次のバックバッファを取得
            {
                uint32_t next_buffer_index = 0;
                bmr = swapchain->AcquireNextBuffer(acquire_info, &next_buffer_index);
                if (bmr >= b::BMRESULT_FAILED)
                    break;
                assert(bmr == b::BMRESULT_SUCCEED || bmr == b::BMRESULT_SUCCEED_NOT_READY);

                back_buffer_index = next_buffer_index;
            }

            // コマンドリストとフェンスを送信
            {
                cmd_fences_data[back_buffer_index]->Wait(fence_values[back_buffer_index].wait, UINT32_MAX);
                //PrepareFrame(back_buffer_index);

                submit_wait_vals[0]                  = fence_values    [back_buffer_index].wait;
                submit_waits[0]                      = cmd_fences_data [back_buffer_index].Get();
                submit_info.command_lists_to_execute = cmd_lists_data  [back_buffer_index].GetAddressOf();
                submit_signal_vals[0]                = fence_values    [back_buffer_index].signal;
                submit_signals[0]                    = cmd_fences_data [back_buffer_index].Get();
                bmr = command_queue->Submit(submit);
                //assert(bmr == b::BMRESULT_SUCCEED);
            }

            // バックバッファをプレゼント
            {
                util_fence->Wait(0, UINT32_MAX);
                util_fence->Reset();
                bmr = swapchain->Present(present_info);
                //assert(bmr == b::BMRESULT_SUCCEED);
            }

            fence_values[back_buffer_index]++;
        }

        // GPUを待機
        bmr = device->WaitIdle();
        assert(bmr == b::BMRESULT_SUCCEED);

    }

    // オブジェクトの解放
    cmd_lists = {};
    cmd_allocator = {};
    vertex_buffer_view.Reset();
    index_buffer_view.Reset();
    vertex_buffer.Reset();
    index_buffer.Reset();
    resource_heap.Reset();
    pipeline.Reset();
    shader_modules = {};
    framebuffers = {};
    render_pass.Reset();
    signature.Reset();
    back_buffer_rtvs = {};
    back_buffers = {};
    swapchain.Reset();
    command_queue.Reset();
    swapchain_fences = {};
    cmd_fences = {};
}

void HelloTriangle::Tick()
{

}

void HelloTriangle::Update()
{

}

void HelloTriangle::Render()
{

}

void HelloTriangle::Term()
{

}


}// namespace buma
