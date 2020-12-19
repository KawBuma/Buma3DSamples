#pragma once
#include "Framework.h"
#include "Buma3D.h"
#include "./Utils.h"
#include "imgui.h"

namespace buma
{
namespace gui
{


class MyImGuiViewportRenderer;
class MyImGuiRenderer
{
    friend class MyImGuiViewportRenderer;
public:
    MyImGuiRenderer(RENDER_RESOURCE& _rr, bool _is_viewport_renderer = false);
    ~MyImGuiRenderer();

    void Draw(buma3d::IFramebuffer* _framebuffer, ImDrawData* _draw_data);

private:
    void Init();

private:
    void PrepareDraw            (buma3d::IFramebuffer* _framebuffer, ImDrawData* _draw_data, buma3d::IView* _rtv = nullptr);
    void PrepareBuffers         (ImDrawData* _draw_data);
    void PrepareIndexBuffer     (ImDrawData* _draw_data);
    void PrepareVertexBuffer    (ImDrawData* _draw_data);
    void BeginDraw              (buma3d::IFramebuffer* _framebuffer, ImDrawData* _draw_data);
    void FlushDraw              (buma3d::IFramebuffer* _framebuffer, ImDrawData* _draw_data);
    void UpdateDescriptorSets   (ImDrawData* _draw_data);
    void EndDraw                (ImDrawData* _draw_data, buma3d::IView* _rtv = nullptr);

private:
    bool IsScissorValid(const ImVec2& _disp_pos, const ImVec4& _clip_rect) const
    {
        return  (_clip_rect.x - _disp_pos.x) >= 0 && (_clip_rect.y - _disp_pos.y) >= 0 &&
                (_clip_rect.z - _disp_pos.x) > (_clip_rect.x - _disp_pos.x) &&
                (_clip_rect.w - _disp_pos.y) > (_clip_rect.y - _disp_pos.y);
    }

private:
    struct B3D_CMD_ARGS
    {
        buma3d::COMMAND_LIST_BEGIN_DESC         list_begin_desc     { buma3d::COMMAND_LIST_BEGIN_FLAG_ONE_TIME_SUBMIT, };
        buma3d::CMD_PUSH_32BIT_CONSTANTS        push_constants      {};
        float                                   mvp[4][4]           {};
        buma3d::CMD_BIND_VERTEX_BUFFER_VIEWS    bind_vbv            {};
        buma3d::VIEWPORT                        viewport            {};
        buma3d::SCISSOR_RECT                    scissor_rect        {};
        buma3d::SUBPASS_BEGIN_DESC              subpass_begin       { buma3d::SUBPASS_CONTENTS_INLINE };
        buma3d::RENDER_PASS_BEGIN_DESC          render_pass_begin   {};
        buma3d::DRAW_INDEXED_ARGUMENTS          draw_indexed        {};
        buma3d::CLEAR_ATTACHMENT                ca                  {};
        buma3d::CMD_CLEAR_ATTACHMENTS           cas                 {};
        util::PipelineBarrierDesc               barrier_desc        {};
    };
    struct IMGUI_DRAW_PROGRESS
    {
        int                                     imcmd_list_offset;
        int                                     imcmd_buffer_offset;
        uint32_t                                start_index_location;
        uint32_t                                texid_set_offset;
        bool                                    has_bound_font_set;
        bool                                    has_done;
    };

private:
    RENDER_RESOURCE&                                        rr;
    bool                                                    is_viewport_renderer;

    uint32_t                                                total_vtx_count;
    uint32_t                                                total_idx_count;
    std::shared_ptr<res::IResourceBuffer>                   vertex_buffer;
    std::shared_ptr<res::IResourceBuffer>                   index_buffer;
    buma3d::util::Ptr<buma3d::IVertexBufferView>            vertex_buffer_view;
    buma3d::util::Ptr<buma3d::IIndexBufferView>             index_buffer_view;

    util::UpdateDescriptorSetDesc                           update_desc;
    buma3d::util::Ptr<buma3d::IDescriptorPool>              descriptor_pool;
    std::vector<buma3d::util::Ptr<buma3d::IDescriptorSet>>  texid_sets;

    buma3d::util::Ptr<buma3d::IFence>                       fence_to_cpu;
    uint64_t                                                dummy_fence_val;
    buma3d::util::Ptr<buma3d::ICommandAllocator>            allocator;
    buma3d::util::Ptr<buma3d::ICommandList>                 list;

    B3D_CMD_ARGS                                            args;
    IMGUI_DRAW_PROGRESS                                     progress;

};

class MyImGuiViewportRenderer
{
public:
    static constexpr uint32_t BACK_BUFFER_COUNT = 3;

public:
    MyImGuiViewportRenderer(RENDER_RESOURCE& _rr);
    ~MyImGuiViewportRenderer();

    bool CreateWindow(ImGuiViewport* _vp);
    bool CreateFramebuffers();
    bool DestroyWindow(ImGuiViewport* _vp);
    bool SetWindowSize(ImGuiViewport* _vp, const ImVec2& _size);
    void MoveToNextFrame();
    bool RenderWindow(ImGuiViewport* _vp, void* _render_arg);
    bool SwapBuffers(ImGuiViewport* _vp, void* _render_arg);

private:
    RENDER_RESOURCE&                            rr;
    MyImGuiRenderer*                            renderer;
    buma3d::SWAP_CHAIN_PRESENT_INFO             present_info;
    std::unique_ptr<SwapChain>                  swapchain;
    const SwapChain::PRESENT_COMPLETE_FENCES*   swapchain_fence;
    const SwapChain::SWAP_CHAIN_BUFFER*         back_buffers;
    buma3d::util::Ptr<buma3d::IFramebuffer>     framebuffers[BACK_BUFFER_COUNT];
    uint32_t                                    back_buffer_index;

};

}// namespace gui
}// namespace buma
