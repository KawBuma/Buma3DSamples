#include "MyImgui.h"

namespace /*anonymous*/
{

static const char* Imgui_PS_hlsl = R"(
struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
    float2 uv  : TEXCOORD0;
};

SamplerState sampler0 : register(s0);
Texture2D texture0 : register(t0);

float4 main(PS_INPUT input) : SV_Target
{
    float4 out_col = input.col * texture0.Sample(sampler0, input.uv);
    return out_col;
}
)";

static const char* Imgui_VS_hlsl = R"(
cbuffer vertexBuffer : register(b0)
{
    float4x4 ProjectionMatrix;
};

struct VS_INPUT
{
    float2 pos : POSITION;
    float4 col : COLOR0;
    float2 uv  : TEXCOORD0;
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
    output.pos = mul(ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f));
    output.col = input.col;
    output.uv  = input.uv;
    return output;
}
)";


}//namespace /*anonymous*/

#include "MyImGui.h"

// Include compiled shaders for ImGui.
#include "./shader/ImGUI_VS.h"
#include "shader/ImGUI_PS.h"

#include <backends/imgui_impl_dx12.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_win32.h>

#include <fstream>
#include <cereal/cereal.hpp>    

// Root parameters for the ImGui root signature.
enum RootParameters
{
    MatrixCB,           // cbuffer vertexBuffer : register(b0)
    FontTexture,        // Texture2D texture0 : register(t0);
    NumRootParameters
};

//--------------------------------------------------------------------------------------
// Get surface information for a particular format
//--------------------------------------------------------------------------------------
void GetSurfaceInfo
(
    _In_ size_t         _width
    , _In_ size_t       _height
    , _In_ DXGI_FORMAT  _fmt
    , size_t*           _out_num_bytes
    , _Out_opt_ size_t* _out_row_bytes
    , _Out_opt_ size_t* _out_num_rows
);


