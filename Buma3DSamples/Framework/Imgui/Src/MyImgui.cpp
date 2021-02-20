#include "MyImGui.h"

#include "./Utils.h"
#include "MyImguiRenderer.h"

#include "imgui.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef CreateWindow
#include <backends/imgui_impl_win32.h>

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include "Framework.h"

namespace /*anonymous*/
{

static const char* Imgui_VS_hlsl = R"(
struct VERTEX_BUFFER_CONSTANT
{
    float4x4 ProjectionMatrix;
};
[[vk::push_constant]] ConstantBuffer<VERTEX_BUFFER_CONSTANT> constant : register(b0, space0);


struct VS_INPUT
{
    float2 pos : POSITION;
    float2 uv  : TEXCOORD0;
    float4 col : COLOR0;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
    float2 uv  : TEXCOORD0;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    output.pos = mul(constant.ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f));
    output.col = input.col;
    output.uv  = input.uv;
    return output;
}

)";

static const char* Imgui_PS_hlsl = R"(
struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
    float2 uv  : TEXCOORD0;
};

Texture2D    texture0 : register(t0, space1);
SamplerState sampler0 : register(s0, space0);

float4 main(PS_INPUT input) : SV_Target
{
    return input.col * texture0.Sample(sampler0, input.uv);
}

)";


}//namespace /*anonymous*/

namespace b = buma3d;

namespace buma
{
namespace gui
{


class MyImGui::MyImGuiImpl
{
public:
    MyImGuiImpl(const std::shared_ptr<DeviceResources>& _dr)
        : desc      {}
        , ctx       {}
        , rr        {}
        , renderer  {}
    {
        rr.dr       = _dr;
        rr.device   = rr.dr->GetDevice();
        rr.queue    = _dr->GetCommandQueues(buma3d::COMMAND_TYPE_DIRECT)[0];
    }

    ~MyImGuiImpl()
    {

    }

    bool Init(const MYIMGUI_CREATE_DESC& _desc);
    bool CreateSampler();
    bool BuildTextureAtlas();
    void CopyDataToTexture(size_t _aligned_row_pitch, size_t _texture_height, void* _mem);
    bool CreateShaderResourceView();
    bool CreateRootSignature();
    bool CreateRenderPass();
    bool CreateShaderModules();
    bool CreateGraphicsPipelines();
    bool CreateFontDescriptorPool();
    bool AllocateFontDescriptorSet();
    bool WriteFontSrv();

    void OnProcessMessage(ProcessMessageEventArgs* _args);

    buma3d::BMRESULT CreateFramebuffer(buma3d::IView* _rtv, buma3d::IFramebuffer** _dst);

    RENDER_RESOURCE& GetRenderResource() { return rr; }

    void NewFrame();
    void DrawGui(buma3d::IFramebuffer* _framebuffer, buma3d::RESOURCE_STATE _current_state, buma3d::RESOURCE_STATE _state_after);
    void RecordGuiCommands(buma3d::ICommandList* _list, buma3d::IFramebuffer* _framebuffer, buma3d::RESOURCE_STATE _current_state, buma3d::RESOURCE_STATE _state_after);
    void SubmitCommands();
    void PresentViewports();
    void Destroy();

