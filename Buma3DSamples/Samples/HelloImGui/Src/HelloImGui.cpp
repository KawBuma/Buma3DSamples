#include "pch.h"
#include "HelloImGui.h"
#include "External/Camera.h"

#include <cassert>

#include "Scenes.h" 

#define BMR_RET_IF_FAILED(x) if (x >= buma3d::BMRESULT_FAILED) { assert(false && #x); return false; }
#define RET_IF_FAILED(x) if (!(x)) { assert(false && #x); return false; }

namespace /*anonymous*/
{

std::vector<float>* g_fpss                 = nullptr;
bool                g_first                = true;
constexpr bool      USE_HOST_WRITABLE_HEAP = true;

Camera g_cam{};

}// namespace /*anonymous*/

namespace init = buma3d::hlp::init;
namespace b = buma3d;

template<typename T>
using Ptr = buma3d::util::Ptr<T>;

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

class HelloImGui::ResizeEvent : public IEvent
{
public:
    ResizeEvent(HelloImGui& _owner) : owner{ _owner } {}
    virtual ~ResizeEvent() {}
    void Execute(IEventArgs* _args) override { owner.OnResize(static_cast<ResizeEventArgs*>(_args)); }    
private:
    HelloImGui& owner;
};

class HelloImGui::BufferResizedEvent : public IEvent
{
public:
    BufferResizedEvent(HelloImGui& _owner) : owner{ _owner } {}
    virtual ~BufferResizedEvent() {}
    void Execute(IEventArgs* _args) override { owner.OnResized(static_cast<BufferResizedEventArgs*>(_args)); }
private:
    HelloImGui& owner;
};

class HelloImGui::ProcessMessageEvent : public IEvent
{
public:
    ProcessMessageEvent(HelloImGui& _owner) : owner{ _owner } {}
    virtual ~ProcessMessageEvent() {}
    void Execute(IEventArgs* _args) override { owner.OnProcessMessage(static_cast<ProcessMessageEventArgs*>(_args)); }
private:
    HelloImGui& owner;
};


HelloImGui::HelloImGui()
    : ApplicationBase           ()
    , platform                  {}
    , spwindow                  {}
    , window                    {}
    , device                    {}
    , quad                      {}
    , cb_model                  {}
    , cb_scene                  {}
    , command_queue             {}
    , timer                     {}
    , swapchain                 {}
    , back_buffers              {}
    , back_buffer_index         {}
    , swapchain_fences          {}
    , CBV_ALIGNMENT             {}
    , vpiewport                 {}
    , scissor_rect              {}
    , framebuffers              {}
    , shader_modules            {}
    , pipeline                  {}
    , cmd_allocator             {}
    , cmd_lists                 {}
    , util_fence                {}
    , fence_values              {}
    , cmd_fences                {}
    , buffer_layout             {}
    , texture_layout            {}
    , pipeline_layout           {}
    , descriptor_heap           {}
    , descriptor_pool           {}
    , descriptor_update         {}
    , buffer_descriptor_sets    {}
    , texture_descriptor_set    {}
    , render_pass               {}
    , vertex_buffer             {}
    , vertex_buffer_view        {}
    , cb_heap                   {}
    , frame_cbs                 {}
    , texture                   {}
    , signal_fence_desc         {}
    , wait_fence_desc           {}
    , submit_info               {}
    , submit                    {}
    , present_info              {}
    , present_region            {}
    , on_resize                 {}
    , on_resized                {}
    , on_process_message        {}
    , myimgui                   {}
    , myimgui_framebuffers      {}
    , is_enabled_gui            {}
{    
    g_fpss = new std::remove_pointer_t<decltype(g_fpss)>;
}

HelloImGui::~HelloImGui()
{
    delete g_fpss;
    g_fpss = nullptr;
}

HelloImGui* HelloImGui::Create()
{
    return new HelloImGui();
}

bool HelloImGui::Prepare(PlatformBase& _platform)
{
    platform = &_platform;
    dr       = platform->GetDeviceResources();
    device   = dr->GetDevice();

    if (!PrepareSettings()) return false;
    settings.window_desc.need_window    = true;
    settings.window_desc.name           = "Buma3DSamples - HelloImGui";

    spwindow = platform->GetWindow();
    window = spwindow.get();

    PrepareSubmitInfo();
    CreateEvents();

    return true;
}

void HelloImGui::PrepareSubmitInfo()
{
    // キューへの送信情報
    submit_info.num_command_lists_to_execute = 1;
    submit.signal_fence_to_cpu               = nullptr;
    submit.num_submit_infos                  = 1;
    submit.submit_infos                      = &submit_info;
}

void HelloImGui::CreateEvents()
{
    // イベントを登録
    on_resize          = IEvent::Create<ResizeEvent>(*this);
    on_resized         = IEvent::Create<BufferResizedEvent>(*this);
    on_process_message = IEvent::Create<ProcessMessageEvent>(*this);
    window->AddResizeEvent(on_resize);
    window->AddBufferResizedEvent(on_resized);
    window->AddProcessMessageEvent(on_process_message);
}

