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

    bool Init() override;
    virtual void LoadAssets();

    void Tick() override;
    virtual void Update();
    virtual void Render();

    void Term() override;

private:
    void PrepareFrame(uint32_t buffer_index);

private:
    struct FENCE_VALUES {
        FENCE_VALUES& operator++()    { ++wait; ++signal; return *this; } 
        FENCE_VALUES  operator++(int) { auto tmp = *this; wait++; signal++; return tmp; }
        uint64_t wait   = 0;
        uint64_t signal = 1;
    };
    enum SCF { PRESENT_COMPLETE, RENDER_COMPLETE, SWAPCHAIN_FENCE_NUM };

private:
    PlatformBase*                                               platform;
    buma3d::util::Ptr<buma3d::IDevice>                          device;
    buma3d::util::Ptr<buma3d::ICommandQueue>                    command_queue;
    StepTimer                                                   timer;

    buma3d::SURFACE_FORMAT                                      sfs_format;
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
    FENCE_VALUES                                                fence_values[BACK_BUFFER_COUNT];
    std::vector<buma3d::util::Ptr<buma3d::IFence>>              cmd_fences;
    buma3d::util::Ptr<buma3d::IFence>                           render_complete_fence;

    std::vector<buma3d::RESOURCE_HEAP_PROPERTIES>               heap_props;

    buma3d::util::Ptr<buma3d::IRootSignature>                   signature;
    buma3d::util::Ptr<buma3d::IRenderPass>                      render_pass;
    buma3d::util::Ptr<buma3d::IResourceHeap>                    resource_heap;
    buma3d::util::Ptr<buma3d::IBuffer>                          vertex_buffer;
    buma3d::util::Ptr<buma3d::IBuffer>                          index_buffer;

    buma3d::util::Ptr<buma3d::IVertexBufferView>                vertex_buffer_view{};
    buma3d::util::Ptr<buma3d::IIndexBufferView>                 index_buffer_view{};


    //buma3d::SWAP_CHAIN_ACQUIRE_NEXT_BUFFER_INFO                 acquire_info;

    buma::util::FenceSubmitDesc                                 signal_fence_desc;
    buma::util::FenceSubmitDesc                                 wait_fence_desc;
    //b::IFence*                                                submit_waits[]       = { cmd_fences[back_buffer_index].Get(), swapchain_fences[SCF::PRESENT_COMPLETE].Get() };
    //uint64_t                                                  submit_wait_vals[]   = { 0, 0 };
    //b::IFence*                                                submit_signals[]     = { cmd_fences[back_buffer_index].Get(), swapchain_fences[SCF::RENDER_COMPLETE].Get() };
    //uint64_t                                                  submit_signal_vals[] = { 0, 0 };

    buma3d::SUBMIT_INFO                                         submit_info;
    buma3d::SUBMIT_DESC                                         submit;
    buma3d::SWAP_CHAIN_PRESENT_INFO                             present_info;
    buma3d::SCISSOR_RECT                                        present_region;


};


}// namespace buma