namespace buma
{

MyImGui::MyImGui() : p_imgui_ctx(nullptr)
{
}

MyImGui::~MyImGui()
{
    Destroy();
}

bool MyImGui::Init(const ImGuiConfigFlags& _config_flags)
{
    if (CreateContext(_config_flags) == false)
        return false;

    ImGui::SetCurrentContext(p_imgui_ctx);
    if (ImGui_ImplWin32_Init(d3d::GetDeviceResources()->GetHWND()) == false)
        return false;

    LoadStyle(L"./Data/Other/imgui_style.bin");

    ImGuiIO& io = ImGui::GetIO();
    auto dr = buma::d3d::GetDeviceResources();
    Microsoft::WRL::ComPtr<ID3D12Device> device = dr->GetDevice();
    Microsoft::WRL::ComPtr<ID3D12Device2> device2;
    device.As(&device2);

    auto cmd_queue = dr->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
    auto cmd_list = cmd_queue->GetCommandList();

    // Build texture atlas
    {
        // ImGui側でフォントイメージを作成
        unsigned char* pixel_data = nullptr;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixel_data, &width, &height);

        size_t row_pitch, slice_pitch;
        GetSurfaceInfo(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, &slice_pitch, &row_pitch, nullptr);

    #if 0 // use DirectXTex

        // DirectXTexでのミップ生成用データ
        DirectX::Image            img{};
        DirectX::ScratchImage    scimg{};
        DirectX::ScratchImage    mipchain{};
        img.width = width;
        img.height = height;
        img.format = DXGI_FORMAT_R8G8B8A8_UNORM;
        img.rowPitch = row_pitch;
        img.slicePitch = slice_pitch;
        img.pixels = pixel_data;
        scimg.InitializeFromImage(img);

        // ミップ生成
        HRESULT hr = DirectX::GenerateMipMaps(scimg.GetImages(), scimg.GetImageCount(), scimg.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0/*0の場合解像度が1x1になるまでのミップ数*/, mipchain);
        ASSERT_HR(hr);

        // テクスチャとミップの情報をサブリソースのvectorにコピー
        std::vector<D3D12_SUBRESOURCE_DATA> subresources;
        hr = DirectX::PrepareUpload(device.Get(), mipchain.GetImages(), mipchain.GetImageCount(), mipchain.GetMetadata(), subresources);
        ASSERT_HR(hr);

        // リソースを作成
        Microsoft::WRL::ComPtr<ID3D12Resource> res;
        hr = DirectX::CreateTexture(device.Get(), mipchain.GetMetadata(), &res);
        ASSERT_HR(hr);
        d3d::res::ResourceStateTracker::AddGlobalResourceState(res.Get(), D3D12_RESOURCE_STATE_COPY_DEST);

        // 中間リソースに必要なサイズを取得
        const UINT64 upload_buffer_size = GetRequiredIntermediateSize(res.Get(), 0, static_cast<unsigned int>(subresources.size()));

        // 中間リソースを作成
        Microsoft::WRL::ComPtr<ID3D12Resource> texture_upload_heap;
        hr = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD)
                                             , D3D12_HEAP_FLAG_NONE
                                             , &CD3DX12_RESOURCE_DESC::Buffer(upload_buffer_size)
                                             , D3D12_RESOURCE_STATE_GENERIC_READ
                                             , nullptr
                                             , IID_PPV_ARGS(&texture_upload_heap)
        );
        ASSERT_HR(hr);
        cmd_list->TrackResource(texture_upload_heap.Get());

        // サブリソースの内容をコピー
        UpdateSubresources(cmd_list->GetGraphicsCommandList().Get()
                           , res.Get(), texture_upload_heap.Get()
                           , 0, 0, static_cast<unsigned int>(subresources.size())
                           , subresources.data()
        );

        font_texture = std::make_unique<d3d::res::Texture>();
        font_texture->SetD3D12Resource(res, nullptr);
        font_texture->SetName(L"ImGui Font Texture");

    #else

        D3D12_SUBRESOURCE_DATA subres_data;
        {
            subres_data.pData = pixel_data;
            subres_data.RowPitch = row_pitch;
            subres_data.SlicePitch = slice_pitch;
        }

        auto font_texture_desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);
        font_texture = std::make_unique<d3d::res::Texture>(d3d::res::Texture(device.Get(), font_texture_desc));
        font_texture->SetName(L"ImGui Font Texture");
        cmd_list->CopyTextureSubresource(*font_texture, 0, 1, &subres_data);
        cmd_list->GenerateMips(*font_texture);

    #endif // use DirectXTex

        cmd_queue->ExecuteCommandList(cmd_list);
    }

    // Create the root signature for the ImGUI shaders.
    {
        D3D12_FEATURE_DATA_ROOT_SIGNATURE feature_data = {};
        {
            feature_data.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
        }

        if (FAILED(device2->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &feature_data, sizeof(feature_data))))
        {
            feature_data.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }


        // Allow input layout and deny unnecessary access to certain pipeline stages.
        D3D12_ROOT_SIGNATURE_FLAGS root_signature_flags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

    #define USE_RootSignature 1
    #if USE_RootSignature

        buma::d3d::RootSignatureDesc            rootsig_desc{};
        buma::d3d::RootParameter                rootparams[RootParameters::NumRootParameters];
        // root parameters
        {
            rootparams[RootParameters::MatrixCB].FinalizeAsConstants(0, 0, sizeof(DirectX::XMMATRIX) / 4, D3D12_SHADER_VISIBILITY_VERTEX);
            rootparams[RootParameters::FontTexture].PushDescriptorRangeTypeSRV(1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE);
            rootparams[RootParameters::FontTexture].FinalizeAsDescriptorTable(D3D12_SHADER_VISIBILITY_PIXEL);
        }
        //rootsig_desc.SetRootParameters(rootparams);
        rootsig_desc.PushBackRootParameter(rootparams[0]);
        rootsig_desc.PushBackRootParameter(rootparams[1]);

        // static sampler
        CD3DX12_STATIC_SAMPLER_DESC linear_repeat_sampler(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);
        {
            linear_repeat_sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
            linear_repeat_sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        }
        rootsig_desc.PushBackStaticSamplerDesc(linear_repeat_sampler);

        // finalize
        rootsig_desc.FinalizeRootSignatureDesc(D3D_ROOT_SIGNATURE_VERSION_1_1, root_signature_flags);
        root_signature = std::make_unique<d3d::RootSignature>();
        root_signature->CreateRootSignature(device.Get(), rootsig_desc, L"MyImGui::root_signature");

    #else

        CD3DX12_DESCRIPTOR_RANGE1 desc_range(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

        CD3DX12_ROOT_PARAMETER1 root_params[RootParameters::NumRootParameters];
        {
            root_params[RootParameters::MatrixCB].InitAsConstants(sizeof(DirectX::XMMATRIX) / 4, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);
            root_params[RootParameters::FontTexture].InitAsDescriptorTable(1, &desc_range, D3D12_SHADER_VISIBILITY_PIXEL);
        }

        CD3DX12_STATIC_SAMPLER_DESC linear_repeat_sampler(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);
        {
            linear_repeat_sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
            linear_repeat_sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        }

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_sig_desc;
        {
            root_sig_desc.Init_1_1(RootParameters::NumRootParameters, root_params, 1, &linear_repeat_sampler, root_signature_flags);
        }

        root_signature = std::make_unique<d3d::RootSignature>(root_sig_desc.Desc_1_1, feature_data.HighestVersion);

    #endif // !USE_RootSignature

        const D3D12_INPUT_ELEMENT_DESC input_layout[] =
        {
        //    { SemanticName    , SemanticIndex    , Format                    , InputSlot    , AlignedByteOffset            , InputSlotClass                                , InstanceDataStepRate    }
            { "POSITION"    , 0                , DXGI_FORMAT_R32G32_FLOAT    , 0            , offsetof(ImDrawVert, pos)    , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA    , 0                        },
            { "TEXCOORD"    , 0                , DXGI_FORMAT_R32G32_FLOAT    , 0            , offsetof(ImDrawVert, uv)    , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA    , 0                        },
            { "COLOR"        , 0                , DXGI_FORMAT_R8G8B8A8_UNORM, 0            , offsetof(ImDrawVert, col)    , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA    , 0                        },
        };

        D3D12_RT_FORMAT_ARRAY rtv_formats = {};
        {
            rtv_formats.NumRenderTargets = 1;
            rtv_formats.RTFormats[0] = buma::d3d::GetDeviceResources()->GetBackBufferFormatOnDisplay();
        }

        D3D12_BLEND_DESC blend_desc = {};
        {
            blend_desc.RenderTarget[0].BlendEnable = TRUE;
            blend_desc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
            blend_desc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
            blend_desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
            blend_desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
            blend_desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
            blend_desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
            blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        }

        D3D12_RASTERIZER_DESC rasterizer_desc = {};
        {
            rasterizer_desc.FillMode = D3D12_FILL_MODE_SOLID;
            rasterizer_desc.CullMode = D3D12_CULL_MODE_NONE;
            rasterizer_desc.FrontCounterClockwise = FALSE;
            rasterizer_desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
            rasterizer_desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
            rasterizer_desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
            rasterizer_desc.DepthClipEnable = TRUE;
            rasterizer_desc.MultisampleEnable = FALSE;
            rasterizer_desc.AntialiasedLineEnable = FALSE;
            rasterizer_desc.ForcedSampleCount = 0;
            rasterizer_desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
        }

        D3D12_DEPTH_STENCIL_DESC depth_stencil_desc = {};
        {
            depth_stencil_desc.DepthEnable = FALSE;
            depth_stencil_desc.StencilEnable = FALSE;
        }

        // Setup the pipeline state.
        struct PIPELINE_STATE_STREAM
        {
            CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE         p_root_signature;
            CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT             input_layout;
            CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY     primitive_topology_type;
            CD3DX12_PIPELINE_STATE_STREAM_VS                     vs;
            CD3DX12_PIPELINE_STATE_STREAM_PS                     ps;
            CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS rtv_formats;
            CD3DX12_PIPELINE_STATE_STREAM_SAMPLE_DESC             sample_desc;
            CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC             blend_desc;
            CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER             rasterizer_state;
            CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL         depth_stencil_state;
        };

        PIPELINE_STATE_STREAM pso_stream{};
        {
            pso_stream.p_root_signature = root_signature->GetD3D12RootSignature().Get();
            pso_stream.input_layout = { input_layout, 3 };
            pso_stream.primitive_topology_type = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            pso_stream.vs = { g_ImGUI_VS, sizeof(g_ImGUI_VS) };
            pso_stream.ps = { g_ImGUI_PS, sizeof(g_ImGUI_PS) };
            pso_stream.rtv_formats = rtv_formats;
            pso_stream.sample_desc = { 1, 0 };
            pso_stream.blend_desc = CD3DX12_BLEND_DESC(blend_desc);
            pso_stream.rasterizer_state = CD3DX12_RASTERIZER_DESC(rasterizer_desc);
            pso_stream.depth_stencil_state = CD3DX12_DEPTH_STENCIL_DESC(depth_stencil_desc);
        }

        D3D12_PIPELINE_STATE_STREAM_DESC pso_stream_desc =
        {
            sizeof(PIPELINE_STATE_STREAM),&pso_stream
        };
        HRESULT hr = device2->CreatePipelineState(&pso_stream_desc, IID_PPV_ARGS(&pipeline_state));
        ASSERT_HR(hr);
    }

    return true;
}