bool HelloImGui::Init()
{
    if (!InitSwapChain()) return false;
    auto&& resolution = swapchain->GetSwapChain()->GetDesc().buffer;
    vpiewport       = {   0, 0  ,  (float)resolution.width, (float)resolution.height, b::B3D_VIEWPORT_MIN_DEPTH, b::B3D_VIEWPORT_MAX_DEPTH };
    scissor_rect    = { { 0, 0 },        {resolution.width,        resolution.height} };

    //g_cam.type = Camera::CameraType::firstperson;
    g_cam.type = Camera::CameraType::lookat;
    g_cam.setPosition(glm::vec3(0.0f, 0.0f, -2.5f));
    g_cam.setRotation(glm::vec3(0.0f));
    g_cam.setRotationSpeed(0.5f);
    g_cam.setPerspective(60.0f, (float)settings.window_desc.width / (float)settings.window_desc.height, 1.0f, 256.0f);

    CBV_ALIGNMENT = dr->GetDeviceAdapterLimits().min_constant_buffer_offset_alignment;

    command_queue = dr->GetCommandQueues(b::COMMAND_TYPE_DIRECT)[0];
    auto&& copy_ques = dr->GetCommandQueues(b::COMMAND_TYPE_COPY_ONLY);
    if (!ctx.Init(dr, command_queue))                                           return false;
    if (!copy_ctx.Init(dr, copy_ques.empty() ? command_queue : copy_ques[0]))   return false;
    if (!LoadAssets())                                                          return false;

    if (!CreateDescriptorSetLayout())       return false;
    if (!CreatePipelineLayout())            return false;
    if (!CreateDescriptorHeapAndPool())     return false;
    if (!AllocateDescriptorSets())          return false;
    if (!CreateRenderPass())                return false;
    if (!CreateFramebuffer())               return false;
    if (!CreateShaderModules())             return false;
    if (!CreateGraphicsPipelines())         return false;
    if (!CreateCommandAllocator())          return false;
    if (!CreateCommandLists())              return false;
    if (!CreateFences())                    return false;

    if (!CreateBuffers())                   return false;
    if (!CopyBuffers())                     return false;
    if (!CreateBufferViews())               return false;
    if (!CreateConstantBuffer())            return false;
    if (!CreateConstantBufferView())        return false;
    if (!CreateTextureResource())           return false;
    if (!CopyDataToTexture())               return false;
    if (!CreateShaderResourceView())        return false;
    if (!CreateSampler())                   return false;
    if (!UpdateDescriptorSets())            return false;

    // 描画コマンドを記録
    for (size_t i = 0; i < BACK_BUFFER_COUNT; i++)
        PrepareFrame(i);

    auto result = InitMyImGui();
    RET_IF_FAILED(result);

    return true;
}
bool HelloImGui::InitSwapChain()
{
    b::SWAP_CHAIN_DESC scd = init::SwapChainDesc(nullptr, buma3d::COLOR_SPACE_SRGB_NONLINEAR,
                                                 init::SwapChainBufferDesc(settings.window_desc.width, settings.window_desc.height, BACK_BUFFER_COUNT, { b::RESOURCE_FORMAT_B8G8R8A8_UNORM }, b::SWAP_CHAIN_BUFFER_FLAG_COLOR_ATTACHMENT),
                                                 dr->GetCommandQueues(b::COMMAND_TYPE_DIRECT)[0].GetAddressOf());
    scd.flags |= b::SWAP_CHAIN_FLAG_ALLOW_DISCARD_AFTER_PRESENT;
    scd.flags |= settings.is_disabled_vsync     ? b::SWAP_CHAIN_FLAG_DISABLE_VERTICAL_SYNC : 0;
    scd.flags |= settings.is_enabled_fullscreen ? b::SWAP_CHAIN_FLAG_FULLSCREEN_EXCLUSIVE : 0;

    window->ResizeWindow({ settings.window_desc.width, settings.window_desc.height }, scd.flags);
    if (!(window->CreateSwapChain(scd, &swapchain)))
        return false;

    back_buffers     = &swapchain->GetBuffers();
    swapchain_fences = &swapchain->GetPresentCompleteFences();

    present_info.num_present_regions = 0;
    present_info.present_regions     = &present_region;
    present_region = { { 0, 0 }, scissor_rect.extent };

    return true;
}

