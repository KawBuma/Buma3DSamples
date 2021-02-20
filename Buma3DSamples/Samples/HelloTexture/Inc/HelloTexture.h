#pragma once

namespace buma
{

class HelloTexture : public ApplicationBase
{
public:
    static constexpr uint32_t BACK_BUFFER_COUNT = 3;

public:
    HelloTexture();
    virtual ~HelloTexture();

    static HelloTexture* Create();

    bool Prepare(PlatformBase& _platform) override;
    void PrepareSubmitInfo();
    void CreateEvents();

    bool Init() override;
    bool InitSwapChain();
    bool LoadAssets();
    bool CreateDescriptorSetLayout();
    bool CreatePipelineLayout();
    bool CreateDescriptorHeap();
    bool CreateDescriptorPools();
    bool AllocateDescriptorSets();
    bool CreateRenderPass();
    bool CreateFramebuffer();
    bool CreateShaderModules();
    bool CreateGraphicsPipelines();
    bool CreateCommandAllocator();
    bool CreateCommandLists();
    bool CreateFences();
    bool CreateBuffers();
    bool CopyBuffers();
    bool CreateBufferViews();
    bool CreateConstantBuffer();
    bool CreateConstantBufferView();
    bool LoadTextureData();
    bool CreateTextureResource();
    bool CopyDataToTexture();
    bool CreateShaderResourceView();
    bool CreateSampler();
    bool UpdateDescriptorSets();

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
    struct FENCE_VALUES {
        FENCE_VALUES& operator++()    { ++wait; ++signal; return *this; } 
        FENCE_VALUES  operator++(int) { auto tmp = *this; wait++; signal++; return tmp; }
        uint64_t wait   = 0;
        uint64_t signal = 1;
    };
    enum SCF { PRESENT_COMPLETE, RENDER_COMPLETE, SWAPCHAIN_FENCE_NUM };

private:
    struct VERTEX {
        buma3d::FLOAT4 position;
        buma3d::FLOAT2 uv;
    };
    std::vector<VERTEX>     quad;
    //std::vector<uint16_t>   index;

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
        void*                                           mapped_data[2/*scene,model*/];
        std::unique_ptr<res::IResourceBuffer>           constant_buffer;
        buma3d::util::Ptr<buma3d::IConstantBufferView>  scene_cbv;
        buma3d::util::Ptr<buma3d::IConstantBufferView>  model_cbv;
    };

    struct TEXTURE
    {
        std::unique_ptr<tex::ITextures>                 data;
        std::unique_ptr<buma::res::IResourceTexture>    texture;
        buma3d::util::Ptr<buma3d::IShaderResourceView>  srv;
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

    uint64_t                                                    CBV_ALIGNMENT;// = dr->GetDeviceAdapterLimits().min_constant_buffer_offset_alignment;
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

    buma3d::util::Ptr<buma3d::IDescriptorSetLayout>             buffer_layout;
    buma3d::util::Ptr<buma3d::IDescriptorSetLayout>             texture_layout;
    buma3d::util::Ptr<buma3d::IPipelineLayout>                  pipeline_layout;
    buma3d::util::Ptr<buma3d::IDescriptorHeap>                  descriptor_heap;
    buma3d::util::Ptr<buma3d::IDescriptorPool>                  descriptor_pool;
    buma3d::util::Ptr<buma3d::IDescriptorPool>                  copyable_descriptor_pool;
    buma3d::util::Ptr<buma3d::IDescriptorUpdate>                descriptor_update;
    std::vector<buma3d::util::Ptr<buma3d::IDescriptorSet>>      buffer_descriptor_sets;
    std::vector<buma3d::util::Ptr<buma3d::IDescriptorSet>>      texture_descriptor_sets;

    buma3d::util::Ptr<buma3d::IRenderPass>                      render_pass;
    std::shared_ptr<buma::res::IResourceBuffer>                 vertex_buffer;
    std::shared_ptr<buma::res::IResourceBuffer>                 index_buffer;
    buma3d::util::Ptr<buma3d::IVertexBufferView>                vertex_buffer_view;
    buma3d::util::Ptr<buma3d::IIndexBufferView>                 index_buffer_view;

    RESOURCE_HEAP_ALLOCATION                                    cb_heap;
    FRAME_CB                                                    frame_cbs[BACK_BUFFER_COUNT];
    TEXTURE                                                     texture;
    buma3d::util::Ptr<buma3d::ISamplerView>                     sampler;

    buma::util::FenceSubmitDesc                                 signal_fence_desc;
    buma::util::FenceSubmitDesc                                 wait_fence_desc;

    buma3d::SUBMIT_INFO                                         submit_info;
    buma3d::SUBMIT_DESC                                         submit;
    buma3d::SWAP_CHAIN_PRESENT_INFO                             present_info;
    buma3d::SCISSOR_RECT                                        present_region;

    std::shared_ptr<ResizeEvent>                                on_resize;
    std::shared_ptr<BufferResizedEvent>                         on_resized;

    DeferredContext                                             copy_ctx;
    DeferredContext                                             ctx;

};

BUMA_DLL_API ApplicationBase* CreateApplication() { return HelloTexture::Create(); }


}// namespace buma
