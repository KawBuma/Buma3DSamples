#include "pch.h"
#include "HelloTriangle.h"

#include <cassert>

#define BMR_RET_IF_FAILED(x) if (x >= buma3d::BMRESULT_FAILED) { assert(false && #x); return false; }
#define RET_IF_FAILED(x) if (!(x)) { assert(false && #x); return false; }

std::vector<float> g_fpss;
bool g_first = true;

namespace init = buma3d::hlp::init;

namespace buma
{

const buma3d::RESOURCE_HEAP_PROPERTIES* FindMappableHeap(const std::vector<buma3d::RESOURCE_HEAP_PROPERTIES>& _heap_props)
{
    for (auto& i : _heap_props) {
        if (i.flags & buma3d::RESOURCE_HEAP_PROPERTY_FLAG_HOST_WRITABLE &&
            i.flags & buma3d::RESOURCE_HEAP_PROPERTY_FLAG_HOST_COHERENT &&
            !(i.flags & buma3d::RESOURCE_HEAP_PROPERTY_FLAG_ACCESS_GENERIC_MEMORY_READ_FIXED))
            return &i;
    }
    return nullptr;
}

const buma3d::RESOURCE_HEAP_PROPERTIES* FindDeviceLocalHeap(const std::vector<buma3d::RESOURCE_HEAP_PROPERTIES>& _heap_props)
{
    for (auto& i : _heap_props) {
        if (i.flags & buma3d::RESOURCE_HEAP_PROPERTY_FLAG_DEVICE_LOCAL)
            return &i;
    }
    return nullptr;
}

namespace b = buma3d;

template<typename T>
using Ptr = buma3d::util::Ptr<T>;


class HelloTriangle::ResizeEvent : public IEvent
{
public:
    ResizeEvent(HelloTriangle& _owner) : owner{ _owner } {}
    virtual ~ResizeEvent() {}
    void Execute(IEventArgs* _args) override { owner.OnResize(static_cast<ResizeEventArgs*>(_args)); }
    static std::shared_ptr<ResizeEvent> Create(HelloTriangle& _owner) { return std::make_shared<ResizeEvent>(_owner); }
private:
    HelloTriangle& owner;
};

class HelloTriangle::BufferResizedEvent : public IEvent
{
public:
    BufferResizedEvent(HelloTriangle& _owner) : owner{ _owner } {}
    virtual ~BufferResizedEvent() {}
    void Execute(IEventArgs* _args) override { owner.OnResized(static_cast<BufferResizedEventArgs*>(_args)); }
    static std::shared_ptr<BufferResizedEvent> Create(HelloTriangle& _owner) { return std::make_shared<BufferResizedEvent>(_owner); }
private:
    HelloTriangle& owner;
};


HelloTriangle::HelloTriangle()
    : ApplicationBase       ()
    , platform              {}
    , spwindow              {}
    , window                {}
    , device                {}
    , command_queue         {}
    , timer                 {}
    , swapchain             {}
    , back_buffers          {}
    , back_buffer_index     {}
    , swapchain_fences      {}
    , vpiewport             {}
    , scissor_rect          {}
    , framebuffers          {}
    , shader_modules        {}
    , pipeline              {}
    , cmd_allocator         {}
    , cmd_lists             {}
    , util_fence            {}
    , fence_values          {}
    , cmd_fences            {}
    , render_complete_fence {}
    , heap_props            {}
    , signature             {}
    , render_pass           {}
    , resource_heap         {}
    , vertex_buffer         {}
    , index_buffer          {}
    , vertex_buffer_src     {}
    , index_buffer_src      {}
    , vertex_buffer_view    {}
    , index_buffer_view     {}
    , signal_fence_desc     {}
    , wait_fence_desc       {}
    , submit_info           {}
    , submit                {}
    , present_info          {}
    , present_region        {}
    , on_resize             {}
    , on_resized            {}
{    
     
}

HelloTriangle::~HelloTriangle()
{
    //Term();
}

HelloTriangle* HelloTriangle::Create()
{
    return new HelloTriangle();
}

bool HelloTriangle::Prepare(PlatformBase& _platform)
{
    platform = &_platform;
    dr       = platform->GetDeviceResources();
    device   = dr->GetDevice();

    spwindow = platform->GetWindow();
    window   = spwindow.get();
    window->SetWindowTitle("Buma3DSamples - HelloTriangle");

    if (!PrepareSwapChain()) return false;
    PrepareSubmitInfo();
    CreateEvents();
    if (!Init()) return false;

    return true;
}

void HelloTriangle::PrepareSubmitInfo()
{
    // キューへの送信情報
    submit_info.num_command_lists_to_execute = 1;
    submit.signal_fence_to_cpu               = nullptr;
    submit.num_submit_infos                  = 1;
    submit.submit_infos                      = &submit_info;
}

void HelloTriangle::CreateEvents()
{
    // イベントを登録
    on_resize = ResizeEvent::Create(*this);
    on_resized = BufferResizedEvent::Create(*this);
    window->AddResizeEvent(on_resize);
    window->AddBufferResizedEvent(on_resized);
}

bool HelloTriangle::PrepareSwapChain()
{
    b::SWAP_CHAIN_DESC scd = init::SwapChainDesc(nullptr, buma3d::COLOR_SPACE_SRGB_NONLINEAR,
                                                 init::SwapChainBufferDesc(1280, 720, BACK_BUFFER_COUNT, { b::RESOURCE_FORMAT_B8G8R8A8_UNORM }, b::SWAP_CHAIN_BUFFER_FLAG_COLOR_ATTACHMENT),
                                                 dr->GetCommandQueues(b::COMMAND_TYPE_DIRECT)[0].GetAddressOf());
    scd.flags = b::SWAP_CHAIN_FLAG_ALLOW_DISCARD_AFTER_PRESENT | b::SWAP_CHAIN_FLAG_DISABLE_VERTICAL_SYNC;
    window->ResizeWindow({ 1280,720 }, scd.flags);
    if (!(window->CreateSwapChain(scd, &swapchain)))
        return false;

    back_buffers = &swapchain->GetBuffers();
    swapchain_fences = &swapchain->GetPresentCompleteFences();

    present_info.num_present_regions = 0;
    present_info.present_regions     = &present_region;
    present_region = { { 0, 0 }, scissor_rect.extent };

    return true;
}

bool HelloTriangle::Init()
{
    auto&& resolution = swapchain->GetSwapChain()->GetDesc().buffer;
    vpiewport       = {   0, 0  ,  (float)resolution.width, (float)resolution.height, b::B3D_VIEWPORT_MIN_DEPTH, b::B3D_VIEWPORT_MAX_DEPTH };
    scissor_rect    = { { 0, 0 },        {resolution.width,        resolution.height} };
    command_queue   = dr->GetCommandQueues(b::COMMAND_TYPE_DIRECT)[0];

    if (!LoadAssets()) return false;

    return command_queue;
}

bool HelloTriangle::LoadAssets()
{
    auto aspect_ratio = window->GetAspectRatio();
    triangle = {
          { {  0.0f ,  0.25f * aspect_ratio, 0.0f, 1.f }, { 1.f, 0.f, 0.f, 1.f} }
        , { {  0.25f, -0.25f * aspect_ratio, 0.0f, 1.f }, { 0.f, 1.f, 0.f, 1.f} }
        , { { -0.25f, -0.25f * aspect_ratio, 0.0f, 1.f }, { 0.f, 0.f, 1.f, 1.f} }
    };
    index = { 0,1,2 };

    if (!CreateRootSignature())     return false;
    if (!CreateRenderPass())        return false;
    if (!CreateFramebuffer())       return false;
    if (!CreateShaderModules())     return false;
    if (!CreateGraphicsPipelines()) return false;
    if (!CreateCommandAllocator())  return false;
    if (!CreateCommandLists())      return false;
    if (!CreateFences())            return false;

    b::RESOURCE_HEAP_ALLOCATION_INFO         heap_alloc_info{};
    std::vector<b::RESOURCE_ALLOCATION_INFO> alloc_infos;
    if (!CreateBuffers())           return false;
    if (!CreateHeaps(&heap_alloc_info, &alloc_infos))       return false;
    if (!BindResourceHeaps(&heap_alloc_info, &alloc_infos)) return false;
    if (!CreateBuffersForCopy())                            return false;
    if (!CopyBuffers())                                     return false;
    if (!CreateBufferViews())                               return false;

    // 描画コマンドを記録
    for (size_t i = 0; i < BACK_BUFFER_COUNT; i++)
        PrepareFrame(i);

    return true;
}

bool HelloTriangle::CreateRootSignature()
{
    // ルートシグネチャの作成
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

    auto bmr = device->CreateRootSignature(rsdesc, &signature);
    assert(bmr == b::BMRESULT_SUCCEED);
    return bmr == b::BMRESULT_SUCCEED;
}
bool HelloTriangle::CreateRenderPass()
{
    b::RENDER_PASS_DESC render_pass_desc{};

    b::ATTACHMENT_DESC attachment{};
    attachment.flags        = b::ATTACHMENT_FLAG_NONE;
    attachment.format       = (*back_buffers)[0].rtv->GetDesc().view.format;
    attachment.sample_count = 1;
    attachment.load_op      = b::ATTACHMENT_LOAD_OP_CLEAR;
    attachment.store_op     = b::ATTACHMENT_STORE_OP_STORE;
    attachment.begin_state  = b::RESOURCE_STATE_COLOR_ATTACHMENT_WRITE;
    attachment.end_state    = b::RESOURCE_STATE_PRESENT;

    b::ATTACHMENT_REFERENCE color_attachment_ref{};
    color_attachment_ref.attachment_index             = 0;
    color_attachment_ref.state_at_pass                = b::RESOURCE_STATE_COLOR_ATTACHMENT_READ_WRITE;
    color_attachment_ref.stencil_state_at_pass        = {};
    color_attachment_ref.input_attachment_aspect_mask = b::TEXTURE_ASPECT_FLAG_COLOR;

    b::SUBPASS_DESC subpass_desc{};
    subpass_desc.flags                          = b::SUBPASS_FLAG_NONE;
    subpass_desc.pipeline_bind_point            = b::PIPELINE_BIND_POINT_GRAPHICS;
    subpass_desc.view_mask                      = 0x0;
    subpass_desc.num_color_attachments          = 1;
    subpass_desc.color_attachments              = &color_attachment_ref;
    subpass_desc.resolve_attachments            = nullptr;
    subpass_desc.depth_stencil_attachment       = nullptr;

    b::SUBPASS_DEPENDENCY dependencies[] = { {},{} };
    dependencies[0].src_subpass                 = b::B3D_SUBPASS_EXTERNAL;
    dependencies[0].dst_subpass                 = 0;
    dependencies[0].src_stage_mask              = b::PIPELINE_STAGE_FLAG_TOP_OF_PIPE;
    dependencies[0].dst_stage_mask              = b::PIPELINE_STAGE_FLAG_COLOR_ATTACHMENT_OUTPUT;
    dependencies[0].src_access                  = b::RESOURCE_ACCESS_FLAG_NONE;
    dependencies[0].dst_access                  = b::RESOURCE_ACCESS_FLAG_COLOR_ATTACHMENT_READ | b::RESOURCE_ACCESS_FLAG_COLOR_ATTACHMENT_WRITE;
    dependencies[0].dependency_flags            = b::DEPENDENCY_FLAG_NONE;
    dependencies[0].view_offset                 = 0;

    dependencies[1].src_subpass                 = 0;
    dependencies[1].dst_subpass                 = b::B3D_SUBPASS_EXTERNAL;
    dependencies[1].src_stage_mask              = b::PIPELINE_STAGE_FLAG_COLOR_ATTACHMENT_OUTPUT;
    dependencies[1].dst_stage_mask              = b::PIPELINE_STAGE_FLAG_BOTTOM_OF_PIPE;
    dependencies[1].src_access                  = b::RESOURCE_ACCESS_FLAG_COLOR_ATTACHMENT_READ | b::RESOURCE_ACCESS_FLAG_COLOR_ATTACHMENT_WRITE;
    dependencies[1].dst_access                  = b::RESOURCE_ACCESS_FLAG_NONE;
    dependencies[1].dependency_flags            = b::DEPENDENCY_FLAG_NONE;
    dependencies[1].view_offset                 = 0;

    render_pass_desc.flags                      = b::RENDER_PASS_FLAG_NONE;
    render_pass_desc.num_attachments            = 1;
    render_pass_desc.attachments                = &attachment;
    render_pass_desc.num_subpasses              = 1;
    render_pass_desc.subpasses                  = &subpass_desc;
    render_pass_desc.num_dependencies           = _countof(dependencies);
    render_pass_desc.dependencies               = dependencies;
    render_pass_desc.num_correlated_view_masks  = 0;
    render_pass_desc.correlated_view_masks      = nullptr;

    auto bmr = device->CreateRenderPass(render_pass_desc, &render_pass);
    assert(bmr == b::BMRESULT_SUCCEED);
    return bmr == b::BMRESULT_SUCCEED;
}
bool HelloTriangle::CreateFramebuffer()
{
    framebuffers.resize(BACK_BUFFER_COUNT);
    b::FRAMEBUFFER_DESC fb_desc{};
    for (uint32_t i = 0; i < BACK_BUFFER_COUNT; i++)
    {
        b::IView* attachment    = (*back_buffers)[i].rtv.Get();
        fb_desc.flags           = b::FRAMEBUFFER_FLAG_NONE;
        fb_desc.render_pass     = render_pass.Get();
        fb_desc.num_attachments = 1;
        fb_desc.attachments     = &attachment;

        auto bmr = device->CreateFramebuffer(fb_desc, &framebuffers[i]);
        BMR_RET_IF_FAILED(bmr);
    }
    return true;
}
bool HelloTriangle::CreateShaderModules()
{
    b::BMRESULT bmr{};
    shader_modules.resize(2);
    shader::LOAD_SHADER_DESC desc{};
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

    auto&& loader = dr->GetShaderLoader();
    // vs
    {
        desc.entry_point    = "main";
        desc.filename       = "./VertexShader.hlsl";
        desc.defines        = {};
        desc.stage          = { shader::SHADER_STAGE_VERTEX };
        std::vector<uint8_t> bytecode;
        loader->LoadShaderFromHLSL(desc, &bytecode);
        assert(!bytecode.empty());

        b::SHADER_MODULE_DESC module_desc{};
        module_desc.flags                    = b::SHADER_MODULE_FLAG_NONE;
        module_desc.bytecode.bytecode_length = bytecode.size();
        module_desc.bytecode.shader_bytecode = bytecode.data();
        bmr = device->CreateShaderModule(module_desc, &shader_modules[0]);
        BMR_RET_IF_FAILED(bmr);
    }

    // ps
    {
        desc.entry_point    = "main";
        desc.filename       = "./PixelShader.hlsl";
        desc.defines        = {};
        desc.stage          = { shader::SHADER_STAGE_PIXEL };
        std::vector<uint8_t> bytecode;
        loader->LoadShaderFromHLSL(desc, &bytecode);
        assert(!bytecode.empty());

        b::SHADER_MODULE_DESC module_desc{};
        module_desc.flags                    = b::SHADER_MODULE_FLAG_NONE;
        module_desc.bytecode.bytecode_length = bytecode.size();
        module_desc.bytecode.shader_bytecode = bytecode.data();
        bmr = device->CreateShaderModule(module_desc, &shader_modules[1]);
        BMR_RET_IF_FAILED(bmr);
    }

    return true;
}
bool HelloTriangle::CreateGraphicsPipelines()
{
    b::BMRESULT bmr{};
    // グラフィックスパイプラインの作成
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
        BMR_RET_IF_FAILED(bmr);
    }