bool HelloImGui::LoadAssets()
{
    auto aspect_ratio = window->GetAspectRatio();
    quad = {
          { { -1.0f , 1.0f, 0.0f, 1.f }, { 0.f, 0.f } }
        , { {  3.0f,  1.0f, 0.0f, 1.f }, { 2.f, 0.f } }
        , { { -1.0f, -3.0f, 0.0f, 1.f }, { 0.f, 2.f } }
        , { {  1.0f, -1.0f, 0.0f, 0.f }, { 1.f, 1.f } }
    };

    if (!LoadTextureData())
        return false;

    return true;
}
bool HelloImGui::LoadTextureData()
{
    tex::TEXTURE_CREATE_DESC texdesc{};
    auto path = AssetPath("UV_Grid_Sm.jpg");
    texdesc.filename    = path.c_str();
    texdesc.mip_count   = 0;
    texdesc.row_pitch_alignment   = dr->GetDeviceAdapterLimits().buffer_copy_row_pitch_alignment;
    texdesc.slice_pitch_alignment = dr->GetDeviceAdapterLimits().buffer_copy_offset_alignment;
    texture.data = tex::CreateTexturesFromFile(texdesc);
    RET_IF_FAILED(texture.data);

    return true;
}

bool HelloImGui::CreateDescriptorSetLayout()
{
    util::DescriptorSetLayoutDesc layout_desc(2);
    // 定数バッファ用レイアウト。 space0 に設定します。
    layout_desc
        //            (descriptor_type       , base_shader_register, num_descriptors, visibility                 , flags                  )
        .AddNewBinding(b::DESCRIPTOR_TYPE_CBV, 0                   , 1              , b::SHADER_VISIBILITY_VERTEX, b::DESCRIPTOR_FLAG_NONE) // model
        .AddNewBinding(b::DESCRIPTOR_TYPE_CBV, 1                   , 1              , b::SHADER_VISIBILITY_VERTEX, b::DESCRIPTOR_FLAG_NONE) // scene
        .SetFlags(b::DESCRIPTOR_SET_LAYOUT_FLAG_NONE)
        .Finalize();
    auto bmr = device->CreateDescriptorSetLayout(layout_desc.Get(), &buffer_layout);
    BMR_RET_IF_FAILED(bmr);

    // テクスチャ用レイアウト。 space1 に設定します。
    layout_desc
        .Reset()
        .AddNewBinding(b::DESCRIPTOR_TYPE_SRV_TEXTURE, 0, 1, b::SHADER_VISIBILITY_ALL_GRAPHICS_COMPUTE, b::DESCRIPTOR_FLAG_NONE)
        .AddNewBinding(b::DESCRIPTOR_TYPE_SAMPLER    , 1, 1, b::SHADER_VISIBILITY_ALL_GRAPHICS_COMPUTE, b::DESCRIPTOR_FLAG_NONE)
        .SetFlags(b::DESCRIPTOR_SET_LAYOUT_FLAG_NONE)
        .Finalize();
    bmr = device->CreateDescriptorSetLayout(layout_desc.Get(), &texture_layout);
    BMR_RET_IF_FAILED(bmr);

    return true;
}
bool HelloImGui::CreatePipelineLayout()
{
    util::PipelineLayoutDesc desc(2, 0);
    desc
        .SetNumLayouts(2)
        .SetLayout(0, buffer_layout.Get())  // space0 モデル定数, シーン定数
        .SetLayout(1, texture_layout.Get()) // space1 テクスチャ
        .SetFlags(b::PIPELINE_LAYOUT_FLAG_NONE)
        .Finalize();

    auto bmr = device->CreatePipelineLayout(desc.Get(), &pipeline_layout);
    BMR_RET_IF_FAILED(bmr);

    return true;
}
bool HelloImGui::CreateDescriptorHeapAndPool()
{
    util::DescriptorSizes sizes;
    sizes.IncrementSizes(buffer_layout.Get(), BACK_BUFFER_COUNT)
         .IncrementSizes(texture_layout.Get(), 1)
         .Finalize();

    b::BMRESULT bmr;
    bmr = device->CreateDescriptorHeap(sizes.GetAsHeapDesc(b::DESCRIPTOR_HEAP_FLAG_NONE, b::B3D_DEFAULT_NODE_MASK), &descriptor_heap);
    BMR_RET_IF_FAILED(bmr);

    bmr = device->CreateDescriptorPool(sizes.GetAsPoolDesc(descriptor_heap.Get(), sizes.GetMaxSetsByTotalMultiplyCount(), b::DESCRIPTOR_POOL_FLAG_NONE), &descriptor_pool);
    BMR_RET_IF_FAILED(bmr);

    return true;
}
bool HelloImGui::AllocateDescriptorSets()
{
    util::DescriptorSetAllocateDesc allocate_desc(BACK_BUFFER_COUNT + 1);

    // buffer_descriptor_setsをBACK_BUFFER_COUNT数分割り当てます。

    allocate_desc.SetNumDescriptorSets(BACK_BUFFER_COUNT + 1);
    for (uint32_t i = 0; i < BACK_BUFFER_COUNT; i++)
        allocate_desc.SetDescriptorSetLayout(i, buffer_layout.Get());
    allocate_desc.SetDescriptorSetLayout(BACK_BUFFER_COUNT, texture_layout.Get());
    allocate_desc.Finalize();

    auto&& [num_sets, dst_sets] = allocate_desc.GetDst();
    auto bmr = descriptor_pool->AllocateDescriptorSets(allocate_desc.Get(), dst_sets);
    BMR_RET_IF_FAILED(bmr);

    buffer_descriptor_sets.resize(BACK_BUFFER_COUNT);
    for (uint32_t i = 0; i < BACK_BUFFER_COUNT; i++)
        buffer_descriptor_sets[i] = dst_sets[i];

    texture_descriptor_set = dst_sets[BACK_BUFFER_COUNT];

    return true;
}
bool HelloImGui::CreateRenderPass()
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
    color_attachment_ref.state_at_pass                = b::RESOURCE_STATE_COLOR_ATTACHMENT_WRITE;
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
    dependencies[0].dst_access                  = b::RESOURCE_ACCESS_FLAG_COLOR_ATTACHMENT_WRITE;
    dependencies[0].dependency_flags            = b::DEPENDENCY_FLAG_BY_REGION;
    dependencies[0].view_offset                 = 0;

    dependencies[1].src_subpass                 = 0;
    dependencies[1].dst_subpass                 = b::B3D_SUBPASS_EXTERNAL;
    dependencies[1].src_stage_mask              = b::PIPELINE_STAGE_FLAG_COLOR_ATTACHMENT_OUTPUT;
    dependencies[1].dst_stage_mask              = b::PIPELINE_STAGE_FLAG_BOTTOM_OF_PIPE;
    dependencies[1].src_access                  = b::RESOURCE_ACCESS_FLAG_COLOR_ATTACHMENT_WRITE;
    dependencies[1].dst_access                  = b::RESOURCE_ACCESS_FLAG_NONE;
    dependencies[1].dependency_flags            = b::DEPENDENCY_FLAG_BY_REGION;
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
bool HelloImGui::CreateFramebuffer()
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
bool HelloImGui::CreateShaderModules()
{
    b::BMRESULT bmr{};
    shader_modules.resize(2);
    shader::LOAD_SHADER_DESC desc{};
    desc.options.pack_matrices_in_row_major = false;       // Experimental: Decide how a matrix get packed
    desc.options.enable16bit_types          = false;       // Enable 16-bit types, such as half, uint16_t. Requires shader model 6.2+
    desc.options.enable_debug_info          = false;       // Embed debug info into the binary
    desc.options.disable_optimizations      = false;       // Force to turn off optimizations. Ignore optimizationLevel below.

    desc.options.optimization_level         = 3; // 0 to 3, no optimization to most optimization
    desc.options.shader_model               = { 6, 2 };

    auto&& loader = dr->GetShaderLoader();
    // vs
    {
        auto path = AssetPath("Shader/VertexShader.hlsl");
        desc.entry_point    = "main";
        desc.filename       = path.c_str();
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
        auto path = AssetPath("Shader/PixelShader.hlsl");
        desc.entry_point    = "main";
        desc.filename       = path.c_str();
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
bool HelloImGui::CreateGraphicsPipelines()
{
    b::BMRESULT bmr{};
    // グラフィックスパイプラインの作成
    {
        b::GRAPHICS_PIPELINE_STATE_DESC pso_desc{};

        pso_desc.pipeline_layout      = pipeline_layout.Get();
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

        pso_desc.stream_output = nullptr;

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
            attachments.is_enabled_blend    = true;
            attachments.src_blend           = b::BLEND_FACTOR_SRC_ALPHA;
            attachments.dst_blend           = b::BLEND_FACTOR_SRC_ALPHA_INVERTED;
            attachments.blend_op            = b::BLEND_OP_ADD;
            attachments.src_blend_alpha     = b::BLEND_FACTOR_ONE;
            attachments.dst_blend_alpha     = b::BLEND_FACTOR_ZERO;
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
bool HelloImGui::CreateCommandAllocator()
{
    cmd_allocator.resize(BACK_BUFFER_COUNT);
    for (auto& i : cmd_allocator)
    {
        b::COMMAND_ALLOCATOR_DESC cad{};
        cad.type    = b::COMMAND_TYPE_DIRECT;
        cad.level   = b::COMMAND_LIST_LEVEL_PRIMARY;
        cad.flags   = b::COMMAND_ALLOCATOR_FLAG_NONE;

        auto bmr = device->CreateCommandAllocator(cad, &i);
        BMR_RET_IF_FAILED(bmr);
    }

    return true;
}
bool HelloImGui::CreateCommandLists()
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
bool HelloImGui::CreateFences()
{
    b::BMRESULT bmr;
    b::FENCE_DESC fd{};
    fd.flags         = b::FENCE_FLAG_NONE;
    fd.initial_value = 0;

    fd.type = b::FENCE_TYPE_BINARY_GPU_TO_CPU;
    {
        bmr = device->CreateFence(fd, &util_fence);
        BMR_RET_IF_FAILED(bmr);
        util_fence->SetName("util_fence");
    }

    fd.type = b::FENCE_TYPE_TIMELINE;
    {
        cmd_fences.resize(BACK_BUFFER_COUNT);
        uint32_t cnt = 0;
        for (auto& i : cmd_fences)
        {
            bmr = device->CreateFence(fd, &i);
            BMR_RET_IF_FAILED(bmr);
            i->SetName(std::string("cmd_fences" + std::to_string(cnt++)).c_str());
        }
    }

    return true;
}
bool HelloImGui::CreateBuffers()
{
    auto&& rc = dr->GetResourceCreate();

    b::BMRESULT bmr{};
    // 頂点バッファを作成
    {
        vertex_buffer = rc->CreateBuffer(init::BufferResourceDesc(sizeof(VERTEX) * quad.size(), init::BUF_COPYABLE_FLAGS | b::BUFFER_USAGE_FLAG_VERTEX_BUFFER), buma3d::RESOURCE_HEAP_PROPERTY_FLAG_DEVICE_LOCAL);
        RET_IF_FAILED(vertex_buffer);
        vertex_buffer->SetName("Vertex buffer");
    }

    return true;
}
bool HelloImGui::CopyBuffers()
{
    copy_ctx.Begin();
    copy_ctx.CopyDataToBuffer(vertex_buffer->GetB3DBuffer().Get(), 0, sizeof(VERTEX) * quad.size(), quad.data());
    copy_ctx.End(copy_ctx.GetGpuWaitFence());
    BMR_RET_IF_FAILED(copy_ctx.WaitOnCpu());

    return true;
}
bool HelloImGui::CreateBufferViews()
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

    return true;
}
bool HelloImGui::CreateConstantBuffer()
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
            i.constant_buffer = rc->CreateBuffer(cb_desc, b::RESOURCE_HEAP_PROPERTY_FLAG_HOST_WRITABLE);
        else
            i.constant_buffer = rc->CreateBuffer(cb_desc, b::RESOURCE_HEAP_PROPERTY_FLAG_DEVICE_LOCAL);
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
            }
        }
    }

    return true;
}
bool HelloImGui::CreateConstantBufferView()
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
bool HelloImGui::CreateTextureResource()
{
    auto&& data_desc = texture.data->GetDesc();
    texture.texture = dr->GetResourceCreate()->CreateTexture(init::Tex2DResourceDesc({ (uint32_t)data_desc.width, (uint32_t)data_desc.height }
                                                                                     , tex::GetDefaultFormat(data_desc), init::TEX_STATIC_SRV_FLAGS, data_desc.num_mips, data_desc.depth)
                                                             , b::RESOURCE_HEAP_PROPERTY_FLAG_DEVICE_LOCAL);
    RET_IF_FAILED(texture.texture);

    return true;
}
bool HelloImGui::CopyDataToTexture()
{
    auto&& data_desc = texture.data->GetDesc();
    util::PipelineBarrierDesc bd{};
    util::TextureBarrierRange tex(&bd);
    tex .SetTexture(texture.texture->GetB3DTexture().Get())
        .AddSubresRange(b::TEXTURE_ASPECT_FLAG_COLOR, 0, 0, 1, texture.texture->GetB3DDesc().texture.mip_levels)
        .Finalize();

    copy_ctx.Begin();
    {
        bd.AddTextureBarrierRange(&tex.Get(), b::RESOURCE_STATE_UNDEFINED, b::RESOURCE_STATE_COPY_DST_WRITE);
        copy_ctx.PipelineBarrier(bd.SetPipelineStageFalgs(b::PIPELINE_STAGE_FLAG_TOP_OF_PIPE, b::PIPELINE_STAGE_FLAG_COPY_RESOLVE).Finalize().Get());

        for (size_t i = 0; i < data_desc.num_mips; i++)
        {
            auto&& tex_data = texture.data->Get(i);
            copy_ctx.CopyDataToTexture(texture.texture->GetB3DTexture().Get(), (uint32_t)i, 0
                                       , tex_data->layout.row_pitch
                                       , tex_data->extent.h
                                       , tex_data->total_size, tex_data->data);
        }

        bd.Reset();
        if (copy_ctx.GetCommandType() == b::COMMAND_TYPE_COPY_ONLY)
        {
            // 所有権をコピーキューから開放します。
            bd.AddTextureBarrierRange(&tex.Get(), b::RESOURCE_STATE_COPY_DST_WRITE, b::RESOURCE_STATE_SHADER_READ
                                      , b::RESOURCE_BARRIER_FLAG_OWNERSHIP_TRANSFER, b::COMMAND_TYPE_COPY_ONLY, b::COMMAND_TYPE_DIRECT);
            copy_ctx.PipelineBarrier(bd.SetPipelineStageFalgs(b::PIPELINE_STAGE_FLAG_COPY_RESOLVE, b::PIPELINE_STAGE_FLAG_TOP_OF_PIPE).Finalize().Get());
        }
        else
        {
            bd.AddTextureBarrierRange(&tex.Get(), b::RESOURCE_STATE_COPY_DST_WRITE, b::RESOURCE_STATE_SHADER_READ);
            copy_ctx.PipelineBarrier(bd.SetPipelineStageFalgs(b::PIPELINE_STAGE_FLAG_COPY_RESOLVE, b::PIPELINE_STAGE_FLAG_ALL_GRAPHICS).Finalize().Get());
        }
    }
    copy_ctx.End(copy_ctx.GetGpuWaitFence());
    BMR_RET_IF_FAILED(copy_ctx.WaitOnCpu());

    // コピーキューから開放された所有権をグラフィックキューで取得します。
    if (copy_ctx.GetCommandType() == b::COMMAND_TYPE_COPY_ONLY)
    {
        ctx.Begin();
        {
            bd.Reset();
            bd.AddTextureBarrierRange(&tex.Get(), b::RESOURCE_STATE_COPY_DST_WRITE, b::RESOURCE_STATE_SHADER_READ
                                      , b::RESOURCE_BARRIER_FLAG_OWNERSHIP_TRANSFER, b::COMMAND_TYPE_COPY_ONLY, b::COMMAND_TYPE_DIRECT);
            ctx.PipelineBarrier(bd.SetPipelineStageFalgs(b::PIPELINE_STAGE_FLAG_TOP_OF_PIPE, b::PIPELINE_STAGE_FLAG_ALL_GRAPHICS).Finalize().Get());
        }
        ctx.End(ctx.GetGpuWaitFence());
        BMR_RET_IF_FAILED(ctx.WaitOnCpu());
    }

    return true;
}
bool HelloImGui::CreateShaderResourceView()
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
bool HelloImGui::CreateSampler()
{
    b::SAMPLER_DESC sd{};
    sd.filter.mode            = b::SAMPLER_FILTER_MODE_ANISOTROPHIC;
    sd.filter.reduction_mode  = b::SAMPLER_FILTER_REDUCTION_MODE_STANDARD;
    sd.filter.max_anisotropy  = (uint32_t)dr->GetDeviceAdapterLimits().max_sampler_anisotropy;
    sd.filter.comparison_func = b::COMPARISON_FUNC_NEVER;
    sd.texture.address.u  = b::TEXTURE_ADDRESS_MODE_BORDER;
    sd.texture.address.v  = b::TEXTURE_ADDRESS_MODE_BORDER;
    sd.texture.address.w  = b::TEXTURE_ADDRESS_MODE_BORDER;
    sd.texture.sample.minification  = b::TEXTURE_SAMPLE_MODE_LINEAR;
    sd.texture.sample.magnification = b::TEXTURE_SAMPLE_MODE_LINEAR;
    sd.texture.sample.mip           = b::TEXTURE_SAMPLE_MODE_LINEAR;
    sd.mip_lod.min    = 0.f;
    sd.mip_lod.max    = FLT_MAX;
    sd.mip_lod.bias   = 0.f;
    sd.border_color   = b::BORDER_COLOR_TRANSPARENT_BLACK_FLOAT;

    auto bmr = device->CreateSampler(sd, &sampler);
    BMR_RET_IF_FAILED(bmr);

    return true;
}
bool HelloImGui::UpdateDescriptorSets()
{
    util::UpdateDescriptorSetDesc update_desc{};
    for (uint32_t i_frame = 0; i_frame < BACK_BUFFER_COUNT; i_frame++)
    {
        update_desc.AddNewWriteDescriptorSet()
            .SetDst(buffer_descriptor_sets[i_frame].Get())
            .AddNewWriteDescriptorBinding().SetNumDescriptors(1).SetDstBinding(0, 0).SetSrcView(0, frame_cbs[i_frame].model_cbv.Get()).Finalize()
            .AddNewWriteDescriptorBinding().SetNumDescriptors(1).SetDstBinding(1, 0).SetSrcView(0, frame_cbs[i_frame].scene_cbv.Get()).Finalize()
            .Finalize();
    }
    update_desc.AddNewWriteDescriptorSet()
        .SetDst(texture_descriptor_set.Get())
        .AddNewWriteDescriptorBinding().SetNumDescriptors(1).SetDstBinding(0, 0).SetSrcView(0, texture.srv.Get()).Finalize()
        .AddNewWriteDescriptorBinding().SetNumDescriptors(1).SetDstBinding(1, 0).SetSrcView(0, sampler.Get()).Finalize()
        .Finalize();

    update_desc.Finalize();

    auto bmr = device->CreateDescriptorUpdate({}, &descriptor_update);
    BMR_RET_IF_FAILED(bmr);

    bmr = descriptor_update->UpdateDescriptorSets(update_desc.Get());
    BMR_RET_IF_FAILED(bmr);

    return true;
}
bool HelloImGui::InitMyImGui()
{
    myimgui = std::make_unique<gui::MyImGui>(dr);
    gui::MYIMGUI_CREATE_DESC gui_cd{};

    auto data_win = static_cast<const b::SURFACE_PLATFORM_DATA_WINDOWS*>(swapchain->GetSurface()->GetDesc().platform_data.data);
    gui_cd.window_handle = data_win->hwnd;
    gui_cd.flags         = gui::MYIMGUI_CREATE_FLAG_DESCRIPTOR_POOL_FEEDING /*| gui::MYIMGUI_CREATE_FLAG_USE_SINGLE_COMMAND_LIST*/;

    gui_cd.config_flags           = ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable | ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableSetMousePos;
    gui_cd.framebuffer_format     = swapchain->GetBuffers()[0].tex->GetDesc().texture.format_desc.format;
    auto result = myimgui->Init(gui_cd);
    RET_IF_FAILED(result);

    result = CreateMyImGuiFramebuffers();
    RET_IF_FAILED(result);

    return true;
}
bool HelloImGui::CreateMyImGuiFramebuffers()
{
    for (uint32_t i = 0; i < BACK_BUFFER_COUNT; i++)
    {
        auto bmr = myimgui->CreateFramebuffer(back_buffers->data()[i].rtv.Get(), &myimgui_framebuffers[i]);
        BMR_RET_IF_FAILED(bmr);
    }

    return true;
}