    // (Optional) Platform functions (e.g. Win32, GLFW, SDL2)
    //   N = ImGui::NewFrame()                        ~ beginning of the dear imgui frame: read info from platform/OS windows (latest size/position)
    //   F = ImGui::Begin(), ImGui::EndFrame()        ~ during the dear imgui frame
    //   U = ImGui::UpdatePlatformWindows()           ~ after the dear imgui frame: create and update all platform/OS windows
    //   R = ImGui::RenderPlatformWindowsDefault()    ~ render
    //   D = ImGui::DestroyPlatformWindows()          ~ shutdown
    // 一般的な考え方は、NewFrame()が現在のPlatform/OSの状態を読み取り、UpdatePlatformWindows()がそれに書き込むというものです。
    // 
    // 関数は、2つのimgui_impl_xxxxファイルを組み合わせることができるように設計されています。1つはプラットフォーム（〜ウィンドウ/入力処理）用、もう1つはレンダラー用です。
    // カスタムエンジンバックエンドは、多くの場合、プラットフォームインターフェイスとレンダラーインターフェイスの両方を提供するため、すべての機能を使用する必要はありません。
    // プラットフォーム関数は通常、他の方法で呼び出されるDestroyを除いて、対応するレンダラーの前に呼び出されます。
    // 
    // CreateWindow  : // . . U . . スワップチェーン、フレームバッファーなどを作成します（Platform_CreateWindowの後に呼び出されます）
    // DestroyWindow : // N . U . D スワップチェーン、フレームバッファなどを破棄します（Platform_DestroyWindowの前に呼び出されます）
    // SetWindowSize : // . . U . . スワップチェーン、フレームバッファーなどのサイズを変更します（Platform_SetWindowSizeの後に呼び出されます）
    // RenderWindow  : // . . . R . （オプション）フレームバッファをクリアし、レンダーターゲットを設定してから、ビューポート-> DrawDataをレンダリングします。 'render_arg'は、RenderPlatformWindowsDefault()に渡される値です。
    // SwapBuffers   : // . . . R . （オプション）Present / SwapBuffersを呼び出します。 'render_arg'は、RenderPlatformWindowsDefault()に渡される値です。