    return true;
}
bool HelloTriangle::CreateCommandAllocator()
{
    cmd_allocator.resize(BACK_BUFFER_COUNT);
    for (auto& i : cmd_allocator)
    {
        b::COMMAND_ALLOCATOR_DESC cad{};
        cad.type    = b::COMMAND_TYPE_DIRECT;
        cad.level   = b::COMMAND_LIST_LEVEL_PRIMARY;
        cad.flags   = b::COMMAND_ALLOCATOR_FLAG_NONE | b::COMMAND_ALLOCATOR_FLAG_TRANSIENT;

        auto bmr = device->CreateCommandAllocator(cad, &i);
        BMR_RET_IF_FAILED(bmr);
    }

    return true;
}
bool HelloTriangle::CreateCommandLists()
{
    cmd_lists.resize(BACK_BUFFER_COUNT);
    b::COMMAND_LIST_DESC cld{};
    cld.type      = b::COMMAND_TYPE_DIRECT;
    cld.level     = b::COMMAND_LIST_LEVEL_PRIMARY;
    cld.node_mask = b::B3D_DEFAULT_NODE_MASK;
    uint32_t cnt = 0;
    for (auto& i : cmd_lists)
    {
        cld.allocator = cmd_allocator[cnt].Get();
        auto bmr = device->AllocateCommandList(cld, &i);
        BMR_RET_IF_FAILED(bmr);
        i->SetName(std::string("CommandList " + std::to_string(cnt++)).c_str());
    }

    return true;
}
bool HelloTriangle::CreateFences()
{
    cmd_fences.resize(BACK_BUFFER_COUNT);
    b::FENCE_DESC fd{};
    fd.flags         = b::FENCE_FLAG_NONE;
    fd.initial_value = 0;

    fd.type = b::FENCE_TYPE_BINARY_GPU_TO_CPU;
    auto bmr = device->CreateFence(fd, &util_fence);
    BMR_RET_IF_FAILED(bmr);
    util_fence->SetName("util_fence");

    fd.type = b::FENCE_TYPE_TIMELINE;
    uint32_t cnt = 0;
    for (auto& i : cmd_fences)
    {
        bmr = device->CreateFence(fd, &i);
        BMR_RET_IF_FAILED(bmr);
        i->SetName(std::string("cmd_fences" + std::to_string(cnt++)).c_str());
    }

    fd.type = b::FENCE_TYPE_BINARY_GPU_TO_GPU;
    bmr = device->CreateFence(fd, &render_complete_fence);
    BMR_RET_IF_FAILED(bmr);
    render_complete_fence->SetName("render_complete_fence");

    return true;
}
bool HelloTriangle::CreateBuffers()
{
    b::BMRESULT bmr{};
    // 頂点バッファリソースの器を作成
    {
        auto vbdesc = init::BufferResourceDesc(sizeof(VERTEX) * triangle.size(), init::BUF_COPYABLE_FLAGS | b::BUFFER_USAGE_FLAG_VERTEX_BUFFER);
        bmr = device->CreatePlacedResource(vbdesc, &vertex_buffer);
        BMR_RET_IF_FAILED(bmr);
        vertex_buffer->SetName("Vertex buffer");
    }

    // インデックスバッファリソースの器を作成
    {
        auto ibdesc = init::BufferResourceDesc(sizeof(decltype(index)::value_type) * index.size(), init::BUF_COPYABLE_FLAGS | b::BUFFER_USAGE_FLAG_INDEX_BUFFER);
        bmr = device->CreatePlacedResource(ibdesc, &index_buffer);
        BMR_RET_IF_FAILED(bmr);
        index_buffer->SetName("Index buffer");
    }

    return true;
}
bool HelloTriangle::CreateHeaps(b::RESOURCE_HEAP_ALLOCATION_INFO* _heap_alloc_info, std::vector<b::RESOURCE_ALLOCATION_INFO>* _alloc_infos)
{
    // バッファのサイズ要件を取得。
    {
        _alloc_infos->resize(2);
        b::IResource* resources[] = { vertex_buffer.Get(), index_buffer.Get() };
        auto bmr = device->GetResourceAllocationInfo(2, resources, _alloc_infos->data(), _heap_alloc_info);
        BMR_RET_IF_FAILED(bmr);
    }

    // ヒーププロパティを取得
    {
        heap_props.resize(device->GetResourceHeapProperties(nullptr));
        device->GetResourceHeapProperties(heap_props.data());
    }

    // 頂点、インデックスバッファ用リソースヒープを作成
    {
        auto heap_prop = FindDeviceLocalHeap(heap_props);
        RET_IF_FAILED(heap_prop);
        RET_IF_FAILED(_heap_alloc_info->heap_type_bits & (1 << heap_prop->heap_index));

        b::RESOURCE_HEAP_DESC heap_desc{};
        heap_desc.heap_index         = heap_prop->heap_index;
        heap_desc.size_in_bytes      = _heap_alloc_info->total_size_in_bytes;
        heap_desc.alignment          = _heap_alloc_info->required_alignment;
        heap_desc.flags              = b::RESOURCE_HEAP_FLAG_NONE;
        heap_desc.creation_node_mask = b::B3D_DEFAULT_NODE_MASK;
        heap_desc.visible_node_mask  = b::B3D_DEFAULT_NODE_MASK;

        auto bmr = device->CreateResourceHeap(heap_desc, &resource_heap);
        BMR_RET_IF_FAILED(bmr);
        resource_heap->SetName("Device local heap");
    }

    return true;
}
bool HelloTriangle::BindResourceHeaps(b::RESOURCE_HEAP_ALLOCATION_INFO* _heap_alloc_info, std::vector<b::RESOURCE_ALLOCATION_INFO>* _alloc_infos)
{
    b::BMRESULT bmr{};
    // 頂点、インデックスバッファをバインド
    b::BIND_RESOURCE_HEAP_INFO info{};
    info.src_heap            = resource_heap.Get();
    info.num_bind_node_masks = 0;
    info.bind_node_masks     = nullptr;

    // 頂点バッファ
    info.src_heap_offset = (*_alloc_infos)[0].heap_offset;
    info.dst_resource    = vertex_buffer.Get();
    bmr = device->BindResourceHeaps(1, &info);
    BMR_RET_IF_FAILED(bmr);

    // インデックスバッファ
    info.src_heap_offset = (*_alloc_infos)[1].heap_offset;
    info.dst_resource    = index_buffer.Get();
    bmr = device->BindResourceHeaps(1, &info);
    BMR_RET_IF_FAILED(bmr);

    return true;
}
bool HelloTriangle::CreateBuffersForCopy()
{
    // コピー用頂点、インデックスバッファを作成
    auto heap_prop = FindMappableHeap(heap_props);
    RET_IF_FAILED(heap_prop);

    b::COMMITTED_RESOURCE_DESC comitted_desc = init::CommittedResourceDesc(heap_prop->heap_index, b::RESOURCE_HEAP_FLAG_NONE, {});
    comitted_desc.resource_desc.dimension       = b::RESOURCE_DIMENSION_BUFFER;
    comitted_desc.resource_desc.flags           = b::RESOURCE_FLAG_NONE;
    comitted_desc.resource_desc.buffer.flags    = b::BUFFER_CREATE_FLAG_NONE;

    // コピー用頂点バッファリソースを作成
    {
        auto&& vertex_buffer_desc = comitted_desc.resource_desc;
        vertex_buffer_desc.buffer.usage         = b::BUFFER_USAGE_FLAG_COPY_SRC | b::BUFFER_USAGE_FLAG_COPY_DST;
        vertex_buffer_desc.buffer.size_in_bytes = sizeof(VERTEX) * triangle.size();

        auto bmr = device->CreateCommittedResource(comitted_desc, &vertex_buffer_src);
        BMR_RET_IF_FAILED(bmr);
        vertex_buffer_src->SetName("Vertex buffer for copy");
        vertex_buffer_src->GetHeap()->SetName("Vertex buffer heap for copy");

        // データを書き込む
        {
            util::Mapper map(vertex_buffer_src->GetHeap());
            memcpy_s(map.As<VERTEX>().GetData(), vertex_buffer_desc.buffer.size_in_bytes, triangle.data(), vertex_buffer_desc.buffer.size_in_bytes);
        }
    }

    // コピー用インデックスバッファリソースを作成
    {
        auto&& index_buffer_desc = comitted_desc.resource_desc;
        index_buffer_desc.buffer.usage         = b::BUFFER_USAGE_FLAG_COPY_SRC | b::BUFFER_USAGE_FLAG_COPY_DST;
        index_buffer_desc.buffer.size_in_bytes = sizeof(uint16_t) * index.size();

        auto bmr = device->CreateCommittedResource(comitted_desc, &index_buffer_src);
        BMR_RET_IF_FAILED(bmr);
        index_buffer_src->SetName("Index buffer for copy");
        index_buffer_src->GetHeap()->SetName("Index buffer heap for copy");

        // データを書き込む
        {
            util::Mapper map(index_buffer_src->GetHeap());
            memcpy_s(map.As<uint16_t>().GetData(), index_buffer_desc.buffer.size_in_bytes, index.data(), sizeof(uint16_t) * index.size());
        }
    }

    return true;
}
bool HelloTriangle::CopyBuffers()
{
    // 頂点、インデックスバッファデータをデバイスローカルバッファへコピー
    b::BMRESULT bmr{};
    auto&& l = cmd_lists[0];

    b::COMMAND_LIST_BEGIN_DESC begin{};
    begin.flags = b::COMMAND_LIST_BEGIN_FLAG_NONE;
    begin.inheritance_desc = nullptr;

    // 記録を開始
    bmr = l->BeginRecord(begin);
    BMR_RET_IF_FAILED(bmr);

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
        buffer_barreirs[0].buffer       = vertex_buffer.Get();
        buffer_barreirs[0].src_state    = b::RESOURCE_STATE_COPY_DST_WRITE;
        buffer_barreirs[0].dst_state    = b::RESOURCE_STATE_UNDEFINED;

        buffer_barreirs[1] = buffer_barreirs[0];
        buffer_barreirs[1].buffer       = index_buffer.Get();
        buffer_barreirs[1].src_state    = b::RESOURCE_STATE_COPY_DST_WRITE;
        buffer_barreirs[1].dst_state    = b::RESOURCE_STATE_UNDEFINED;

        barreir.src_stages              = b::PIPELINE_STAGE_FLAG_COPY_RESOLVE;
        barreir.dst_stages              = b::PIPELINE_STAGE_FLAG_BOTTOM_OF_PIPE;
        barreir.dependency_flags        = b::DEPENDENCY_FLAG_NONE;
        barreir.num_buffer_barriers     = 2;
        barreir.buffer_barriers         = buffer_barreirs;
        barreir.num_texture_barriers    = 0;
        barreir.texture_barriers        = nullptr;

        l->PipelineBarrier(barreir);
    }

    // 記録を終了
    bmr = l->EndRecord();
    BMR_RET_IF_FAILED(bmr);

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
        BMR_RET_IF_FAILED(bmr);
    }

    // 待機
    bmr = util_fence->Wait(0, UINT32_MAX);
    BMR_RET_IF_FAILED(bmr);

    // GPU_TO_CPUフェンスをリセット
    bmr = util_fence->Reset();
    BMR_RET_IF_FAILED(bmr);

    return true;
}
bool HelloTriangle::CreateBufferViews()
{
    b::BMRESULT bmr{};
    // 頂点バッファビューを作成
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
        BMR_RET_IF_FAILED(bmr);
    }

    // インデックスバッファビューを作成
    {
        b::INDEX_BUFFER_VIEW_DESC ibvdesc{};
        ibvdesc.buffer_offset   = 0;
        ibvdesc.size_in_bytes   = index_buffer->GetDesc().buffer.size_in_bytes;
        ibvdesc.index_type      = b::INDEX_TYPE_UINT16;

        bmr = device->CreateIndexBufferView(index_buffer.Get(), ibvdesc, &index_buffer_view);
        BMR_RET_IF_FAILED(bmr);
    }

    return true;
}

