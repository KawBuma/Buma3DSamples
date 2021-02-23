#pragma once

namespace buma
{

class HelloTriangle : public ApplicationBase
{
public:
    static constexpr uint32_t BACK_BUFFER_COUNT = 3;

public:
    HelloTriangle();
    virtual ~HelloTriangle();

    static HelloTriangle* Create();

    bool Prepare(PlatformBase& _platform) override;
    void PrepareSubmitInfo();
    void CreateEvents();

    bool Init() override;
    bool InitSwapChain();
    bool LoadAssets();
    bool CreatePipelineLayout();
    bool CreateRenderPass();
    bool CreateFramebuffer();
    bool CreateShaderModules();
    bool CreateGraphicsPipelines();
    bool CreateCommandAllocator();
    bool CreateCommandLists();
    bool CreateFences();
    bool CreateBuffers();
    bool CreateHeaps(buma3d::RESOURCE_HEAP_ALLOCATION_INFO* _heap_alloc_info, std::vector<buma3d::RESOURCE_ALLOCATION_INFO>* _alloc_infos);
    bool BindResourceHeaps(buma3d::RESOURCE_HEAP_ALLOCATION_INFO* _heap_alloc_info, std::vector<buma3d::RESOURCE_ALLOCATION_INFO>* _alloc_infos);
    bool CreateBuffersForCopy();
    bool CopyBuffers();
    bool CreateBufferViews();

    void Tick() override;
    void Update();
    void Render();

    void MoveToNextFrame();

    void OnResize(ResizeEventArgs* _args);
    void OnResized(BufferResizedEventArgs* _args);

    void Term() override;

private:
    void PrepareFrame(uint32_t buffer_index);

private:
    struct VERTEX {
        buma3d::FLOAT4 position;
        buma3d::FLOAT4 color;
    };
    std::vector<VERTEX> triangle;
    std::vector<uint16_t> index;

    class ResizeEvent;
    class BufferResizedEvent;

private:
    PlatformBase*                                               platform;
    std::shared_ptr<WindowBase>                                 spwindow;
    WindowBase*                                                 window;
    buma3d::util::Ptr<buma3d::IDevice>                          device;
    buma3d::util::Ptr<buma3d::ICommandQueue>                    command_queue;
    StepTimer                                                   timer;

    std::shared_ptr<buma::SwapChain>                            swapchain;
    const std::vector<buma::SwapChain::SWAP_CHAIN_BUFFER>*      back_buffers;
    uint32_t                                                    back_buffer_index;
    const SwapChain::PRESENT_COMPLETE_FENCES*                   swapchain_fences;

    buma3d::VIEWPORT                                            vpiewport;
    buma3d::SCISSOR_RECT                                        scissor_rect;
    std::vector<buma3d::util::Ptr<buma3d::IFramebuffer>>        framebuffers;

    std::vector<buma3d::util::Ptr<buma3d::IShaderModule>>       shader_modules;
    buma3d::util::Ptr<buma3d::IPipelineState>                   pipeline;
    std::vector<buma3d::util::Ptr<buma3d::ICommandAllocator>>   cmd_allocator;
    std::vector<buma3d::util::Ptr<buma3d::ICommandList>>        cmd_lists;

    buma3d::util::Ptr<buma3d::IFence>                           util_fence;
    util::FENCE_VALUES                                          fence_values[BACK_BUFFER_COUNT];
    std::vector<buma3d::util::Ptr<buma3d::IFence>>              cmd_fences;
    buma3d::util::Ptr<buma3d::IFence>                           render_complete_fence;

    std::vector<buma3d::RESOURCE_HEAP_PROPERTIES>               heap_props;

    buma3d::util::Ptr<buma3d::IPipelineLayout>                  pipeline_layout;
    buma3d::util::Ptr<buma3d::IRenderPass>                      render_pass;
    buma3d::util::Ptr<buma3d::IResourceHeap>                    resource_heap;
    buma3d::util::Ptr<buma3d::IBuffer>                          vertex_buffer;
    buma3d::util::Ptr<buma3d::IBuffer>                          index_buffer;
    buma3d::util::Ptr<buma3d::IBuffer>                          vertex_buffer_src;
    buma3d::util::Ptr<buma3d::IBuffer>                          index_buffer_src;

    buma3d::util::Ptr<buma3d::IVertexBufferView>                vertex_buffer_view;
    buma3d::util::Ptr<buma3d::IIndexBufferView>                 index_buffer_view;

    buma::util::FenceSubmitDesc                                 signal_fence_desc;
    buma::util::FenceSubmitDesc                                 wait_fence_desc;

    buma3d::SUBMIT_INFO                                         submit_info;
    buma3d::SUBMIT_DESC                                         submit;
    buma3d::SWAP_CHAIN_PRESENT_INFO                             present_info;
    buma3d::SCISSOR_RECT                                        present_region;

    std::shared_ptr<ResizeEvent>                                on_resize;
    std::shared_ptr<BufferResizedEvent>                         on_resized;

};

BUMA_DLL_API ApplicationBase* CreateApplication() { return HelloTriangle::Create(); }


}// namespace buma
