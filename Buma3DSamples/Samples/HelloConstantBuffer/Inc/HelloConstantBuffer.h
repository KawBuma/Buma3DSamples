#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace buma
{

class HelloConstantBuffer : public ApplicationBase
{
public:
    static constexpr uint32_t BACK_BUFFER_COUNT = 3;

public:
    HelloConstantBuffer();
    virtual ~HelloConstantBuffer();

    static HelloConstantBuffer* Create();

    bool Prepare(PlatformBase& _platform) override;
    void PrepareSubmitInfo();
    void CreateEvents();

    bool Init() override;
    bool InitSwapChain();
    bool LoadAssets();
    bool CreateDescriptorSetLayout();
    bool CreatePipelineLayout();
    bool CreateDescriptorHeap();
    bool CreateDescriptorPool();
    bool AllocateDescriptorSets();
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
    bool CreateConstantBuffer();
    bool CreateConstantBufferView();
    bool UpdateDescriptorSet();

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

    struct CB_MODEL // register(b0, space0);
    {
        glm::mat4 model;
    };
    struct CB_SCENE // register(b0, space1);
    {
        glm::mat4 view_proj;
    };
    CB_MODEL cb_model;
    CB_SCENE cb_scene;

    struct FRAME_CB
    {
        void*                                                   mapped_data[2/*model,scene*/];
        buma3d::util::Ptr<buma3d::IBuffer>                      constant_buffer;
        buma3d::util::Ptr<buma3d::IConstantBufferView>          scene_cbv;
        buma3d::util::Ptr<buma3d::IConstantBufferView>          model_cbv;
    };

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

    buma3d::util::Ptr<buma3d::IDescriptorSetLayout>             descriptor_set_layout;
    buma3d::util::Ptr<buma3d::IPipelineLayout>                  pipeline_layout;
    buma3d::util::Ptr<buma3d::IDescriptorHeap>                  descriptor_heap;
    buma3d::util::Ptr<buma3d::IDescriptorPool>                  descriptor_pool;
    buma3d::util::Ptr<buma3d::IDescriptorUpdate>                descriptor_update;
    std::vector<buma3d::util::Ptr<buma3d::IDescriptorSet>>      descriptor_sets;

    buma3d::util::Ptr<buma3d::IRenderPass>                      render_pass;
    buma3d::util::Ptr<buma3d::IResourceHeap>                    resource_heap;
    buma3d::util::Ptr<buma3d::IBuffer>                          vertex_buffer;
    buma3d::util::Ptr<buma3d::IBuffer>                          index_buffer;
    buma3d::util::Ptr<buma3d::IBuffer>                          vertex_buffer_src;
    buma3d::util::Ptr<buma3d::IBuffer>                          index_buffer_src;

    buma3d::util::Ptr<buma3d::IVertexBufferView>                vertex_buffer_view;
    buma3d::util::Ptr<buma3d::IIndexBufferView>                 index_buffer_view;

    RESOURCE_HEAP_ALLOCATION                                    cb_heap;
    FRAME_CB                                                    frame_cbs[BACK_BUFFER_COUNT];

    buma::util::FenceSubmitDesc                                 signal_fence_desc;
    buma::util::FenceSubmitDesc                                 wait_fence_desc;

    buma3d::SUBMIT_INFO                                         submit_info;
    buma3d::SUBMIT_DESC                                         submit;
    buma3d::SWAP_CHAIN_PRESENT_INFO                             present_info;
    buma3d::SCISSOR_RECT                                        present_region;

    std::shared_ptr<ResizeEvent>                                on_resize;
    std::shared_ptr<BufferResizedEvent>                         on_resized;

    DeferredContext ctx{};

};

BUMA_DLL_API ApplicationBase* CreateApplication() { return HelloConstantBuffer::Create(); }


}// namespace buma