    static void CreateWindow (ImGuiViewport* _vp);
    static void DestroyWindow(ImGuiViewport* _vp);
    static void SetWindowSize(ImGuiViewport* _vp, ImVec2 _size);
    static void RenderWindow (ImGuiViewport* _vp, void* _render_arg);
    static void SwapBuffers  (ImGuiViewport* _vp, void* _render_arg);

private:
    bool CreateContext(ImGuiConfigFlags _config_flags);

public:
    MYIMGUI_CREATE_DESC                 desc;
    ImGuiContext*                       ctx;
    RENDER_RESOURCE                     rr;
    std::unique_ptr<MyImGuiRenderer>    renderer;

};

void MyImGui::MyImGuiImpl::CreateWindow(ImGuiViewport* _vp)
{
    auto self = static_cast<MyImGui::MyImGuiImpl*>(ImGui::GetIO().UserData);
    auto renderer = new MyImGuiViewportRenderer(self->rr);
    _vp->RendererUserData = renderer;
    auto result = renderer->CreateWindow(_vp);
    assert(result);
}
void MyImGui::MyImGuiImpl::DestroyWindow(ImGuiViewport* _vp)
{
    if (!_vp->RendererUserData)
        return;
    auto renderer = static_cast<MyImGuiViewportRenderer*>(_vp->RendererUserData);
    auto result = renderer->DestroyWindow(_vp);
    assert(result);

    delete renderer;
    _vp->RendererUserData = nullptr;
}
void MyImGui::MyImGuiImpl::SetWindowSize(ImGuiViewport* _vp, ImVec2 _size)
{
    if (!_vp->RendererUserData)
        return;
    auto renderer = static_cast<MyImGuiViewportRenderer*>(_vp->RendererUserData);
    auto result = renderer->SetWindowSize(_vp, _size);
    assert(result);
}
void MyImGui::MyImGuiImpl::RenderWindow(ImGuiViewport* _vp, void* _render_arg)
{
    if (!_vp->RendererUserData)
        return;
    auto renderer = static_cast<MyImGuiViewportRenderer*>(_vp->RendererUserData);
    auto result = renderer->RenderWindow(_vp, _render_arg);
    assert(result);
}
void MyImGui::MyImGuiImpl::SwapBuffers(ImGuiViewport* _vp, void* _render_arg)
{
    if (!_vp->RendererUserData)
        return;
    auto renderer = static_cast<MyImGuiViewportRenderer*>(_vp->RendererUserData);
    auto result = renderer->SwapBuffers(_vp, _render_arg);
    assert(result);
}


bool MyImGui::MyImGuiImpl::Init(const MYIMGUI_CREATE_DESC& _desc)
{
    desc = _desc;
    if (!CreateContext(desc.config_flags))
        return false;

    if (!ImGui_ImplWin32_Init(desc.window_handle))
        return false;

    RET_IF_FAILED(CreateSampler());
    RET_IF_FAILED(BuildTextureAtlas());
    RET_IF_FAILED(CreateShaderResourceView());
    RET_IF_FAILED(CreateRootSignature());
    RET_IF_FAILED(CreateRenderPass());
    RET_IF_FAILED(CreateShaderModules());
    RET_IF_FAILED(CreateGraphicsPipelines());
    RET_IF_FAILED(CreateFontDescriptorPool());
    RET_IF_FAILED(AllocateFontDescriptorSet());
    RET_IF_FAILED(WriteFontSrv());
    rr.flags = desc.flags;

    renderer = std::make_unique<MyImGuiRenderer>(rr, /*viewport*/false, /*primary*/true);
    return true;
}

bool MyImGui::MyImGuiImpl::CreateSampler()
{
    b::SAMPLER_DESC sd{};
    sd.filter.mode            = b::SAMPLER_FILTER_MODE_STANDARD;
    sd.filter.reduction_mode  = b::SAMPLER_FILTER_REDUCTION_MODE_STANDARD;
    sd.filter.max_anisotropy  = 0;
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
    sd.border_color   = b::BORDER_COLOR_OPAQUE_BLACK_FLOAT;

    auto bmr = rr.device->CreateSampler(sd, &rr.sampler);
    BMR_RET_IF_FAILED(bmr);
    rr.sampler->SetName("MyImGui::MyImGuiImpl::rr.sampler");

    return true;
}
bool MyImGui::MyImGuiImpl::BuildTextureAtlas()
{
    auto&& io = ImGui::GetIO();

    ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\Cica-Regular.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    IM_ASSERT(font != NULL);

    // アライメントのためにpixel_dataのデータをmemにコピー
    unsigned char* pixel_data{};
    void* mem{};
    int width{}, height{};
    io.Fonts->GetTexDataAsRGBA32(&pixel_data, &width, &height);
    size_t row_pitch, slice_pitch, texture_height, aligned_row_pitch;
    {
        GetSurfaceInfo(width, height, buma3d::RESOURCE_FORMAT_R8G8B8A8_UNORM, &slice_pitch, &row_pitch, &texture_height);
        aligned_row_pitch  = util::AlignUp(row_pitch, rr.dr->GetDeviceAdapterLimits().buffer_copy_row_pitch_alignment);

        mem = malloc(aligned_row_pitch * texture_height);
        assert(mem);
        std::fill((uint8_t*)mem, (uint8_t*)mem + (aligned_row_pitch * texture_height), 0);

        for (size_t i = 0; i < texture_height; i++)
            memcpy(((uint8_t*)mem) + (aligned_row_pitch * i), ((uint8_t*)pixel_data) + (row_pitch * i), row_pitch);
    }

    // テクスチャリソース作成
    {
        auto font_texture_desc = buma3d::hlp::init::Tex2DResourceDesc({ (uint32_t)width, (uint32_t)height }, buma3d::RESOURCE_FORMAT_R8G8B8A8_UNORM);
        font_texture_desc.texture.mip_levels = 1;
        rr.font_texture = rr.dr->GetResourceCreate()->CreateTexture(font_texture_desc, buma3d::RESOURCE_HEAP_PROPERTY_FLAG_DEVICE_LOCAL);
        rr.font_texture->SetName("MyImGui::MyImGuiImpl::rr.font_texture");
    }

    // データをコピー
    CopyDataToTexture(aligned_row_pitch, texture_height, mem);

    free(mem);
    return true;
}
void MyImGui::MyImGuiImpl::CopyDataToTexture(size_t _aligned_row_pitch, size_t _texture_height, void* _mem)
{
    auto&& graphics_queue = rr.dr->GetCommandQueues(buma3d::COMMAND_TYPE_DIRECT);
    DeferredContext ctx;
    ctx.Init(rr.dr, graphics_queue[0]);

    ctx.Begin();
    {
        util::TextureBarrierRange br{};
        util::PipelineBarrierDesc pb{};
        auto ft = rr.font_texture->GetB3DTexture().Get();
        br.AddSubresRange(buma3d::TEXTURE_ASPECT_FLAG_COLOR, 0, 0);

        pb.AddTextureBarrier(br.Get(ft), buma3d::RESOURCE_STATE_UNDEFINED, buma3d::RESOURCE_STATE_COPY_DST_WRITE);
        ctx.PipelineBarrier(pb.Get(buma3d::PIPELINE_STAGE_FLAG_TOP_OF_PIPE, buma3d::PIPELINE_STAGE_FLAG_COPY_RESOLVE));

        ctx.CopyDataToTexture(rr.font_texture->GetB3DTexture().Get(), 0, 0, _aligned_row_pitch, _texture_height, _aligned_row_pitch * _texture_height, _mem);
        //ctx.GenerateMips(*font_texture);

        pb.Reset();
        pb.AddTextureBarrier(br.Get(ft), buma3d::RESOURCE_STATE_COPY_DST_WRITE, buma3d::RESOURCE_STATE_SHADER_READ);
        ctx.PipelineBarrier(pb.Get(buma3d::PIPELINE_STAGE_FLAG_COPY_RESOLVE, buma3d::PIPELINE_STAGE_FLAG_PIXEL_SHADER));
    }
    ctx.End(ctx.GetGpuWaitFence());
    ctx.WaitOnCpu();
}
bool MyImGui::MyImGuiImpl::CreateShaderResourceView()
{
    buma3d::SHADER_RESOURCE_VIEW_DESC srvd{};
    srvd.view.type          = b::VIEW_TYPE_SHADER_RESOURCE;
    srvd.view.format        = rr.font_texture->GetB3DDesc().texture.format_desc.format;
    srvd.view.dimension     = b::VIEW_DIMENSION_TEXTURE_2D;
    srvd.texture.components                             = { b::COMPONENT_SWIZZLE_IDENTITY, b::COMPONENT_SWIZZLE_IDENTITY, b::COMPONENT_SWIZZLE_IDENTITY, b::COMPONENT_SWIZZLE_IDENTITY };
    srvd.texture.subresource_range.offset.aspect        = b::TEXTURE_ASPECT_FLAG_COLOR;
    srvd.texture.subresource_range.offset.mip_slice     = 0;
    srvd.texture.subresource_range.offset.array_slice   = 0;
    srvd.texture.subresource_range.mip_levels           = b::B3D_USE_REMAINING_MIP_LEVELS;
    srvd.texture.subresource_range.array_size           = 1;
    srvd.flags                                          = b::SHADER_RESOURCE_VIEW_FLAG_DENY_INPUT_ATTACHMENT;

    auto bmr = rr.device->CreateShaderResourceView(rr.font_texture->GetB3DTexture().Get(), srvd, &rr.font_srv);
    BMR_RET_IF_FAILED(bmr);
    rr.font_srv->SetName("MyImGui::MyImGuiImpl::rr.font_srv");

    return true;
}
bool MyImGui::MyImGuiImpl::CreateRootSignature()
{
    util::RootSignatureDesc rootsig_desc{};
    {        
        auto&& matrix_cb = rootsig_desc.AddNewRootParameter();
        matrix_cb.InitAsPush32BitConstants(sizeof(float[4][4]) / 4, 0, 0);
        matrix_cb.SetShaderVisibility(buma3d::SHADER_VISIBILITY_VERTEX);

        rootsig_desc.SetRegisterShift(buma3d::SHADER_REGISTER_TYPE_T, 1, 1);
        auto&& font_or_user_texture = rootsig_desc.AddNewRootParameter();
        font_or_user_texture.InitAsDescriptorTable();
        font_or_user_texture.AddRange(buma3d::DESCRIPTOR_TYPE_SRV_TEXTURE, 1, 0, 1, buma3d::DESCRIPTOR_FLAG_NONE);// フォントまたはImGui::Image用
        font_or_user_texture.SetShaderVisibility(buma3d::SHADER_VISIBILITY_PIXEL);

        rootsig_desc.SetRegisterShift(buma3d::SHADER_REGISTER_TYPE_S, 2, 0);
        rootsig_desc.AddStaticSampler(0, 0, rr.sampler.Get(), buma3d::SHADER_VISIBILITY_PIXEL);
    }

    auto bmr = rr.device->CreateRootSignature(rootsig_desc.Get(buma3d::ROOT_SIGNATURE_FLAG_NONE), &rr.root_signature);
    BMR_RET_IF_FAILED(bmr);
    rr.root_signature->SetName("MyImGui::MyImGuiImpl::rr.root_signature");

    return true;
}
bool MyImGui::MyImGuiImpl::CreateRenderPass()
{
    b::RENDER_PASS_DESC render_pass_desc{};

    b::ATTACHMENT_DESC attachment{};
    attachment.flags        = b::ATTACHMENT_FLAG_NONE;
    attachment.format       = desc.framebuffer_format;
    attachment.sample_count = 1;
    attachment.load_op      = b::ATTACHMENT_LOAD_OP_LOAD;
    attachment.store_op     = b::ATTACHMENT_STORE_OP_STORE;
    attachment.begin_state  = b::RESOURCE_STATE_COLOR_ATTACHMENT_READ_WRITE;
    attachment.end_state    = b::RESOURCE_STATE_COLOR_ATTACHMENT_READ_WRITE;

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

    auto bmr = rr.device->CreateRenderPass(render_pass_desc, &rr.render_pass_load);
    BMR_RET_IF_FAILED(bmr);
    rr.render_pass_load->SetName("MyImGui::MyImGuiImpl::rr.render_pass_load");

    return true;
}
bool MyImGui::MyImGuiImpl::CreateShaderModules()
{
    b::BMRESULT bmr{};
    shader::LOAD_SHADER_DESC desc{};
    desc.options.pack_matrices_in_row_major = false;       // Experimental: Decide how a matrix get packed
    desc.options.enable16bit_types          = false;       // Enable 16-bit types, such as half, uint16_t. Requires shader model 6.2+
    desc.options.enable_debug_info          = false;       // Embed debug info into the binary
    desc.options.disable_optimizations      = false;       // Force to turn off optimizations. Ignore optimizationLevel below.

    desc.options.optimization_level         = 3; // 0 to 3, no optimization to most optimization
    desc.options.shader_model               = { 6, 2 };

    desc.options.shift_all_cbuf_bindings    = 1;// register(t0, space0) -> register(t1, space0)
    desc.options.shift_all_cbuf_bindings    = 2;// register(s0, space0) -> register(s2, space0)
    desc.options.shift_all_cbuf_bindings    = 0;
    desc.options.shift_all_cbuf_bindings    = 0;

    auto&& loader = rr.dr->GetShaderLoader();
    // vs
    {
        desc.entry_point    = "main";
        desc.filename       = nullptr;
        desc.defines        = {};
        desc.stage          = { shader::SHADER_STAGE_VERTEX };
        std::vector<uint8_t> bytecode;
        loader->LoadShaderFromHLSLString(desc, Imgui_VS_hlsl, &bytecode);
        assert(!bytecode.empty());

        b::SHADER_MODULE_DESC module_desc{};
        module_desc.flags                    = b::SHADER_MODULE_FLAG_NONE;
        module_desc.bytecode.bytecode_length = bytecode.size();
        module_desc.bytecode.shader_bytecode = bytecode.data();
        bmr = rr.device->CreateShaderModule(module_desc, &rr.vs);
        BMR_RET_IF_FAILED(bmr);
    }

    // ps
    {
        desc.entry_point    = "main";
        desc.filename       = nullptr;
        desc.defines        = {};
        desc.stage          = { shader::SHADER_STAGE_PIXEL };
        std::vector<uint8_t> bytecode;
        loader->LoadShaderFromHLSLString(desc, Imgui_PS_hlsl, &bytecode);
        assert(!bytecode.empty());

        b::SHADER_MODULE_DESC module_desc{};
        module_desc.flags                    = b::SHADER_MODULE_FLAG_NONE;
        module_desc.bytecode.bytecode_length = bytecode.size();
        module_desc.bytecode.shader_bytecode = bytecode.data();
        bmr = rr.device->CreateShaderModule(module_desc, &rr.ps);
        BMR_RET_IF_FAILED(bmr);
    }

    return true;
}
bool MyImGui::MyImGuiImpl::CreateGraphicsPipelines()
{
    b::BMRESULT bmr{};
    b::GRAPHICS_PIPELINE_STATE_DESC pso_desc{};

    pso_desc.root_signature       = rr.root_signature.Get();
    pso_desc.render_pass          = rr.render_pass_load.Get();
    pso_desc.subpass              = 0;
    pso_desc.node_mask            = b::B3D_DEFAULT_NODE_MASK;
    pso_desc.flags                = b::PIPELINE_STATE_FLAG_NONE;

    b::PIPELINE_SHADER_STAGE_DESC shader_stages[2]{};
    {
        shader_stages[0].stage            = b::SHADER_STAGE_FLAG_VERTEX;
        shader_stages[0].entry_point_name = "main";
        shader_stages[0].flags            = b::PIPELINE_SHADER_STAGE_FLAG_NONE;
        shader_stages[0].module           = rr.vs.Get();

        shader_stages[1].stage            = b::SHADER_STAGE_FLAG_PIXEL;
        shader_stages[1].entry_point_name = "main";
        shader_stages[1].flags            = b::PIPELINE_SHADER_STAGE_FLAG_NONE;
        shader_stages[1].module           = rr.ps.Get();

        pso_desc.num_shader_stages    = 2;
        pso_desc.shader_stages        = shader_stages;
    }

    // 入力レイアウト
    b::INPUT_LAYOUT_DESC  input_layout{};
    b::INPUT_SLOT_DESC    input_slot{};
    b::INPUT_ELEMENT_DESC input_elements[3]{};
    {
        //                  { semantic_name, semantic_index, format                             , aligned_byte_offset           }
        input_elements[0] = { "POSITION"   , 0             , b::RESOURCE_FORMAT_R32G32_FLOAT    , b::B3D_APPEND_ALIGNED_ELEMENT };
        input_elements[1] = { "TEXCOORD"   , 0             , b::RESOURCE_FORMAT_R32G32_FLOAT    , b::B3D_APPEND_ALIGNED_ELEMENT };
        input_elements[2] = { "COLOR"      , 0             , b::RESOURCE_FORMAT_R8G8B8A8_UNORM  , b::B3D_APPEND_ALIGNED_ELEMENT };

        input_slot.slot_number              = 0;
        input_slot.stride_in_bytes          = sizeof(ImDrawVert);
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
        rs.fill_mode                        = b::FILL_MODE_SOLID;
        rs.cull_mode                        = b::CULL_MODE_NONE;
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
        attachments.is_enabled_blend    = true;
        attachments.src_blend           = b::BLEND_FACTOR_SRC_ALPHA;
        attachments.dst_blend           = b::BLEND_FACTOR_SRC_ALPHA_INVERTED;
        attachments.blend_op            = b::BLEND_OP_ADD;
        attachments.src_blend_alpha     = b::BLEND_FACTOR_SRC_ALPHA_INVERTED;
        attachments.dst_blend_alpha     = b::BLEND_FACTOR_ZERO;
        attachments.blend_op_alpha      = b::BLEND_OP_ADD;
        attachments.color_write_mask    = b::COLOR_WRITE_FLAG_ALL;

        bs.is_enabled_independent_blend = false;
        bs.is_enabled_logic_op          = false;
        bs.logic_op                     = b::LOGIC_OP_SET;
        bs.num_attachments              = 1;
        bs.attachments                  = &attachments;
        bs.blend_constants              = { 0.f, 0.f, 0.f, 0.f };

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

    bmr = rr.device->CreateGraphicsPipelineState(pso_desc, &rr.pipeline_state_load);
    BMR_RET_IF_FAILED(bmr);
    rr.pipeline_state_load->SetName("MyImGui::MyImGuiImpl::rr.pipeline_state_load");

    return true;
}
bool MyImGui::MyImGuiImpl::CreateFontDescriptorPool()
{
    b::DESCRIPTOR_POOL_DESC dpd{};
    dpd.flags                       = b::DESCRIPTOR_POOL_FLAG_NONE;
    dpd.max_sets_allocation_count   = 1;
    dpd.max_num_register_space      = 2;

    b::DESCRIPTOR_POOL_SIZE size{ b::DESCRIPTOR_TYPE_SRV_TEXTURE, 1 };
    dpd.num_pool_sizes              = 1;
    dpd.pool_sizes                  = &size;
    dpd.node_mask                   = b::B3D_DEFAULT_NODE_MASK;

    auto bmr = rr.device->CreateDescriptorPool(dpd, &rr.font_pool);
    BMR_RET_IF_FAILED(bmr);
    rr.font_pool->SetName("MyImGui::MyImGuiImpl::rr.font_pool");

    return true;
}
bool MyImGui::MyImGuiImpl::AllocateFontDescriptorSet()
{
    auto bmr = rr.font_pool->AllocateDescriptorSet(rr.root_signature.Get(), &rr.font_set);
    BMR_RET_IF_FAILED(bmr);
    rr.font_set->SetName("MyImGui::MyImGuiImpl::rr.font_set");

    return true;
}
bool MyImGui::MyImGuiImpl::WriteFontSrv()
{
    util::UpdateDescriptorSetDesc update_desc{};
    {
        auto&& write_set   = update_desc.AddNewWriteDescriptorSets();

        auto&& write_table = write_set.AddNewWriteDescriptorTable();
        write_table.AddNewWriteDescriptorRange().SetDstRange(0, 0, 1).SetSrcView(0, rr.font_srv.Get());
        write_table.Finalize(1);

        write_set.Finalize(rr.font_set.Get());
    }
    update_desc.Finalize();
    auto bmr = rr.device->UpdateDescriptorSets(update_desc.Get());
    BMR_RET_IF_FAILED(bmr);

    return true;
}

void MyImGui::MyImGuiImpl::OnProcessMessage(ProcessMessageEventArgs* _args)
{
    auto data = reinterpret_cast<const ProcessMessageEventArgs::PROCESS_MESSAGE_EVENT_ARGS_WINDOWS*>(_args->data);
    ImGui_ImplWin32_WndProcHandler((HWND)data->hwnd, data->message, data->wparam, data->lparam);
}

buma3d::BMRESULT MyImGui::MyImGuiImpl::CreateFramebuffer(buma3d::IView* _rtv, buma3d::IFramebuffer** _dst)
{
    b::FRAMEBUFFER_DESC fbd{ b::FRAMEBUFFER_FLAG_NONE, rr.render_pass_load.Get() };
    fbd.num_attachments = 1;
    fbd.attachments     = &_rtv;
    return rr.device->CreateFramebuffer(fbd, _dst);
}

void MyImGui::MyImGuiImpl::NewFrame()
{
    rr.barriers.Reset();
    rr.submit.Reset();
    ImGui::SetCurrentContext(ctx);
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}
void MyImGui::MyImGuiImpl::DrawGui(buma3d::IFramebuffer* _framebuffer, buma3d::RESOURCE_STATE _current_state, buma3d::RESOURCE_STATE _state_after)
{
    renderer->BeginRecord();
    RecordGuiCommands(renderer->GetCommandList(), _framebuffer, _current_state, _state_after);
    renderer->EndRecord();
}
void MyImGui::MyImGuiImpl::RecordGuiCommands(buma3d::ICommandList* _list, buma3d::IFramebuffer* _framebuffer, buma3d::RESOURCE_STATE _current_state, buma3d::RESOURCE_STATE _state_after)
{
    ImGui::Render();

    if (auto draw_data = ImGui::GetDrawData())
    {
        ImGuiIO& io = ImGui::GetIO();
        //if (draw_data->CmdListsCount != 0)
        {
            renderer->BeginRecord(_list);
            renderer->RecordGuiCommands(_framebuffer, _current_state, _state_after, draw_data);
            renderer->EndRecord(_list);
        }
    }

    if (desc.config_flags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();

        auto viewport_list = desc.flags & MYIMGUI_CREATE_FLAG_USE_SINGLE_COMMAND_LIST ? _list : nullptr/*renderer固有のコマンドリストが使用されます。*/;
        ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
        for (int i = 1; i < platform_io.Viewports.Size; i++)
        {
            ImGuiViewport* viewport = platform_io.Viewports[i];
            if (viewport->Flags & ImGuiViewportFlags_Minimized)
                continue;
            if (platform_io.Platform_RenderWindow) platform_io.Platform_RenderWindow(viewport, nullptr);
            if (platform_io.Renderer_RenderWindow) platform_io.Renderer_RenderWindow(viewport, viewport_list);
        }
    }
}

void MyImGui::MyImGuiImpl::SubmitCommands()
{
    renderer->AddSubmissionTo(&rr.submit.AddNewSubmitInfo());

    if (desc.config_flags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
        for (int i = 1; i < platform_io.Viewports.Size; i++)
        {
            ImGuiViewport* viewport = platform_io.Viewports[i];
            if (viewport->Flags & ImGuiViewportFlags_Minimized)
                continue;
            auto renderer = static_cast<MyImGuiViewportRenderer*>(viewport->RendererUserData);
            renderer->AddSubmitInfoTo(&rr.submit);
        }
    }
    auto bmr = rr.queue->Submit(rr.submit.Get());
    assert(util::IsSucceeded(bmr));
}

void MyImGui::MyImGuiImpl::PresentViewports()
{
    if (desc.config_flags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
        for (int i = 1; i < platform_io.Viewports.Size; i++)
        {
            ImGuiViewport* viewport = platform_io.Viewports[i];
            if (viewport->Flags & ImGuiViewportFlags_Minimized)
                continue;
            if (platform_io.Platform_SwapBuffers) platform_io.Platform_SwapBuffers(viewport, nullptr);
            if (platform_io.Renderer_SwapBuffers) platform_io.Renderer_SwapBuffers(viewport, nullptr);
        }
    }
}

void MyImGui::MyImGuiImpl::Destroy()
{
    renderer.reset();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext(ctx);
    ctx = nullptr;
    rr = {};
}

bool MyImGui::MyImGuiImpl::CreateContext(ImGuiConfigFlags _config_flags)
{
    // Setup Dear ImGui context
    if (IMGUI_CHECKVERSION() == false)
        return false;

    ctx = ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.UserData = this;

    io.ConfigFlags  = _config_flags;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    desc.config_flags = io.ConfigFlags;

    ImGui::StyleColorsLight();

    // ビューポートが有効になっている場合は、WindowRounding/WindowBgを微調整して、プラットフォームウィンドウが通常のウィンドウと同じように見えるようにします。
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup backend capabilities flags
    io.BackendRendererName = "MyImGuiRenderer";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.
    io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;  // We can create multi-viewports on the Renderer side (optional) // FIXME-VIEWPORT: Actually unfinished..

    auto&& pio = ImGui::GetPlatformIO();
    pio.Renderer_CreateWindow   = CreateWindow;
    pio.Renderer_DestroyWindow  = DestroyWindow;
    pio.Renderer_SetWindowSize  = SetWindowSize;
    pio.Renderer_RenderWindow   = RenderWindow;
    pio.Renderer_SwapBuffers    = SwapBuffers;

    return true;
}


MyImGui::MyImGui(const std::shared_ptr<DeviceResources>& _dr)
    : impl{}
{
    impl = new MyImGuiImpl(_dr);
}

MyImGui::~MyImGui()
{
    delete impl;
    impl = nullptr;
}

bool MyImGui::Init(const MYIMGUI_CREATE_DESC& _desc)
{
    return impl->Init(_desc);
}

void MyImGui::OnProcessMessage(ProcessMessageEventArgs* _args)
{
    impl->OnProcessMessage(_args);
}

buma3d::BMRESULT MyImGui::CreateFramebuffer(buma3d::IView* _rtv, buma3d::IFramebuffer** _dst)
{
    return impl->CreateFramebuffer(_rtv, _dst);
}

void MyImGui::NewFrame()
{
    impl->NewFrame();
}

void MyImGui::DrawGui(buma3d::IFramebuffer* _framebuffer, buma3d::RESOURCE_STATE _current_state, buma3d::RESOURCE_STATE _state_after)
{
    impl->DrawGui(_framebuffer, _current_state, _state_after);
}

void MyImGui::RecordGuiCommands(buma3d::ICommandList* _list, buma3d::IFramebuffer* _framebuffer, buma3d::RESOURCE_STATE _current_state, buma3d::RESOURCE_STATE _state_after)
{
    assert(impl->desc.flags & MYIMGUI_CREATE_FLAG_DESCRIPTOR_POOL_FEEDING);
    impl->RecordGuiCommands(_list, _framebuffer, _current_state, _state_after);
}

void MyImGui::SubmitCommands()
{
    impl->SubmitCommands();
}

void MyImGui::PresentViewports()
{
    impl->PresentViewports();
}

void MyImGui::Destroy()
{
    impl->Destroy();
}


}// gui
}// buma