void HelloTriangle::PrepareFrame(uint32_t _buffer_index)
{
    auto reset_flags = b::COMMAND_ALLOCATOR_RESET_FLAG_RELEASE_RESOURCES;
    cmd_allocator[_buffer_index]->Reset(reset_flags);

    auto&& l = cmd_lists[_buffer_index];
    b::COMMAND_LIST_BEGIN_DESC begin{};
    begin.flags            = b::COMMAND_LIST_BEGIN_FLAG_NONE;
    begin.inheritance_desc = nullptr;

    auto bmr = l->BeginRecord(begin);
    assert(bmr == b::BMRESULT_SUCCEED);
    {
        b::CMD_PIPELINE_BARRIER barrier{};
        b::TEXTURE_BARRIER_DESC tb{};
        tb.type           = b::TEXTURE_BARRIER_TYPE_VIEW;
        tb.view           = (*back_buffers)[_buffer_index].rtv.Get();
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

        static float sc = 0.f;
        static float sx = 0.f;
        sc = sc + 0.34f * timer.GetElapsedSecondsF();
        sx = fabsf(sinf(sc));
        b::CLEAR_VALUE            clear_val{ b::CLEAR_RENDER_TARGET_VALUE{0.9f * sx ,0.28f,0.13f,1.f} };
        b::RENDER_PASS_BEGIN_DESC rpbd{ render_pass.Get(), framebuffers[_buffer_index].Get(), 1, &clear_val };
        b::SUBPASS_BEGIN_DESC     spbd{ b::SUBPASS_CONTENTS_INLINE };
        l->BeginRenderPass(rpbd, spbd);
        {
            l->SetViewports(1, &vpiewport);
            l->SetScissorRects(1, &scissor_rect);

            l->BindVertexBufferViews({ 0, 1, vertex_buffer_view.GetAddressOf() });
            l->BindIndexBufferView(index_buffer_view.Get());

            //             { index_count_per_instance, instance_count, start_index_location, base_vertex_location, start_instance_location }
            l->DrawIndexed({ 3                       , 1             , 0                   , 0                   , 0                       });
        }
        l->EndRenderPass({});
    }
    bmr = l->EndRecord();
    assert(bmr == b::BMRESULT_SUCCEED);
}

