#include "MyImguiRenderer.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
//#include <libloaderapi.h>
#include <Windows.h>
#undef CreateWindow

namespace b = buma3d;

namespace buma
{
namespace gui
{

MyImGuiRenderer::MyImGuiRenderer(RENDER_RESOURCE& _rr, bool _is_viewport_renderer)
    : rr                    { _rr }
    , is_viewport_renderer  { _is_viewport_renderer }
    , total_vtx_count       {}
    , total_idx_count       {}
    , vertex_buffer         {}
    , index_buffer          {}
    , vertex_buffer_view    {}
    , index_buffer_view     {}
    , update_desc           {}
    , descriptor_pool       {}
    , texid_sets            {}
    , fence_to_cpu          {}
    , dummy_fence_val       {}
    , allocator             {}
    , list                  {}
    , args                  {}
    , progress              {}
{
    Init();
}
void MyImGuiRenderer::Init()
{
    b::DESCRIPTOR_POOL_DESC dpd{};
    dpd.flags                       = b::DESCRIPTOR_POOL_FLAG_NONE;
    dpd.max_sets_allocation_count   = 128;
    dpd.max_num_register_space      = 2;

    b::DESCRIPTOR_POOL_SIZE size{ b::DESCRIPTOR_TYPE_SRV_TEXTURE, 128 };
    dpd.num_pool_sizes              = 1;
    dpd.pool_sizes                  = &size;
    dpd.node_mask                   = b::B3D_DEFAULT_NODE_MASK;

    auto bmr = rr.device->CreateDescriptorPool(dpd, &descriptor_pool);
    assert(util::IsSucceeded(bmr));
    descriptor_pool->SetName("MyImGuiRenderer::descriptor_pool");

    texid_sets.resize(dpd.max_sets_allocation_count);
    for (auto& i : texid_sets)
    {
        bmr = descriptor_pool->AllocateDescriptorSet(rr.root_signature.Get(), &i);
        assert(util::IsSucceeded(bmr));
    }

    static const b::CLEAR_VALUE cv{ 0.f, 0.f, 0.f, 1.f };
    args.ca = { b::TEXTURE_ASPECT_FLAG_COLOR, 0, &cv };
    args.cas = { 1, &args.ca };

    bmr = rr.device->CreateCommandAllocator(buma3d::hlp::init::CommandAllocatorDesc(rr.queue->GetDesc().type, buma3d::COMMAND_LIST_LEVEL_PRIMARY, buma3d::COMMAND_ALLOCATOR_FLAG_TRANSIENT), &allocator);
    assert(util::IsSucceeded(bmr));
    allocator->SetName("MyImGuiRenderer::allocator");

    bmr = rr.device->AllocateCommandList(buma3d::hlp::init::CommandListDesc(allocator.Get(), buma3d::B3D_DEFAULT_NODE_MASK), &list);
    assert(util::IsSucceeded(bmr));
    list->SetName("MyImGuiRenderer::list");

    bmr = rr.device->CreateFence(buma3d::hlp::init::BinaryCpuFenceDesc(), &fence_to_cpu);
    assert(util::IsSucceeded(bmr));
    fence_to_cpu->SetName("MyImGuiRenderer::fence_to_cpu");
}

MyImGuiRenderer::~MyImGuiRenderer()
{

}

void MyImGuiRenderer::Draw(buma3d::IFramebuffer* _framebuffer, ImDrawData* _draw_data)
{
    progress.imcmd_list_offset      = 0;
    progress.imcmd_buffer_offset    = 0;
    progress.start_index_location   = 0;
    progress.texid_set_offset       = 0;
    progress.has_done               = false;

    args.draw_indexed.instance_count        = 1;
    args.draw_indexed.start_index_location  = 0;
    args.draw_indexed.base_vertex_location  = 0;

    float L = _draw_data->DisplayPos.x;
    float R = _draw_data->DisplayPos.x + _draw_data->DisplaySize.x;
    float T = _draw_data->DisplayPos.y;
    float B = _draw_data->DisplayPos.y + _draw_data->DisplaySize.y;
    auto Set = [](float _m[4], float _v0, float _v1, float _v2, float _v3) { _m[0] = _v0; _m[1] = _v1; _m[2] = _v2; _m[3] = _v3; };
    Set(args.mvp[0], 2.0f / (R - L)       , 0.0f                    , 0.0f,            0.0f);
    Set(args.mvp[1], 0.0f                 , 2.0f / (T - B)          , 0.0f,            0.0f);
    Set(args.mvp[2], 0.0f                 , 0.0f                    , 0.5f,            0.0f);
    Set(args.mvp[3], (R + L) / (L - R)    , (T + B) / (B - T)       , 0.5f,            1.0f);

    args.push_constants.root_parameter_index       = 0;
    args.push_constants.num32_bit_values_to_set    = sizeof(args.mvp) / 4;
    args.push_constants.src_data                   = args.mvp;
    args.push_constants.dst_offset_in_32bit_values = 0;

    args.bind_vbv.num_views = 1;
    args.bind_vbv.views     = vertex_buffer_view.GetAddressOf();

    args.viewport.width      = _draw_data->DisplaySize.x;
    args.viewport.height     = _draw_data->DisplaySize.y;
    args.viewport.min_depth  = 0.0f;
    args.viewport.max_depth  = 1.0f;

    args.render_pass_begin.render_pass      = rr.render_pass_load.Get();
    args.render_pass_begin.framebuffer      = _framebuffer;
    args.render_pass_begin.num_clear_values = 0;
    args.render_pass_begin.clear_values     = nullptr;

    b::IView* rtv = nullptr;
    if (is_viewport_renderer)
        rtv = _framebuffer->GetDesc().attachments[0];

    UpdateDescriptorSets(_draw_data);
    PrepareBuffers  (_draw_data);
    PrepareDraw     (_framebuffer, _draw_data, rtv);
    BeginDraw       (_framebuffer, _draw_data);
    EndDraw         (_draw_data, rtv);
}
void MyImGuiRenderer::PrepareDraw(buma3d::IFramebuffer* _framebuffer, ImDrawData* _draw_data, buma3d::IView* _rtv)
{
    allocator->Reset(b::COMMAND_ALLOCATOR_RESET_FLAG_NONE);
    list->BeginRecord(args.list_begin_desc);
    list->InsertMarker("MyImGuiRenderer::PrepareDraw", nullptr);
    list->SetRootSignature(b::PIPELINE_BIND_POINT_GRAPHICS, rr.root_signature.Get());
    list->SetPipelineState(rr.pipeline_state_load.Get());

    //bool is_first = false;
    if (is_viewport_renderer &&
        progress.imcmd_list_offset == 0 && progress.imcmd_buffer_offset == 0)// 初回のみバッファのクリア操作を定義します。
    {
        args.barrier_desc.Reset();
        args.barrier_desc.AddTextureBarrier(_rtv, b::RESOURCE_STATE_UNDEFINED, b::RESOURCE_STATE_COLOR_ATTACHMENT_READ_WRITE);
        list->PipelineBarrier(args.barrier_desc.Get(b::PIPELINE_STAGE_FLAG_TOP_OF_PIPE, b::PIPELINE_STAGE_FLAG_COLOR_ATTACHMENT_OUTPUT));

        //is_first = true;
        static const b::CLEAR_RENDER_TARGET_VALUE crtv = { 0.f, 0.f, 0.f, 1.f };
        list->ClearRenderTargetView(static_cast<b::IRenderTargetView*>(_rtv), crtv);
    }

    list->Push32BitConstants(b::PIPELINE_BIND_POINT_GRAPHICS, args.push_constants);
    list->BindDescriptorSet(b::PIPELINE_BIND_POINT_GRAPHICS, { rr.font_set.Get() });
    progress.has_bound_font_set = true;

    list->BindVertexBufferViews(args.bind_vbv);
    list->BindIndexBufferView(index_buffer_view.Get());

    list->SetViewports(1, &args.viewport);

    list->BeginRenderPass(args.render_pass_begin, args.subpass_begin);
    //if (is_first)
    //{
    //    b::SCISSOR_RECT rect{};
    //    rect.offset = { 0,0 };
    //    rect.extent = { (uint32_t)_draw_data->DisplaySize.x, (uint32_t)_draw_data->DisplaySize.y };
    //    args.cas.num_rects = 1;
    //    args.cas.rects     = &rect;
    //    list->ClearAttachments(args.cas);
    //}
}
void MyImGuiRenderer::PrepareBuffers(ImDrawData* _draw_data)
{
    PrepareVertexBuffer(_draw_data);
    PrepareIndexBuffer(_draw_data);

    uint32_t idx_offset = 0;
    uint32_t vtx_offset = 0;
    for (int i_cmd = 0; i_cmd < _draw_data->CmdListsCount; i_cmd++)
    {
        auto&& draw_list = _draw_data->CmdLists[i_cmd];
        memcpy(index_buffer->GetMppedDataAs<ImDrawIdx>(idx_offset)
               , draw_list->IdxBuffer.Data
               , draw_list->IdxBuffer.Size * sizeof(ImDrawIdx));
        idx_offset += draw_list->IdxBuffer.Size;

        memcpy(vertex_buffer->GetMppedDataAs<ImDrawVert>(vtx_offset)
               , draw_list->VtxBuffer.Data
               , draw_list->VtxBuffer.Size * sizeof(ImDrawVert));
        vtx_offset += draw_list->VtxBuffer.Size;
    }
}
void MyImGuiRenderer::PrepareIndexBuffer(ImDrawData* _draw_data)
{
    if (total_idx_count < (uint32_t)_draw_data->TotalIdxCount)
    {
        total_idx_count = (uint32_t)_draw_data->TotalIdxCount;
        index_buffer_view.Reset();
        index_buffer.reset();
        index_buffer = rr.dr->GetResourceCreate()
            ->CreateBuffer(  buma3d::hlp::init::BufferResourceDesc(sizeof(ImDrawIdx) * (uint64_t)total_idx_count
                                                                   , b::BUFFER_USAGE_FLAG_COPY_DST | b::BUFFER_USAGE_FLAG_INDEX_BUFFER)
                           , buma3d::RESOURCE_HEAP_PROPERTY_FLAG_HOST_WRITABLE | buma3d::RESOURCE_HEAP_PROPERTY_FLAG_HOST_COHERENT);

        buma3d::INDEX_BUFFER_VIEW_DESC ibvd{};
        ibvd.buffer_offset  = 0;
        ibvd.size_in_bytes  = sizeof(ImDrawIdx) * total_idx_count;
        ibvd.index_type     = sizeof(ImDrawIdx) == 4 ? b::INDEX_TYPE_UINT32 : b::INDEX_TYPE_UINT16;
        auto bmr = rr.device->CreateIndexBufferView(index_buffer->GetB3DBuffer().Get(), ibvd, &index_buffer_view);
        assert(util::IsSucceeded(bmr));
    }
}
void MyImGuiRenderer::PrepareVertexBuffer(ImDrawData* _draw_data)
{
    if (total_vtx_count < (uint32_t)_draw_data->TotalVtxCount)
    {
        total_vtx_count = (uint32_t)_draw_data->TotalVtxCount;
        vertex_buffer_view.Reset();
        vertex_buffer.reset();
        vertex_buffer = rr.dr->GetResourceCreate()
            ->CreateBuffer(  buma3d::hlp::init::BufferResourceDesc(sizeof(ImDrawVert) * (uint64_t)total_vtx_count
                                                                   , b::BUFFER_USAGE_FLAG_COPY_DST | b::BUFFER_USAGE_FLAG_VERTEX_BUFFER)
                           , buma3d::RESOURCE_HEAP_PROPERTY_FLAG_HOST_WRITABLE | buma3d::RESOURCE_HEAP_PROPERTY_FLAG_HOST_COHERENT);

        buma3d::VERTEX_BUFFER_VIEW_DESC vbvd{};
        uint64_t buffer_offset      = 0;
        uint32_t size_in_bytes      = sizeof(ImDrawVert) * total_vtx_count;
        uint32_t stride_in_bytes    = sizeof(ImDrawVert);
        vbvd.num_input_slots  = 1;
        vbvd.buffer_offsets   = &buffer_offset;
        vbvd.sizes_in_bytes   = &size_in_bytes;
        vbvd.strides_in_bytes = &stride_in_bytes;
        auto bmr = rr.device->CreateVertexBufferView(vertex_buffer->GetB3DBuffer().Get(), vbvd, &vertex_buffer_view);
        assert(util::IsSucceeded(bmr));
    }
}
void MyImGuiRenderer::BeginDraw(buma3d::IFramebuffer* _framebuffer, ImDrawData* _draw_data)
{
    // TextureIdを設定するためには、引数の_cmd_listに直接記録するのではなく、
    // 追加でコマンドリストを作成し、複数のディスクリプタを割り当てられるようにし、その追加のコマンドリストで描画したものを引数のコマンドリストに指定する必要がある。 

    auto&& display_pos = _draw_data->DisplayPos;
    for (int i_cmd = progress.imcmd_list_offset; i_cmd < _draw_data->CmdListsCount; i_cmd++)
    {
        auto&& draw_list = _draw_data->CmdLists[i_cmd];
        for (int i_cmd_buf = progress.imcmd_buffer_offset; i_cmd_buf < draw_list->CmdBuffer.Size; i_cmd_buf++)
        {
            auto&& draw_cmd = draw_list->CmdBuffer.Data[i_cmd_buf];
            auto&& clip_rect = draw_cmd.ClipRect;
            if (!IsScissorValid(display_pos, clip_rect))
            {
                progress.imcmd_buffer_offset++;
                args.draw_indexed.start_index_location += draw_cmd.ElemCount;
                continue;
            }

            args.scissor_rect.offset.x      = static_cast<int32_t>(clip_rect.x - display_pos.x);
            args.scissor_rect.offset.y      = static_cast<int32_t>(clip_rect.y - display_pos.y);
            args.scissor_rect.extent.width  = static_cast<uint32_t>(clip_rect.z - clip_rect.x);
            args.scissor_rect.extent.height = static_cast<uint32_t>(clip_rect.w - clip_rect.y);
            list->SetScissorRects(1, &args.scissor_rect);

            if (draw_cmd.TextureId)
            {
                list->BindDescriptorSet(b::PIPELINE_BIND_POINT_GRAPHICS, { texid_sets.data()[progress.texid_set_offset++].Get() });
                progress.has_bound_font_set = false;
            }
            else if (!progress.has_bound_font_set)
            {
                list->BindDescriptorSet(b::PIPELINE_BIND_POINT_GRAPHICS, { rr.font_set.Get() });
                progress.has_bound_font_set = true;
            }

            args.draw_indexed.index_count_per_instance = draw_cmd.ElemCount;
            list->DrawIndexed(args.draw_indexed);

            progress.imcmd_buffer_offset++;
            if (progress.texid_set_offset == descriptor_pool->GetDesc().max_sets_allocation_count)
                FlushDraw(_framebuffer, _draw_data); // 予約したディスクリプタセットの上限値に達したため、一旦コマンドを実行します。
            args.draw_indexed.start_index_location += draw_cmd.ElemCount;
        }
        progress.imcmd_buffer_offset = 0;
        args.draw_indexed.base_vertex_location += draw_list->VtxBuffer.Size;
    }

    progress.has_done = true;
}
void MyImGuiRenderer::FlushDraw(buma3d::IFramebuffer* _framebuffer, ImDrawData* _draw_data)
{
    EndDraw(_draw_data);
    UpdateDescriptorSets(_draw_data);
    PrepareDraw(_framebuffer, _draw_data);
}
void MyImGuiRenderer::UpdateDescriptorSets(ImDrawData* _draw_data)
{
    update_desc.Reset();
    uint32_t num_texid_sets = 0;
    auto&& display_pos = _draw_data->DisplayPos;
    b::SCISSOR_RECT rect{};
    for (int i_cmd = progress.imcmd_list_offset; i_cmd < _draw_data->CmdListsCount; i_cmd++)
    {
        auto&& draw_list = _draw_data->CmdLists[i_cmd];
        for (int i_cmd_buf = progress.imcmd_buffer_offset; i_cmd_buf < draw_list->CmdBuffer.Size; i_cmd_buf++)
        {
            auto&& draw_cmd = draw_list->CmdBuffer.Data[i_cmd_buf];
            if (draw_cmd.UserCallback)
                continue;

            auto&& clip_rect = draw_cmd.ClipRect;
            if (!IsScissorValid(display_pos, clip_rect))
                continue;

            if (draw_cmd.TextureId)
            {
                auto&& write_set = update_desc.AddNewWriteDescriptorSets();
                auto&& write_table = write_set.AddNewWriteDescriptorTable();
                write_table.AddNewWriteDescriptorRange()
                    .SetDstRange(0, 0, 1)
                    .SetSrcView(0, reinterpret_cast<b::IShaderResourceView*>(draw_cmd.TextureId));
                write_table.Finalize(1);
                write_set.Finalize(texid_sets.data()[num_texid_sets++].Get());
            }

            if (num_texid_sets == descriptor_pool->GetDesc().max_sets_allocation_count)
                goto fulled;
        }
    }

fulled:
    update_desc.Finalize();
    auto bmr = rr.device->UpdateDescriptorSets(update_desc.Get());
    assert(util::IsSucceeded(bmr));

}
void MyImGuiRenderer::EndDraw(ImDrawData* _draw_data, buma3d::IView* _rtv)
{
    list->EndRenderPass({});

    if (is_viewport_renderer && progress.has_done)
    {
        args.barrier_desc.Reset();
        args.barrier_desc.AddTextureBarrier(_rtv, b::RESOURCE_STATE_COLOR_ATTACHMENT_READ_WRITE, b::RESOURCE_STATE_PRESENT);
        list->PipelineBarrier(args.barrier_desc.Get(b::PIPELINE_STAGE_FLAG_COLOR_ATTACHMENT_OUTPUT, b::PIPELINE_STAGE_FLAG_BOTTOM_OF_PIPE));
    }

    auto bmr = list->EndRecord();
    assert(util::IsSucceeded(bmr));
    
    rr.submit_info.wait_fence.num_fences        = 0;
    rr.submit_info.num_command_lists_to_execute = 1;
    rr.submit_info.command_lists_to_execute     = list.GetAddressOf();
    rr.submit_info.signal_fence.num_fences      = 0;

    rr.submit_desc.num_submit_infos    = 1;
    rr.submit_desc.submit_infos        = &rr.submit_info;
    rr.submit_desc.signal_fence_to_cpu = fence_to_cpu.Get();
    bmr = rr.queue->Submit(rr.submit_desc);
    assert(util::IsSucceeded(bmr));

    bmr = fence_to_cpu->Wait(0, UINT32_MAX);
    assert(util::IsSucceeded(bmr));
    bmr = fence_to_cpu->Reset();
    assert(util::IsSucceeded(bmr));
}


MyImGuiViewportRenderer::MyImGuiViewportRenderer(RENDER_RESOURCE& _rr)
    : rr                    { _rr }
    , renderer              {}
    , present_info          {}
    , swapchain             {}
    , swapchain_fence       {}
    , back_buffers          {}
    , framebuffers          {}
    , back_buffer_index     {}
{

}

MyImGuiViewportRenderer::~MyImGuiViewportRenderer()
{
    assert(!renderer);
}

bool MyImGuiViewportRenderer::CreateWindow(ImGuiViewport* _vp)
{
    renderer = new MyImGuiRenderer(rr, /*_is_viewport_renderer =*/true);

    // スワップチェインを作成
    {
        // PlatformHandleRaw should always be a HWND, whereas PlatformHandle might be a higher-level handle (e.g. GLFWWindow*, SDL_Window*).
        // Some backends will leave PlatformHandleRaw NULL, in which case we assume PlatformHandle will contain the HWND.
        auto hwnd = _vp->PlatformHandleRaw ? _vp->PlatformHandleRaw : _vp->PlatformHandle;
        back_buffer_index = UINT_MAX;

        b::SURFACE_DESC                     sd{};
        b::SURFACE_PLATFORM_DATA_WINDOWS    spd{};
        spd.hinstance   = GetModuleHandle(NULL);
        spd.hwnd        = hwnd;
        sd.platform_data.type = b::SURFACE_PLATFORM_DATA_TYPE_WINDOWS;
        sd.platform_data.data = &spd;

        auto scd = b::hlp::init::SwapChainDesc(nullptr
                                               , b::COLOR_SPACE_SRGB_NONLINEAR
                                               , b::hlp::init::SwapChainBufferDesc((uint32_t)_vp->Size.x, (uint32_t)_vp->Size.y, BACK_BUFFER_COUNT
                                                                                   , { rr.render_pass_load->GetDesc().attachments[0].format }, b::SWAP_CHAIN_BUFFER_FLAG_COLOR_ATTACHMENT| b::SWAP_CHAIN_BUFFER_FLAG_COPY_DST)
                                               , rr.queue.GetAddressOf());
        scd.flags = b::SWAP_CHAIN_FLAG_ALLOW_DISCARD_AFTER_PRESENT | b::SWAP_CHAIN_FLAG_DISABLE_VERTICAL_SYNC;

        swapchain = std::make_unique<SwapChain>();
        auto res = swapchain->Create(rr.dr, sd, scd);
        RET_IF_FAILED(res);

        back_buffers            = swapchain->GetBuffers().data();
        swapchain_fence         = &swapchain->GetPresentCompleteFences();
        present_info.wait_fence = swapchain_fence->signal_fence.Get();
    }

    auto result = CreateFramebuffers();
    RET_IF_FAILED(result);

    return true;
}
bool MyImGuiViewportRenderer::CreateFramebuffers()
{
    b::FRAMEBUFFER_DESC fd{ b::FRAMEBUFFER_FLAG_NONE, rr.render_pass_load.Get(), 1 };
    for (uint32_t i = 0; i < BACK_BUFFER_COUNT; i++)
    {
        fd.attachments = (b::IView* const*)back_buffers[i].rtv.GetAddressOf();
        auto bmr = rr.device->CreateFramebuffer(fd, &framebuffers[i]);
        RET_IF_FAILED(util::IsSucceeded(bmr));
    }

    return true;
}
bool MyImGuiViewportRenderer::DestroyWindow(ImGuiViewport* _vp)
{
    for (auto& i : framebuffers)
        i.Reset();

    back_buffers    = nullptr;
    swapchain_fence = nullptr;
    swapchain.reset();

    delete renderer;
    renderer = nullptr;

    return true;
}
bool MyImGuiViewportRenderer::SetWindowSize(ImGuiViewport* _vp, const ImVec2& _size)
{
    for (auto& i : framebuffers)
        i.Reset();
    swapchain->Resize({ (uint32_t)_size.x, (uint32_t)_size.y }, b::SWAP_CHAIN_FLAG_ALLOW_DISCARD_AFTER_PRESENT | b::SWAP_CHAIN_FLAG_DISABLE_VERTICAL_SYNC);
    back_buffers = swapchain->GetBuffers().data();
    auto result = CreateFramebuffers();
    RET_IF_FAILED(result);

    return true;
}
void MyImGuiViewportRenderer::MoveToNextFrame()
{
    uint32_t next_buffer_index = 0;
    auto bmr = swapchain->AcquireNextBuffer(UINT32_MAX, &next_buffer_index);
    assert(bmr == b::BMRESULT_SUCCEED || bmr == b::BMRESULT_SUCCEED_NOT_READY);
    back_buffer_index = next_buffer_index;
}
bool MyImGuiViewportRenderer::RenderWindow(ImGuiViewport* _vp, void* _render_arg)
{
    MoveToNextFrame();
    renderer->Draw(framebuffers[back_buffer_index].Get(), _vp->DrawData);

    return true;
}
bool MyImGuiViewportRenderer::SwapBuffers(ImGuiViewport* _vp, void* _render_arg)
{
    swapchain_fence->signal_fence_to_cpu->Wait(0, UINT32_MAX);
    swapchain_fence->signal_fence_to_cpu->Reset();
    auto bmr = swapchain->Present(present_info);
    BMR_RET_IF_FAILED(bmr);

    return true;
}


}// namespace gui
}// namespace buma
