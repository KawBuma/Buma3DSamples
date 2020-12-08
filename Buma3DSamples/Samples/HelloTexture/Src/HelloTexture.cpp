#include "pch.h"
#include "HelloTexture.h"
#include "External/Camera.h"

#include <cassert>

#define BMR_RET_IF_FAILED(x) if (x >= buma3d::BMRESULT_FAILED) { assert(false && #x); return false; }
#define RET_IF_FAILED(x) if (!(x)) { assert(false && #x); return false; }

std::vector<float>* g_fpss = nullptr;
bool g_first = true;
constexpr bool USE_HOST_WRITABLE_HEAP = true;

Camera g_cam{};

namespace init = buma3d::hlp::init;

namespace buma
{

namespace tex
{
buma3d::RESOURCE_FORMAT GetDefaultFormat(const tex::TEXTURE_DESC& _tex_desc)
{
    static const buma3d::RESOURCE_FORMAT FORMAT_TABLE[3][4] = {
        { buma3d::RESOURCE_FORMAT_R8_SNORM  , buma3d::RESOURCE_FORMAT_R8G8_SNORM    , buma3d::RESOURCE_FORMAT_R8G8B8A8_SNORM     , buma3d::RESOURCE_FORMAT_R8G8B8A8_SNORM },
        { buma3d::RESOURCE_FORMAT_R8_UNORM  , buma3d::RESOURCE_FORMAT_R8G8_UNORM    , buma3d::RESOURCE_FORMAT_R8G8B8A8_UNORM     , buma3d::RESOURCE_FORMAT_R8G8B8A8_UNORM },
        { buma3d::RESOURCE_FORMAT_R16_FLOAT , buma3d::RESOURCE_FORMAT_R16G16_FLOAT  , buma3d::RESOURCE_FORMAT_R16G16B16A16_FLOAT , buma3d::RESOURCE_FORMAT_R16G16B16A16_FLOAT }, };

    switch (_tex_desc.format)
    {
    case TEXTURE_FORMAT_SINT   : return FORMAT_TABLE[0][_tex_desc.component_count-1];
    case TEXTURE_FORMAT_UINT   : return FORMAT_TABLE[1][_tex_desc.component_count-1];
    case TEXTURE_FORMAT_SFLOAT : return FORMAT_TABLE[2][_tex_desc.component_count-1];

    default:
        return buma3d::RESOURCE_FORMAT_UNKNOWN;
    }
}
}// namespace tex


namespace b = buma3d;

template<typename T>
using Ptr = buma3d::util::Ptr<T>;


class HelloTexture::ResizeEvent : public IEvent
{
public:
    ResizeEvent(HelloTexture& _owner) : owner{ _owner } {}
    virtual ~ResizeEvent() {}
    void Execute(IEventArgs* _args) override { owner.OnResize(static_cast<ResizeEventArgs*>(_args)); }    
private:
    HelloTexture& owner;
};

class HelloTexture::BufferResizedEvent : public IEvent
{
public:
    BufferResizedEvent(HelloTexture& _owner) : owner{ _owner } {}
    virtual ~BufferResizedEvent() {}
    void Execute(IEventArgs* _args) override { owner.OnResized(static_cast<BufferResizedEventArgs*>(_args)); }
private:
    HelloTexture& owner;
};


HelloTexture::HelloTexture()
    : ApplicationBase       ()
    , platform              {}
    , spwindow              {}
    , window                {}
    , device                {}
    , quad                  {}
    //, index                 {}
    , cb_model              {}
    , cb_scene              {}
    , command_queue         {}
    , timer                 {}
    , swapchain             {}
    , back_buffers          {}
    , back_buffer_index     {}
    , swapchain_fences      {}
    , CBV_ALIGNMENT         {}
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
    , signature             {}
    , descriptor_pool       {}
    , descriptor_sets       {}
    , render_pass           {}
    , vertex_buffer         {}
    , index_buffer          {}
    , vertex_buffer_view    {}
    , index_buffer_view     {}
    , cb_heap               {}
    , frame_cbs             {}
    , texture               {}
    , signal_fence_desc     {}
    , wait_fence_desc       {}
    , submit_info           {}
    , submit                {}
    , present_info          {}
    , present_region        {}
    , on_resize             {}
    , on_resized            {}
{    
    //g_cam.type = Camera::CameraType::firstperson;
    g_cam.type = Camera::CameraType::lookat;
    g_cam.setPosition(glm::vec3(0.0f, 0.0f, -2.5f));
    g_cam.setRotation(glm::vec3(0.0f));
    g_cam.setRotationSpeed(0.5f);
    g_cam.setPerspective(60.0f, (float)1280 / (float)720, 1.0f, 256.0f);

    g_fpss = new std::remove_pointer_t<decltype(g_fpss)>;
}

HelloTexture::~HelloTexture()
{
    delete g_fpss;
    g_fpss = nullptr;
    //Term();
}

HelloTexture* HelloTexture::Create()
{
    return new HelloTexture();
}

bool HelloTexture::Prepare(PlatformBase& _platform)
{
    platform = &_platform;
    dr       = platform->GetDeviceResources();
    device   = dr->GetDevice();

    spwindow = platform->GetWindow();
    window   = spwindow.get();
    window->SetWindowTitle("Buma3DSamples - HelloTexture");

    if (!PrepareSwapChain()) return false;
    PrepareSubmitInfo();
    CreateEvents();
    if (!Init()) return false;

    return true;
}

void HelloTexture::PrepareSubmitInfo()
{
    // キューへの送信情報
    submit_info.num_command_lists_to_execute = 1;
    submit.signal_fence_to_cpu               = nullptr;
    submit.num_submit_infos                  = 1;
    submit.submit_infos                      = &submit_info;
}

void HelloTexture::CreateEvents()
{
    // イベントを登録
    on_resize = IEvent::Create<ResizeEvent>(*this);
    on_resized = IEvent::Create<BufferResizedEvent>(*this);
    window->AddResizeEvent(on_resize);
    window->AddBufferResizedEvent(on_resized);
}

bool HelloTexture::PrepareSwapChain()
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

bool HelloTexture::Init()
{
    auto&& resolution = swapchain->GetSwapChain()->GetDesc().buffer;
    vpiewport       = {   0, 0  ,  (float)resolution.width, (float)resolution.height, b::B3D_VIEWPORT_MIN_DEPTH, b::B3D_VIEWPORT_MAX_DEPTH };
    scissor_rect    = { { 0, 0 },        {resolution.width,        resolution.height} };
    command_queue   = dr->GetCommandQueues(b::COMMAND_TYPE_DIRECT)[0];

    CBV_ALIGNMENT = dr->GetDeviceAdapterLimits().min_constant_buffer_offset_alignment;

    auto&& copy_que = dr->GetCommandQueues(b::COMMAND_TYPE_COPY_ONLY)[0];
    copy_ctx.Init(dr, copy_que ? copy_que : command_queue);
    ctx.Init(dr, command_queue);

    if (!LoadAssets()) return false;

    return command_queue;
}

bool HelloTexture::LoadAssets()
{
    auto aspect_ratio = window->GetAspectRatio();
    quad = {
          { { -1.0f , 1.0f, 0.0f, 1.f }, { 0.f, 0.f } }
        , { {  1.0f,  1.0f, 0.0f, 1.f }, { 1.f, 0.f } }
        , { { -1.0f, -1.0f, 0.0f, 1.f }, { 0.f, 1.f } }
        , { {  1.0f, -1.0f, 0.0f, 1.f }, { 1.f, 1.f } }
    };
    //index = { 0,1,2,3 };

    if (!CreateRootSignature())     return false;
    if (!CreateDescriptorPool())    return false;
    if (!AllocateDescriptorSets())  return false;
    if (!CreateRenderPass())        return false;
    if (!CreateFramebuffer())       return false;
    if (!CreateShaderModules())     return false;
    if (!CreateGraphicsPipelines()) return false;
    if (!CreateCommandAllocator())  return false;
    if (!CreateCommandLists())      return false;
    if (!CreateFences())            return false;

    if (!CreateBuffers())               return false;
    if (!CopyBuffers())                 return false;
    if (!CreateBufferViews())           return false;
    if (!CreateConstantBuffer())        return false;
    if (!CreateConstantBufferView())    return false;
    if (!LoadTextureData())             return false;
    if (!CreateTextureResource())       return false;
    if (!CopyDataToTexture())           return false;
    if (!CreateShaderResourceView())    return false;
    if (!CreateSampler())               return false;
    if (!UpdateDescriptorSets())        return false;

    // 描画コマンドを記録
    for (size_t i = 0; i < BACK_BUFFER_COUNT; i++)
        PrepareFrame(i);

    return true;
}

bool HelloTexture::CreateRootSignature()
{
    util::RootSignatureDesc rsdesc{};

    // モデル定数
    auto&& model_rp = rsdesc.AddNewRootParameter();
    model_rp.SetShaderVisibility(b::SHADER_VISIBILITY_VERTEX);
    model_rp.InitAsDescriptorTable();
    //                type                  , num_descriptors , base_shader_register
    model_rp.AddRange(b::DESCRIPTOR_TYPE_CBV, 1               , 0);

    // シーン定数
    auto&& scene_rp = rsdesc.AddNewRootParameter();
    scene_rp.SetShaderVisibility(b::SHADER_VISIBILITY_VERTEX);
    scene_rp.InitAsDescriptorTable();
    scene_rp.AddRange(b::DESCRIPTOR_TYPE_CBV, 1, 0, 1);

    // テクスチャ
    auto&& tex_rp = rsdesc.AddNewRootParameter();
    tex_rp.SetShaderVisibility(b::SHADER_VISIBILITY_ALL_GRAPHICS_COMPUTE);
    tex_rp.InitAsDescriptorTable();
    tex_rp.AddRange(b::DESCRIPTOR_TYPE_SRV_TEXTURE, 1, 0, 0);

    // サンプラー
    auto&& sampler_rp = rsdesc.AddNewRootParameter();
    sampler_rp.SetShaderVisibility(b::SHADER_VISIBILITY_ALL_GRAPHICS_COMPUTE);
    sampler_rp.InitAsDescriptorTable();
    sampler_rp.AddRange(b::DESCRIPTOR_TYPE_SAMPLER, 1, 0, 0);

    rsdesc.SetRegisterShift(b::SHADER_REGISTER_TYPE_T, 1, 0);// register(t0, space0) -> register(t1, space0)
    rsdesc.SetRegisterShift(b::SHADER_REGISTER_TYPE_S, 2, 0);// register(s0, space0) -> register(s2, space0)

    auto bmr = device->CreateRootSignature(rsdesc.Get(b::ROOT_SIGNATURE_FLAG_NONE), &signature);
    BMR_RET_IF_FAILED(bmr);

    return true;
}
bool HelloTexture::CreateDescriptorPool()
{
    uint32_t max_num_register_space{};
    std::vector<b::DESCRIPTOR_POOL_SIZE> pool_sizes;

    if constexpr (false)
    {
        pool_sizes.resize(signature->GetDescriptorPoolRequirementSizes(BACK_BUFFER_COUNT, &max_num_register_space, nullptr));
        signature->GetDescriptorPoolRequirementSizes(BACK_BUFFER_COUNT, &max_num_register_space, pool_sizes.data());
    }
    else
    {
        pool_sizes.resize(device->GetDescriptorPoolSizesAllocationInfo(1, signature.GetAddressOf(), &BACK_BUFFER_COUNT, &max_num_register_space, nullptr));
        device->GetDescriptorPoolSizesAllocationInfo(1, signature.GetAddressOf(), &BACK_BUFFER_COUNT, &max_num_register_space, pool_sizes.data());
    }

    assert(pool_sizes[0].type == b::DESCRIPTOR_TYPE_CBV);
    assert(pool_sizes[0].num_descriptors == 6);

    b::DESCRIPTOR_POOL_DESC pool_desc{};
    pool_desc.flags                     = b::DESCRIPTOR_POOL_FLAG_COPY_SRC;
    pool_desc.max_sets_allocation_count = BACK_BUFFER_COUNT;
    pool_desc.max_num_register_space    = max_num_register_space;

    pool_desc.num_pool_sizes = (uint32_t)pool_sizes.size();
    pool_desc.pool_sizes     = pool_sizes.data();
    pool_desc.node_mask      = b::B3D_DEFAULT_NODE_MASK;

    auto bmr = device->CreateDescriptorPool(pool_desc, &descriptor_pool);
    BMR_RET_IF_FAILED(bmr);

    return true;
}
bool HelloTexture::AllocateDescriptorSets()
{
    descriptor_sets.resize(BACK_BUFFER_COUNT);
    for (auto& i : descriptor_sets)
    {
        auto bmr = descriptor_pool->AllocateDescriptorSet(signature.Get(), &i);
        BMR_RET_IF_FAILED(bmr);
    }
    return true;
}
bool HelloTexture::CreateRenderPass()
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
bool HelloTexture::CreateFramebuffer()
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
bool HelloTexture::CreateShaderModules()
{
    b::BMRESULT bmr{};
    shader_modules.resize(2);
    shader::LOAD_SHADER_DESC desc{};
    desc.options.packMatricesInRowMajor     = false;       // Experimental: Decide how a matrix get packed
    desc.options.enable16bitTypes           = false;       // Enable 16-bit types, such as half, uint16_t. Requires shader model 6.2+
    desc.options.enableDebugInfo            = false;       // Embed debug info into the binary
    desc.options.disableOptimizations       = false;       // Force to turn off optimizations. Ignore optimizationLevel below.

    desc.options.optimizationLevel          = 3; // 0 to 3, no optimization to most optimization
    desc.options.shaderModel                = { 6, 2 };

    desc.options.shiftAllTexturesBindings   = 1;// register(t0, space0) -> register(t1, space0)
    desc.options.shiftAllSamplersBindings   = 2;// register(s0, space0) -> register(s2, space0)
    desc.options.shiftAllCBuffersBindings   = 0;
    desc.options.shiftAllUABuffersBindings  = 0;

    auto&& loader = dr->GetShaderLoader();
    // vs
    {
        desc.entry_point    = "main";
        desc.filename       = "./Samples/HelloTexture/Shader/VertexShader.hlsl";
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
        desc.filename       = "./Samples/HelloTexture/Shader/PixelShader.hlsl";
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
bool HelloTexture::CreateGraphicsPipelines()
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
            input_elements[1] = { "TEXCOORD"   , 0             , b::RESOURCE_FORMAT_R32G32_FLOAT      , b::B3D_APPEND_ALIGNED_ELEMENT };

            input_slot.slot_number              = 0;
            input_slot.stride_in_bytes          = sizeof(VERTEX);
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
            ia.topology = b::PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            pso_desc.input_assembly_state = &ia;
        }

        pso_desc.tessellation_state   = nullptr;
        pso_desc.viewport_state       = nullptr;

        b::RASTERIZATION_STATE_DESC rs{};
        {
            rs.fill_mode                        = b::FILL_MODE_SOLID;
            rs.cull_mode                        = b::CULL_MODE_NONE;
            //rs.cull_mode                        = b::CULL_MODE_BACK;
            rs.is_front_counter_clockwise       = false;
            rs.is_enabled_depth_clip            = false;
            rs.is_enabled_depth_bias            = false;
            rs.depth_bias_scale                 = 0;
            rs.depth_bias_clamp                 = 0.f;
            rs.depth_bias_slope_scale           = 0.f;
            rs.is_enabled_conservative_raster   = false;
            rs.line_rasterization_mode          = b::LINE_RASTERIZATION_MODE_DEFAULT;
            rs.line_width                       = 1.f;
            
            pso_desc.rasterization_state  = &rs;
        }

        pso_desc.stream_output        = nullptr;

        b::MULTISAMPLE_STATE_DESC ms{};
        {
            ms.is_enabled_alpha_to_coverage     = false;
            ms.is_enabled_sample_rate_shading   = false;
            ms.rasterization_samples            = 1;
            ms.sample_masks                     = b::B3D_DEFAULT_SAMPLE_MASK;
            ms.sample_position_state.is_enabled = false;
            ms.sample_position_state.desc       = nullptr;

            pso_desc.multisample_state = &ms;
        }

        b::DEPTH_STENCIL_STATE_DESC ds{};
        {
            ds.is_enabled_depth_test        = false;
            ds.is_enabled_depth_write       = false;
            ds.depth_comparison_func        = b::COMPARISON_FUNC_NEVER;
            ds.is_enabled_depth_bounds_test = false;
            ds.min_depth_bounds             = 0;
            ds.max_depth_bounds             = 1;
            ds.is_enabled_stencil_test      = false;
            
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
            attachments.is_enabled_blend    = false;
            attachments.src_blend           = b::BLEND_FACTOR_ONE;
            attachments.dst_blend           = b::BLEND_FACTOR_ONE;
            attachments.blend_op            = b::BLEND_OP_ADD;
            attachments.src_blend_alpha     = b::BLEND_FACTOR_ONE;
            attachments.dst_blend_alpha     = b::BLEND_FACTOR_ONE;
            attachments.blend_op_alpha      = b::BLEND_OP_ADD;
            attachments.color_write_mask    = b::COLOR_WRITE_FLAG_ALL;

            bs.is_enabled_independent_blend = false;
            bs.is_enabled_logic_op          = false;
            bs.logic_op                     = b::LOGIC_OP_SET;
            bs.num_attachments              = 1;
            bs.attachments                  = &attachments;
            bs.blend_constants              = { 1.f, 1.f, 1.f, 1.f };

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
bool HelloTexture::CreateCommandAllocator()
{
    cmd_allocator.resize(BACK_BUFFER_COUNT);
    for (auto& i : cmd_allocator)
    {
        b::COMMAND_ALLOCATOR_DESC cad{};
        cad.type    = b::COMMAND_TYPE_DIRECT;
        cad.level   = b::COMMAND_LIST_LEVEL_PRIMARY;
        cad.flags   = b::COMMAND_ALLOCATOR_FLAG_NONE /*| b::COMMAND_ALLOCATOR_FLAG_TRANSIENT*/;

        auto bmr = device->CreateCommandAllocator(cad, &i);
        BMR_RET_IF_FAILED(bmr);
    }

    return true;
}
bool HelloTexture::CreateCommandLists()
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
bool HelloTexture::CreateFences()
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
bool HelloTexture::CreateBuffers()
{
    auto&& rc = dr->GetResourceCreate();

    b::BMRESULT bmr{};
    // 頂点バッファを作成
    {
        vertex_buffer = rc->CreateBuffer(init::BufferResourceDesc(sizeof(VERTEX) * quad.size(), init::BUF_COPYABLE_FLAGS | b::BUFFER_USAGE_FLAG_VERTEX_BUFFER), buma3d::RESOURCE_HEAP_PROPERTY_FLAG_DEVICE_LOCAL);
        RET_IF_FAILED(vertex_buffer);
        vertex_buffer->SetName("Vertex buffer");
    }

    //// インデックスバッファを作成
    //{
    //    index_buffer = rc->CreateBuffer(init::BufferResourceDesc(sizeof(decltype(index)::value_type) * index.size(), init::BUF_COPYABLE_FLAGS | b::BUFFER_USAGE_FLAG_INDEX_BUFFER), buma3d::RESOURCE_HEAP_PROPERTY_FLAG_DEVICE_LOCAL);
    //    RET_IF_FAILED(index_buffer);
    //    index_buffer->SetName("Vertex buffer");
    //}

    return true;
}
bool HelloTexture::CopyBuffers()
{
    copy_ctx.Begin();
    copy_ctx.CopyDataToBuffer(vertex_buffer->GetB3DBuffer().Get(), 0, sizeof(VERTEX) * quad.size(), quad.data());
    //copy_ctx.CopyDataToBuffer(index_buffer->GetB3DBuffer().Get(), 0, sizeof(decltype(index)::value_type) * index.size(), index.data());
    copy_ctx.End(copy_ctx.GetGpuWaitFence());
    BMR_RET_IF_FAILED(copy_ctx.WaitOnCpu());

    return true;
}
bool HelloTexture::CreateBufferViews()
{
    b::BMRESULT bmr{};
    // 頂点バッファビューを作成
    {
        b::VERTEX_BUFFER_VIEW_DESC vbvdesc{};

        uint64_t buffer_offset      = 0;
        uint32_t sizes_in_bytes     = vertex_buffer->GetB3DDesc().buffer.size_in_bytes;
        uint32_t strides_in_bytes   = sizeof(VERTEX);
        vbvdesc.num_input_slots     = 1;
        vbvdesc.buffer_offsets      = &buffer_offset;
        vbvdesc.sizes_in_bytes      = &sizes_in_bytes;
        vbvdesc.strides_in_bytes    = &strides_in_bytes;

        bmr = device->CreateVertexBufferView(vertex_buffer->GetB3DBuffer().Get(), vbvdesc, &vertex_buffer_view);
        BMR_RET_IF_FAILED(bmr);
    }

    //// インデックスバッファビューを作成
    //{
    //    b::INDEX_BUFFER_VIEW_DESC ibvdesc{};
    //    ibvdesc.buffer_offset   = 0;
    //    ibvdesc.size_in_bytes   = index_buffer->GetB3DDesc().buffer.size_in_bytes;
    //    ibvdesc.index_type      = b::INDEX_TYPE_UINT16;

    //    bmr = device->CreateIndexBufferView(index_buffer->GetB3DBuffer().Get(), ibvdesc, &index_buffer_view);
    //    BMR_RET_IF_FAILED(bmr);
    //}

    return true;
}
bool HelloTexture::CreateConstantBuffer()
{
    // 定数バッファを作成します。
    // コマンドリストがGPUで実行中に参照しているリソースが外部(CPU等)から変更されると表示結果に悪影響を与える可能性があります。 
    // 各フレーム用コマンドリストに同じバッファを使用してしまうと、すべてのコマンドリストで定数バッファの値が書き替わってしまうため、上記の問題が発生します。
    // したがって、定数バッファはバックバッファ毎に必要です。
    auto cb_desc = init::BufferResourceDesc(0, init::BUF_CBV_FLAGS);
    cb_desc.buffer.size_in_bytes += util::AlignUp(sizeof(CB_MODEL), CBV_ALIGNMENT);
    cb_desc.buffer.size_in_bytes += util::AlignUp(sizeof(CB_SCENE), CBV_ALIGNMENT);

    // 定数バッファを作成
    auto rc = dr->GetResourceCreate();
    for (auto& i : frame_cbs)
    {
        if constexpr (USE_HOST_WRITABLE_HEAP)
        {
            i.constant_buffer = rc->CreateBuffer(cb_desc, b::RESOURCE_HEAP_PROPERTY_FLAG_HOST_WRITABLE);
        }
        else
        {
            i.constant_buffer = rc->CreateBuffer(cb_desc, b::RESOURCE_HEAP_PROPERTY_FLAG_DEVICE_LOCAL);
        }
        RET_IF_FAILED(i.constant_buffer);
    }

    cb_model.model = glm::mat4(1.f);
    cb_model.model *= glm::translate(cb_model.model, glm::vec3());
    cb_model.model *= glm::scale    (cb_model.model, glm::vec3(1, 1, 1));
    cb_model.model *= glm::rotate   (cb_model.model, 0.f, glm::vec3(0, 1, 0));

    cb_scene.view_proj = g_cam.matrices.perspective * g_cam.matrices.view;

    // 定数データを送信
    if constexpr (USE_HOST_WRITABLE_HEAP)
    {
        for (auto& i : frame_cbs)
        {
            i.mapped_data[0] = i.constant_buffer->GetMppedData();
            i.mapped_data[1] = i.constant_buffer->GetMppedDataAs<uint8_t>(util::AlignUp(sizeof(CB_MODEL), CBV_ALIGNMENT));
            memcpy(i.mapped_data[0], &cb_model, sizeof(CB_MODEL));
            memcpy(i.mapped_data[1], &cb_scene, sizeof(CB_SCENE));
        }
    }
    else
    {
        ImmediateContext ictx(copy_ctx);
        {
            util::PipelineBarrierDesc barrier{};
            for (auto& i : frame_cbs)
            {
                ictx.CopyDataToBuffer(i.constant_buffer->GetB3DBuffer().Get(), 0
                                     , sizeof(cb_model), &cb_model);

                ictx.CopyDataToBuffer(i.constant_buffer->GetB3DBuffer().Get(), util::AlignUp(sizeof(CB_MODEL), CBV_ALIGNMENT)
                                     , sizeof(cb_scene), &cb_scene);

                //barrier.AddBufferBarrier(init::BufferBarrierDesc(i.constant_buffer.Get(), b::RESOURCE_STATE_UNDEFINED, b::RESOURCE_STATE_CONSTANT_READ));
            }
            //ctx.PipelineBarrier(barrier.Get(b::PIPELINE_STAGE_FLAG_COPY_RESOLVE, b::PIPELINE_STAGE_FLAG_BOTTOM_OF_PIPE));
        }
    }

    return true;
}
bool HelloTexture::CreateConstantBufferView()
{
    b::CONSTANT_BUFFER_VIEW_DESC cbv_desc{};
    for (auto& i : frame_cbs)
    {
        cbv_desc.buffer_offset = 0;
        cbv_desc.size_in_bytes = util::AlignUp(sizeof(CB_MODEL), CBV_ALIGNMENT);
        auto bmr = device->CreateConstantBufferView(i.constant_buffer->GetB3DBuffer().Get(), cbv_desc, &i.model_cbv);
        BMR_RET_IF_FAILED(bmr);

        cbv_desc.buffer_offset = util::AlignUp(sizeof(CB_MODEL), CBV_ALIGNMENT);
        cbv_desc.size_in_bytes = util::AlignUp(sizeof(CB_SCENE), CBV_ALIGNMENT);
        bmr = device->CreateConstantBufferView(i.constant_buffer->GetB3DBuffer().Get(), cbv_desc, &i.scene_cbv);
        BMR_RET_IF_FAILED(bmr);
    }

    return true;
}
bool HelloTexture::LoadTextureData()
{
    tex::TEXTURE_CREATE_DESC texdesc{};
    texdesc.filename    = "./Samples/HelloTexture/Data/UV_Grid_Sm.jpg";
    texdesc.mip_count   = 0;
    texdesc.row_pitch_alignment   = dr->GetDeviceAdapterLimits().buffer_copy_row_pitch_alignment;
    texdesc.slice_pitch_alignment = dr->GetDeviceAdapterLimits().buffer_copy_offset_alignment;
    texture.data = tex::CreateTexturesFromFile(texdesc);
    RET_IF_FAILED(texture.data);

    return true;
}
bool HelloTexture::CreateTextureResource()
{
    auto&& data_desc = texture.data->GetDesc();
    texture.texture = dr->GetResourceCreate()->CreateTexture(init::Tex2DResourceDesc({ (uint32_t)data_desc.width, (uint32_t)data_desc.height }
                                                                                     , tex::GetDefaultFormat(data_desc), init::TEX_STATIC_SRV_FLAGS, data_desc.num_mips, data_desc.depth)
                                                             , b::RESOURCE_HEAP_PROPERTY_FLAG_DEVICE_LOCAL);
    RET_IF_FAILED(texture.texture);

    return true;
}
bool HelloTexture::CopyDataToTexture()
{
    auto&& data_desc = texture.data->GetDesc();
    copy_ctx.Begin();
    {
        util::PipelineBarrierDesc bd{};
        util::TextureBarrierRange tex{};

        tex.AddSubresRange(b::TEXTURE_ASPECT_FLAG_COLOR, 0, 0, 1, texture.texture->GetB3DDesc().texture.mip_levels);
        bd.AddTextureBarrier(tex.Get(texture.texture->GetB3DTexture().Get()), b::RESOURCE_STATE_UNDEFINED, b::RESOURCE_STATE_COPY_DST_WRITE);
        copy_ctx.PipelineBarrier(bd.Get(b::PIPELINE_STAGE_FLAG_TOP_OF_PIPE, b::PIPELINE_STAGE_FLAG_COPY_RESOLVE));

        for (size_t i = 0; i < data_desc.num_mips; i++)
        {
            auto&& tex_data = texture.data->Get(i);
            copy_ctx.CopyDataToTexture(texture.texture->GetB3DTexture().Get(), (uint32_t)i, 0
                                       , tex_data->layout.row_pitch
                                       , tex_data->extent.h
                                       //, tex_data->layout.slice_pitch / tex_data->layout.texel_size
                                       , tex_data->total_size, tex_data->data);
        }

        // 所有権をコピーキューから開放します。
        bd.Reset();
        bd.AddTextureBarrier(tex.Get(texture.texture->GetB3DTexture().Get()), b::RESOURCE_STATE_COPY_DST_WRITE, b::RESOURCE_STATE_SHADER_READ
                             , b::RESOURCE_BARRIER_FLAG_OWNERSHIP_TRANSFER, b::COMMAND_TYPE_COPY_ONLY, b::COMMAND_TYPE_DIRECT);
        copy_ctx.PipelineBarrier(bd.Get(b::PIPELINE_STAGE_FLAG_COPY_RESOLVE, b::PIPELINE_STAGE_FLAG_TOP_OF_PIPE));
    }
    copy_ctx.End(copy_ctx.GetGpuWaitFence());
    BMR_RET_IF_FAILED(copy_ctx.WaitOnCpu());

    // コピーキューから開放された所有権をグラフィックキューで取得します。
    ctx.Begin();
    {
        util::PipelineBarrierDesc bd{};
        util::TextureBarrierRange tex{};
        tex.AddSubresRange(b::TEXTURE_ASPECT_FLAG_COLOR, 0, 0, 1, texture.texture->GetB3DDesc().texture.mip_levels);
        bd.AddTextureBarrier(tex.Get(texture.texture->GetB3DTexture().Get()), b::RESOURCE_STATE_COPY_DST_WRITE, b::RESOURCE_STATE_SHADER_READ
                             , b::RESOURCE_BARRIER_FLAG_OWNERSHIP_TRANSFER, b::COMMAND_TYPE_COPY_ONLY, b::COMMAND_TYPE_DIRECT);
        ctx.PipelineBarrier(bd.Get(b::PIPELINE_STAGE_FLAG_TOP_OF_PIPE, b::PIPELINE_STAGE_FLAG_ALL_GRAPHICS));
    }
    ctx.End(ctx.GetGpuWaitFence());
    BMR_RET_IF_FAILED(ctx.WaitOnCpu());

    return true;
}
bool HelloTexture::CreateShaderResourceView()
{
    buma3d::SHADER_RESOURCE_VIEW_DESC srvd{};
    srvd.view.type          = b::VIEW_TYPE_SHADER_RESOURCE;
    srvd.view.format        = texture.texture->GetB3DDesc().texture.format_desc.format;
    srvd.view.dimension     = b::VIEW_DIMENSION_TEXTURE_2D;
    srvd.texture.components                             = { b::COMPONENT_SWIZZLE_IDENTITY, b::COMPONENT_SWIZZLE_IDENTITY, b::COMPONENT_SWIZZLE_IDENTITY, b::COMPONENT_SWIZZLE_IDENTITY };
    srvd.texture.subresource_range.offset.aspect        = b::TEXTURE_ASPECT_FLAG_COLOR;
    srvd.texture.subresource_range.offset.mip_slice     = 0;
    srvd.texture.subresource_range.offset.array_slice   = 0;
    srvd.texture.subresource_range.mip_levels           = b::B3D_USE_REMAINING_MIP_LEVELS;
    srvd.texture.subresource_range.array_size           = 1;
    srvd.flags                                          = b::SHADER_RESOURCE_VIEW_FLAG_DENY_INPUT_ATTACHMENT;

    auto bmr = device->CreateShaderResourceView(texture.texture->GetB3DTexture().Get(), srvd, &texture.srv);
    BMR_RET_IF_FAILED(bmr);
    
    return true;
}
bool HelloTexture::CreateSampler()
{
    b::SAMPLER_DESC sd{};
    sd.filter.mode            = b::SAMPLER_FILTER_MODE_ANISOTROPHIC;
    sd.filter.reduction_mode  = b::SAMPLER_FILTER_REDUCTION_MODE_STANDARD;
    sd.filter.max_anisotropy  = (uint32_t)dr->GetDeviceAdapterLimits().max_sampler_anisotropy;
    sd.filter.comparison_func = b::COMPARISON_FUNC_NEVER;
    sd.texture.address.u  = b::TEXTURE_ADDRESS_MODE_WRAP;
    sd.texture.address.v  = b::TEXTURE_ADDRESS_MODE_WRAP;
    sd.texture.address.w  = b::TEXTURE_ADDRESS_MODE_WRAP;
    sd.texture.sample.minification  = b::TEXTURE_SAMPLE_MODE_LINEAR;
    sd.texture.sample.magnification = b::TEXTURE_SAMPLE_MODE_LINEAR;
    sd.texture.sample.mip           = b::TEXTURE_SAMPLE_MODE_LINEAR;
    sd.mip_lod.min    = 0.f;
    sd.mip_lod.max    = FLT_MAX;
    sd.mip_lod.bias   = 0.f;
    sd.border_color   = b::BORDER_COLOR_OPAQUE_BLACK_FLOAT;

    auto bmr = device->CreateSampler(sd, &sampler);
    BMR_RET_IF_FAILED(bmr);

    return true;
}
bool HelloTexture::UpdateDescriptorSets()
{
    util::UpdateDescriptorSetDesc update_desc{};
    for (uint32_t i = 0; i < BACK_BUFFER_COUNT; i++)
    {
        auto&& write_set = update_desc.AddNewWriteDescriptorSets();

        // モデル定数
        auto&& write_table_model_cb = write_set.AddNewWriteDescriptorTable();
        write_table_model_cb.AddNewWriteDescriptorRange()
            .SetDstRange(0, 0, 1)
            .SetSrcView(0, frame_cbs[i].model_cbv.Get());
        write_table_model_cb.Finalize(0);

        // シーン定数
        auto&& write_table_scene_cb = write_set.AddNewWriteDescriptorTable();
        write_table_scene_cb.AddNewWriteDescriptorRange()
            .SetDstRange(0, 0, 1)
            .SetSrcView(0, frame_cbs[i].scene_cbv.Get());
        write_table_scene_cb.Finalize(1);

        if (i == 0)// 最初のセットのみに書き込みます。 
        {
            // テクスチャ
            auto&& write_table_texture = write_set.AddNewWriteDescriptorTable();
            write_table_texture.AddNewWriteDescriptorRange()
                .SetDstRange(0, 0, 1)
                .SetSrcView(0, texture.srv.Get());
            write_table_texture.Finalize(2);

            // サンプラー
            auto&& write_table_sampler = write_set.AddNewWriteDescriptorTable();
            write_table_sampler.AddNewWriteDescriptorRange()
                .SetDstRange(0, 0, 1)
                .SetSrcView(0, sampler.Get());
            write_table_sampler.Finalize(3);
        }

        write_set.Finalize(descriptor_sets[i].Get());
    }

    // テクスチャ、サンプラをコピー
    for (uint32_t i = 1; i < BACK_BUFFER_COUNT; i++)
    {
        auto&& copy_set = update_desc.AddNewCopyDescriptorSets();

        // テクスチャ
        copy_set.AddNewCopyTable()
            .SetRootParameterIndex(2, 2)
            .AddRange(0, 0, 0, 0, 1);

        // サンプラー
        copy_set.AddNewCopyTable()
            .SetRootParameterIndex(3, 3)
            .AddRange(0, 0, 0, 0, 1);

        copy_set.Finalize(descriptor_sets[0].Get(), descriptor_sets[i].Get());
    }

    update_desc.Finalize();
    auto bmr = device->UpdateDescriptorSets(update_desc.Get());
    BMR_RET_IF_FAILED(bmr);

    return true;
}

void HelloTexture::PrepareFrame(uint32_t _buffer_index)
{
    auto reset_flags = b::COMMAND_ALLOCATOR_RESET_FLAG_NONE;
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
        tb.view           = (*back_buffers).data()[_buffer_index].rtv.Get();
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

        b::CMD_BIND_DESCRIPTOR_SET bind_sets{};
        bind_sets.descriptor_set                 = descriptor_sets.data()[_buffer_index].Get();
        bind_sets.num_dynamic_descriptor_offsets = 0;
        bind_sets.dynamic_descriptor_offsets     = nullptr;
        l->BindDescriptorSet(b::PIPELINE_BIND_POINT_GRAPHICS, bind_sets);

        l->BindVertexBufferViews({ 0, 1, vertex_buffer_view.GetAddressOf() });
        //l->BindIndexBufferView(index_buffer_view.Get());

        static float sc = 0.f;
        static float sx = 0.f;
        sc = sc + 0.34f * timer.GetElapsedSecondsF();
        sx = fabsf(sinf(sc));
        b::CLEAR_VALUE            clear_val{ b::CLEAR_RENDER_TARGET_VALUE{0.9f * sx ,0.28f,0.13f,1.f} };
        b::RENDER_PASS_BEGIN_DESC rpbd{ render_pass.Get(), framebuffers.data()[_buffer_index].Get(), 1, &clear_val };
        b::SUBPASS_BEGIN_DESC     spbd{ b::SUBPASS_CONTENTS_INLINE };
        l->BeginRenderPass(rpbd, spbd);
        {
            l->SetViewports(1, &vpiewport);
            l->SetScissorRects(1, &scissor_rect);

            ////             { index_count_per_instance, instance_count, start_index_location, base_vertex_location, start_instance_location }
            //l->DrawIndexed({ 4                       , 1             , 0                   , 0                   , 0                       });            
            l->Draw({ 4, 1, 0, 0 });
        }
        l->EndRenderPass({});
    }
    bmr = l->EndRecord();
    assert(bmr == b::BMRESULT_SUCCEED);
}

void HelloTexture::Tick()
{
    timer.Tick();
    if (timer.IsOneSecElapsed())
    {
        if (!g_first)
            g_fpss->emplace_back(timer.GetFramesPerSecond());
        g_first = false;

        platform->GetLogger()->LogInfo(("fps: " + std::to_string(timer.GetFramesPerSecond())).c_str());
    }

    Update();
    Render();
}

void HelloTexture::Update()
{
    if (window->GetWindowStateFlags() & WINDOW_STATE_FLAG_MINIMIZED)
        return;

    // 次のバックバッファを取得
    MoveToNextFrame();

    {
        auto&& key = input::GetKey();
        g_cam.keys.up       = key.W.press_count;
        g_cam.keys.down     = key.S.press_count;
        g_cam.keys.left     = key.A.press_count;
        g_cam.keys.right    = key.D.press_count;

        auto&& mouse = input::GetMouse();
        auto dx = (float)mouse.x.delta;
        auto dy = (float)mouse.y.delta;
        if (mouse.buttons.left.press_count)
            g_cam.rotate(glm::vec3(dy * g_cam.rotationSpeed, dx * g_cam.rotationSpeed, 0.0f));

        if (mouse.buttons.right.press_count)
            g_cam.translate(glm::vec3(-0.0f, 0.0f, dy * .005f));

        if (mouse.buttons.middle.press_count)
            g_cam.translate(glm::vec3(dx * 0.005f, -dy * 0.005f, 0.0f));

        auto wheel_delta = static_cast<float>(mouse.rot.delta) * 0.005f;
        if (wheel_delta != 0.f)
            g_cam.translate(glm::vec3(0.0f, 0.0f, wheel_delta));

        auto dirty = g_cam.dirty;
        g_cam.update(timer.GetElapsedSecondsF());
        //if (g_cam.updated || dirty)
        {
            cb_scene.view_proj = g_cam.matrices.perspective * g_cam.matrices.view;
            if constexpr (USE_HOST_WRITABLE_HEAP)
            {
                memcpy(frame_cbs[back_buffer_index].mapped_data[1], &cb_scene, sizeof(CB_SCENE));
            }
            else
            {
                ctx.Begin();
                ctx.CopyDataToBuffer(frame_cbs[back_buffer_index].constant_buffer->GetB3DBuffer().Get(), util::AlignUp(sizeof(CB_MODEL), CBV_ALIGNMENT)
                                     , util::AlignUp(sizeof(CB_SCENE), CBV_ALIGNMENT)
                                     , &cb_scene);
                ctx.End(ctx.GetGpuWaitFence());
                ctx.WaitOnCpu();
            }
        }
    }

}

void HelloTexture::MoveToNextFrame()
{
    uint32_t next_buffer_index = 0;
    auto bmr = swapchain->AcquireNextBuffer(UINT32_MAX, &next_buffer_index);
    assert(bmr == b::BMRESULT_SUCCEED || bmr == b::BMRESULT_SUCCEED_NOT_READY);

    back_buffer_index = next_buffer_index;
}

void HelloTexture::Render()
{
    if (window->GetWindowStateFlags() & WINDOW_STATE_FLAG_MINIMIZED)
        return;

    auto cmd_lists_data  = cmd_lists.data();
    auto cmd_fences_data = cmd_fences.data();
    b::BMRESULT bmr{};

    // コマンドリストとフェンスを送信
    {
        cmd_fences_data[back_buffer_index]->Wait(fence_values[back_buffer_index].wait, UINT32_MAX);
        //PrepareFrame(back_buffer_index);

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

    fence_values[back_buffer_index]++;
}

void HelloTexture::OnResize(ResizeEventArgs* _args)
{
    command_queue->WaitIdle();
    framebuffers = {};
    back_buffers = {};
}

void HelloTexture::OnResized(BufferResizedEventArgs* _args)
{
    back_buffers     = &swapchain->GetBuffers();
    swapchain_fences = &swapchain->GetPresentCompleteFences();

    vpiewport       = {   0, 0  ,  (float)_args->size.width, (float)_args->size.height, b::B3D_VIEWPORT_MIN_DEPTH, b::B3D_VIEWPORT_MAX_DEPTH };
    scissor_rect    = { { 0, 0 },        {_args->size.width,        _args->size.height} };

    if ((_args->size.width > 0.0f) && (_args->size.height > 0.0f))
        g_cam.updateAspectRatio((float)_args->size.width / (float)_args->size.height);

    CreateFramebuffer();
    for (size_t i = 0; i < BACK_BUFFER_COUNT; i++)
        PrepareFrame(i);

    Update();
    Render();
}

void HelloTexture::Term()
{
    dr->WaitForGpu();

    // result
    {
        float res = 0.f;
        float size = static_cast<float>(g_fpss->size());
        for (auto& i : *g_fpss)
            res += i;
        std::stringstream ss;
        ss << "\nprof result: average fps";
        ss << (res / size) << std::endl;

        platform->GetLogger()->LogInfo(ss.str().c_str());
    }

    // オブジェクトの解放
    for (auto& i : cmd_allocator)
        i->Reset(b::COMMAND_ALLOCATOR_RESET_FLAG_RELEASE_RESOURCES);
    cmd_lists = {};
    cmd_allocator = {};
    descriptor_sets = {};
    descriptor_pool.Reset();
    copy_ctx.Reset();
    ctx.Reset();
    for (auto& i : frame_cbs)
    {
        i.model_cbv.Reset();
        i.scene_cbv.Reset();
        i.constant_buffer.reset();
    }
    if (cb_heap)
        dr->GetResourceHeapsAllocator()->Free(cb_heap);
    cb_model = {};
    cb_scene = {};
    vertex_buffer_view.Reset();
    index_buffer_view.Reset();
    vertex_buffer.reset();
    index_buffer.reset();
    texture = {};
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