void HelloImGui::PrepareFrame(uint32_t _buffer_index)
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

        l->SetPipelineLayout(b::PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout.Get());
        l->SetPipelineState(pipeline.Get());

        b::IDescriptorSet* sets[2] = { buffer_descriptor_sets[_buffer_index].Get(), texture_descriptor_set.Get() };
        b::CMD_BIND_DESCRIPTOR_SETS bind_sets{};
        bind_sets.first_set                         = 0;
        bind_sets.num_descriptor_sets               = 2;
        bind_sets.descriptor_sets                   = sets;
        bind_sets.num_dynamic_descriptor_offsets    = 0;
        bind_sets.dynamic_descriptor_offsets        = nullptr;
        l->BindDescriptorSets(b::PIPELINE_BIND_POINT_GRAPHICS, bind_sets);

        l->BindVertexBufferViews({ 0, 1, vertex_buffer_view.GetAddressOf() });

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
            l->Draw({ 3, 1, 0, 0 });
        }
        l->EndRenderPass({});
    }
    bmr = l->EndRecord();
    assert(bmr == b::BMRESULT_SUCCEED);
}

void HelloImGui::Tick()
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

void HelloImGui::Update()
{
    if (window->GetWindowStateFlags() & WINDOW_STATE_FLAG_MINIMIZED)
        return;

    // 次のバックバッファを取得
    MoveToNextFrame();

    {
        auto&& key = platform->GetInputs()->GetKey();
        if (key.F3.is_release)
            is_enabled_gui = !is_enabled_gui;

        if (is_enabled_gui)
        {
            myimgui->NewFrame();
            ImGui::ShowDemoWindow();

            if (ImGui::Begin("Custom image"))
            {
                ImGui::Image(texture.srv.Get(), { 512, 512 });
            }
            ImGui::End();
        }
    }

    {
        auto&& key = platform->GetInputs()->GetKey();
        g_cam.keys.up       = key.W.press_count;
        g_cam.keys.down     = key.S.press_count;
        g_cam.keys.left     = key.A.press_count;
        g_cam.keys.right    = key.D.press_count;

        auto&& mouse = platform->GetInputs()->GetMouse();
        auto dx = (float)mouse.x.delta;
        auto dy = (float)mouse.y.delta;
        auto wheel_delta = static_cast<float>(mouse.rot.delta) * 0.005f;
        if (mouse.buttons.left.press_count)     g_cam.rotate   (glm::vec3(dy * g_cam.rotationSpeed, dx * g_cam.rotationSpeed, 0.0f));
        if (mouse.buttons.right.press_count)    g_cam.translate(glm::vec3(-0.0f      , 0.0f        , dy * 0.005f));
        if (mouse.buttons.middle.press_count)   g_cam.translate(glm::vec3(dx * 0.005f, -dy * 0.005f, 0.0f       ));
        if (wheel_delta != 0.f)                 g_cam.translate(glm::vec3(0.0f       , 0.0f        , wheel_delta));

        g_cam.update(timer.GetElapsedSecondsF());
        cb_scene.view_proj = g_cam.matrices.perspective * g_cam.matrices.view;
    }

    {
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

void HelloImGui::MoveToNextFrame()
{
    uint32_t next_buffer_index = 0;
    auto bmr = swapchain->AcquireNextBuffer(UINT32_MAX, &next_buffer_index, true);
    assert(bmr == b::BMRESULT_SUCCEED || bmr == b::BMRESULT_SUCCEED_NOT_READY);

    back_buffer_index = next_buffer_index;
}

void HelloImGui::Render()
{
    if (window->GetWindowStateFlags() & WINDOW_STATE_FLAG_MINIMIZED)
        return;

    auto cmd_lists_data  = cmd_lists.data();
    auto cmd_fences_data = cmd_fences.data();
    b::BMRESULT bmr{};

    // 送信情報を準備
    {
        // 待機フェンス
        wait_fence_desc.Reset().AddFence(swapchain_fences->signal_fence.Get(), 0).Finalize();
        submit_info.wait_fence = wait_fence_desc.GetAsWait().wait_fence;

        // コマンドリスト
        submit_info.command_lists_to_execute = cmd_lists_data[back_buffer_index].GetAddressOf();

        // シグナルフェンス
        signal_fence_desc.Reset().AddFence(cmd_fences_data[back_buffer_index].Get(), fence_values[back_buffer_index].signal()).Finalize();
        submit_info.signal_fence = signal_fence_desc.GetAsSignal().signal_fence;
    }

    // コマンドリストとフェンスを送信
    {
        cmd_fences_data[back_buffer_index]->Wait(fence_values[back_buffer_index].wait(), UINT32_MAX);
        //PrepareFrame(back_buffer_index);

        bmr = command_queue->Submit(submit);
        assert(bmr == b::BMRESULT_SUCCEED);
    }

    if (is_enabled_gui)
    {
        myimgui->DrawGui(myimgui_framebuffers[back_buffer_index].Get(), b::RESOURCE_STATE_PRESENT, b::RESOURCE_STATE_PRESENT);
        myimgui->SubmitCommands();
        myimgui->PresentViewports();
    }

    // バックバッファをプレゼント
    {
        present_info.wait_fence = nullptr;
        bmr = swapchain->Present(present_info, true);
        assert(bmr == b::BMRESULT_SUCCEED);
    }

    fence_values[back_buffer_index]++;
}

void HelloImGui::OnResize(ResizeEventArgs* _args)
{
    command_queue->WaitIdle();
    framebuffers.clear();
    back_buffers = nullptr;

    for (auto& i : myimgui_framebuffers)
        i.Reset();
}

void HelloImGui::OnResized(BufferResizedEventArgs* _args)
{
    back_buffers     = &swapchain->GetBuffers();
    swapchain_fences = &swapchain->GetPresentCompleteFences();

    vpiewport       = {   0, 0  ,  (float)_args->size.width, (float)_args->size.height, b::B3D_VIEWPORT_MIN_DEPTH, b::B3D_VIEWPORT_MAX_DEPTH };
    scissor_rect    = { { 0, 0 },        {_args->size.width,        _args->size.height} };

    if ((_args->size.width > 0.0f) && (_args->size.height > 0.0f))
        g_cam.updateAspectRatio((float)_args->size.width / (float)_args->size.height);

    auto result = CreateFramebuffer();
    assert(result);
    for (size_t i = 0; i < BACK_BUFFER_COUNT; i++)
        PrepareFrame(i);

    result = CreateMyImGuiFramebuffers();
    assert(result);

    Update();
    Render();
}

void HelloImGui::OnProcessMessage(ProcessMessageEventArgs* _args)
{
    if (!is_enabled_gui)
        return;
    myimgui->OnProcessMessage(_args);
}

void HelloImGui::Term()
{
    dr->WaitForGpu();

    // result
    {
        float res = 0.f;
        float size = static_cast<float>(g_fpss->size());
        for (auto& i : *g_fpss)
            res += i;
        std::stringstream ss;
        ss << "prof result: average fps ";
        ss << (res / size) << std::endl;

        platform->GetLogger()->LogInfo(ss.str().c_str());
    }

    if (myimgui)
    {
        for (auto& i : myimgui_framebuffers)
            i.Reset();
        myimgui->Destroy();
        myimgui.reset();
    }

    // オブジェクトの解放
    for (auto& i : cmd_allocator)
        i->Reset(b::COMMAND_ALLOCATOR_RESET_FLAG_RELEASE_RESOURCES);
    cmd_lists = {};
    cmd_allocator = {};
    buffer_descriptor_sets = {};
    texture_descriptor_set.Reset();
    descriptor_pool.Reset();
    descriptor_heap.Reset();
    descriptor_update.Reset();
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
    vertex_buffer.reset();
    texture = {};
    pipeline.Reset();
    shader_modules = {};
    framebuffers = {};
    render_pass.Reset();
    pipeline_layout.Reset();
    buffer_layout.Reset();
    texture_layout.Reset();
    back_buffers = nullptr;
    swapchain.reset();
    command_queue.Reset();
    swapchain_fences = {};
    cmd_fences = {};
}


}// namespace buma