void MyImGui::LoadStyle(const std::wstring& _path)
{
    std::ifstream ifs(_path);
    if (ifs.fail())
        return;

    std::string path = std::to_string(_path);
    auto& style = ImGui::GetStyle();
    {
        cereal::BinaryInputArchive ar(ifs);
        ar(cereal::make_nvp(path, style));
    }
}

void MyImGui::SaveStyle(const std::wstring& _path)
{
    std::ofstream ofs(_path);
    if (ofs.fail())
        return;

    std::string path = std::to_string(_path);
    auto& style = ImGui::GetStyle();
    {
        cereal::BinaryOutputArchive ar(ofs);
        ar(cereal::make_nvp(path, style));
    }
}

bool MyImGui::CreateContext(const ImGuiConfigFlags& _config_flags)
{
    // Setup Dear ImGui context
    if (IMGUI_CHECKVERSION() == false)
        return false;

    p_imgui_ctx = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags = _config_flags;

    return true;
}

void MyImGui::NewFrame()
{
    ImGui::SetCurrentContext(p_imgui_ctx);
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void MyImGui::Render(std::shared_ptr<d3d::CommandList> _cmd_list, const d3d::res::RenderTargets& _render_target)
{
    ImGui::SetCurrentContext(p_imgui_ctx);
    ImGui::Render();

    ImGuiIO& io = ImGui::GetIO();
    ImDrawData* draw_data = ImGui::GetDrawData();

    // Check if there is anything to render.
    if (!draw_data || draw_data->CmdListsCount == 0)
        return;

    ImVec2 display_pos = draw_data->DisplayPos;

    _cmd_list->SetGraphicsRootSignature(*root_signature);
    _cmd_list->SetPipelineState(pipeline_state);
    _cmd_list->SetRenderTarget(_render_target);

    // Set root arguments.
//    DirectX::XMMATRIX projection_matrix = DirectX::XMMatrixOrthographicRH( draw_data->DisplaySize.x, draw_data->DisplaySize.y, 0.0f, 1.0f );
    float L = draw_data->DisplayPos.x;
    float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
    float T = draw_data->DisplayPos.y;
    float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
    float mvp[4][4] =
    {
        { 2.0f / (R - L)    , 0.0f                , 0.0f,            0.0f },
        { 0.0f                , 2.0f / (T - B)    , 0.0f,            0.0f },
        { 0.0f                , 0.0f                , 0.5f,            0.0f },
        { (R + L) / (L - R)    , (T + B) / (B - T)    , 0.5f,            1.0f },
    };

    _cmd_list->SetGraphics32BitConstants(RootParameters::MatrixCB, mvp);
    _cmd_list->SetShaderResourceView(RootParameters::FontTexture, 0, *font_texture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    D3D12_VIEWPORT viewport = {};
    viewport.Width = draw_data->DisplaySize.x;
    viewport.Height = draw_data->DisplaySize.y;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    _cmd_list->SetViewport(viewport);
    _cmd_list->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    const DXGI_FORMAT index_format = sizeof(ImDrawIdx) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

    // It may happen that ImGui doesn't actually render anything. In this case,
    // any pending resource barriers in the commandList will not be flushed (since 
    // resource barriers are only flushed when a draw command is executed).
    // In that case, manually flushing the resource barriers will ensure that
    // they are properly flushed before exiting this function.
    _cmd_list->FlushResourceBarriers();

    for (int i_cmd = 0; i_cmd < draw_data->CmdListsCount; ++i_cmd)
    {
        const ImDrawList* draw_list = draw_data->CmdLists[i_cmd];

        _cmd_list->SetDynamicVertexBuffer(0, draw_list->VtxBuffer.size(), sizeof(ImDrawVert), draw_list->VtxBuffer.Data);
        _cmd_list->SetDynamicIndexBuffer(draw_list->IdxBuffer.size(), index_format, draw_list->IdxBuffer.Data);

        int index_offset = 0;
        for (int j_cmd_buf = 0; j_cmd_buf < draw_list->CmdBuffer.size(); j_cmd_buf++)
        {
            const ImDrawCmd& draw_cmd = draw_list->CmdBuffer[j_cmd_buf];
            if (draw_cmd.UserCallback)
            {
                draw_cmd.UserCallback(draw_list, &draw_cmd);
            }
            else
            {
                ImVec4 clip_rect = draw_cmd.ClipRect;
                D3D12_RECT scissor_rect;
                scissor_rect.left = static_cast<LONG>(clip_rect.x - display_pos.x);
                scissor_rect.top = static_cast<LONG>(clip_rect.y - display_pos.y);
                scissor_rect.right = static_cast<LONG>(clip_rect.z - display_pos.x);
                scissor_rect.bottom = static_cast<LONG>(clip_rect.w - display_pos.y);

                if (scissor_rect.right - scissor_rect.left > 0 &&
                    scissor_rect.bottom - scissor_rect.top > 0)
                {
                    // TextureIdにはShaderResourceViewのポインタを使用する
                    if (draw_cmd.TextureId)
                        _cmd_list->SetShaderResourceView(RootParameters::FontTexture, 0, *reinterpret_cast<d3d::res::ShaderResourceView*>(draw_cmd.TextureId), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
                    else
                        _cmd_list->SetShaderResourceView(RootParameters::FontTexture, 0, *font_texture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

                    _cmd_list->SetScissorRect(scissor_rect);
                    _cmd_list->DrawIndexed(draw_cmd.ElemCount, 1, index_offset);
                }
            }
            index_offset += draw_cmd.ElemCount;
        }
    }

}

void MyImGui::Destroy()
{
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext(p_imgui_ctx);
    p_imgui_ctx = nullptr;
}

inline void gui::MyImGui::SetStyleColor(eImGuiStyleColor _e_style_col)
{
    switch (_e_style_col)
    {
    case MyImGui::eImGuiStyleColor::Classic:
        ImGui::StyleColorsClassic();
        break;
    case MyImGui::eImGuiStyleColor::Dark:
        ImGui::StyleColorsDark();
        break;
    case MyImGui::eImGuiStyleColor::Light:
        ImGui::StyleColorsLight();
        break;
    default:
        break;
    }
}

inline void gui::MyImGui::SetStyle(ImGuiStyle& _style)
{
    ImGui::GetStyle() = _style;
}


}// buma


void GetSurfaceInfo
(
    _In_ size_t            _width
    , _In_ size_t        _height
    , _In_ DXGI_FORMAT    _fmt
    , size_t* _out_num_bytes
    , _Out_opt_ size_t* _out_row_bytes
    , _Out_opt_ size_t* _out_num_rows
)
{
    size_t num_bytes = 0;
    size_t row_bytes = 0;
    size_t num_rows = 0;

    bool is_bc = false;
    bool is_packed = false;
    bool is_planar = false;
    size_t bpe = 0;
    switch (_fmt)
    {
    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
        is_bc = true;
        bpe = 8;
        break;

    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        is_bc = true;
        bpe = 16;
        break;

    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
    case DXGI_FORMAT_YUY2:
        is_packed = true;
        bpe = 4;
        break;

    case DXGI_FORMAT_Y210:
    case DXGI_FORMAT_Y216:
        is_packed = true;
        bpe = 8;
        break;

    case DXGI_FORMAT_NV12:
    case DXGI_FORMAT_420_OPAQUE:
        is_planar = true;
        bpe = 2;
        break;

    case DXGI_FORMAT_P010:
    case DXGI_FORMAT_P016:
        is_planar = true;
        bpe = 4;
        break;
    }

    if (is_bc)
    {
        size_t num_blocks_wide = 0;
        if (_width > 0)
        {
            num_blocks_wide = std::max<size_t>(1, (_width + 3) / 4);
        }
        size_t num_blocks_high = 0;
        if (_height > 0)
        {
            num_blocks_high = std::max<size_t>(1, (_height + 3) / 4);
        }
        row_bytes = num_blocks_wide * bpe;
        num_rows = num_blocks_high;
        num_bytes = row_bytes * num_blocks_high;
    }
    else if (is_packed)
    {
        row_bytes = ((_width + 1) >> 1) * bpe;
        num_rows = _height;
        num_bytes = row_bytes * _height;
    }
    else if (_fmt == DXGI_FORMAT_NV11)
    {
        row_bytes = ((_width + 3) >> 2) * 4;
        num_rows = _height * 2;            // Direct3D makes this simplifying assumption, although it is larger than the 4:1:1 data
        num_bytes = row_bytes * num_rows;
    }
    else if (is_planar)
    {
        row_bytes = ((_width + 1) >> 1) * bpe;
        num_bytes = (row_bytes * _height) + ((row_bytes * _height + 1) >> 1);
        num_rows = _height + ((_height + 1) >> 1);
    }
    else
    {
        size_t bpp = DirectX::BitsPerPixel(_fmt);
        row_bytes = (_width * bpp + 7) / 8; // round up to nearest byte
        num_rows = _height;
        num_bytes = row_bytes * _height;
    }

    if (_out_num_bytes)
        *_out_num_bytes = num_bytes;

    if (_out_row_bytes)
        *_out_row_bytes = row_bytes;

    if (_out_num_rows)
        *_out_num_rows = num_rows;
}