void HelloTriangle::Tick()
{
    timer.Tick();
    if (timer.IsOneSecElapsed())
    {
        if (!g_first)
            g_fpss.emplace_back(timer.GetFramesPerSecond());
        g_first = false;

        platform->GetLogger()->LogInfo(("fps: " + std::to_string(timer.GetFramesPerSecond())).c_str());
    }

    Update();
    Render();
}

void HelloTriangle::Update()
{
    if (window->GetWindowStateFlags() & WINDOW_STATE_FLAG_MINIMIZED)
        return;
    // 次のバックバッファを取得
    MoveToNextFrame();
}

void HelloTriangle::MoveToNextFrame()
{
    uint32_t next_buffer_index = 0;
    auto bmr = swapchain->AcquireNextBuffer(UINT32_MAX, &next_buffer_index);
    assert(bmr == b::BMRESULT_SUCCEED || bmr == b::BMRESULT_SUCCEED_NOT_READY);

    back_buffer_index = next_buffer_index;
}

void HelloTriangle::Render()
{
    if (window->GetWindowStateFlags() & WINDOW_STATE_FLAG_MINIMIZED)
        return;

    auto cmd_lists_data  = cmd_lists.data();
    auto cmd_fences_data = cmd_fences.data();
    b::BMRESULT bmr{};

    //if (timer.IsOneSecElapsed())
    //    SetWindowTextA(hwnd, std::string("FPS: " + std::to_string(timer.GetFramesPerSecond())).c_str());

    // コマンドリストとフェンスを送信
    {
        cmd_fences_data[back_buffer_index]->Wait(fence_values[back_buffer_index].wait, UINT32_MAX);
        PrepareFrame(back_buffer_index);

        // 待機フェンス
        wait_fence_desc.Reset();
        wait_fence_desc.AddFence(cmd_fences_data[back_buffer_index].Get(), fence_values[back_buffer_index].wait);
        wait_fence_desc.AddFence(swapchain_fences->signal_fence.Get(), 0);
        submit_info.wait_fence = wait_fence_desc.GetAsWait().wait_fence;

        // コマンドリスト
        submit_info.command_lists_to_execute = cmd_lists_data[back_buffer_index].GetAddressOf();

        // シグナルフェンス
        signal_fence_desc.Reset();
        signal_fence_desc.AddFence(cmd_fences_data[back_buffer_index].Get(), fence_values[back_buffer_index].signal);
        signal_fence_desc.AddFence(render_complete_fence.Get(), 0);
        submit_info.signal_fence = signal_fence_desc.GetAsSignal().signal_fence;

        bmr = command_queue->Submit(submit);
        assert(bmr == b::BMRESULT_SUCCEED);
    }

    // バックバッファをプレゼント
    {
        swapchain_fences->signal_fence_to_cpu->Wait(0, UINT32_MAX);
        swapchain_fences->signal_fence_to_cpu->Reset();

        present_info.wait_fence = render_complete_fence.Get();
        bmr = swapchain->Present(present_info);
        assert(bmr == b::BMRESULT_SUCCEED);
    }

    //platform->GetLogger()->LogInfo("framed");
    fence_values[back_buffer_index]++;
}

void HelloTriangle::OnResize(ResizeEventArgs* _args)
{
    command_queue->WaitIdle();
    framebuffers = {};
    back_buffers = {};
}

void HelloTriangle::OnResized(BufferResizedEventArgs* _args)
{
    back_buffers     = &swapchain->GetBuffers();
    swapchain_fences = &swapchain->GetPresentCompleteFences();

    CreateFramebuffer();
    for (size_t i = 0; i < BACK_BUFFER_COUNT; i++)
        PrepareFrame(i);

    Update();
    Render();
}

void HelloTriangle::Term()
{
    dr->WaitForGpu();

    // result
    {
        float res = 0.f;
        float size = static_cast<float>(g_fpss.size());
        for (auto& i : g_fpss)
            res += i;
        std::stringstream ss;
        ss << "\nprof result: average fps";
        ss << (res / size) << std::endl;

        //ss << 100.f * ((res / size) / 5000.f);
        //ss << "% vs. 5000fps" << std::endl;

        //ss << 100.f * ((res / size) / 6000.f);
        //ss << "% vs. 6000fps" << std::endl;

        //ss << 100.f * ((res / size) / 7000.f);
        //ss << "% vs. 7000fps" << std::endl << std::endl;

        platform->GetLogger()->LogInfo(ss.str().c_str());
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
    back_buffers = nullptr;
    swapchain.reset();
    command_queue.Reset();
    swapchain_fences = {};
    cmd_fences = {};
}


}// namespace buma
