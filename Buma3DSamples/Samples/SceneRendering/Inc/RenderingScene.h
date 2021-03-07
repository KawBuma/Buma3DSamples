#pragma once

namespace buma
{

class Node
{
public:
    Node()
    {
    }

    ~Node()
    {
    }

private:
    const Node*                         parent;
    std::vector<std::unique_ptr<Node>>  children;
    glm::mat4                           transform;
    bool                                is_visible;

};

class View
{
public:
    View()
    {
    }

    ~View()
    {
    }

private:

};

struct RNEDER_RESOURCES
{
    struct RENDER_PASS
    {
        buma3d::util::Ptr<buma3d::IRenderPass>              forward;
        buma3d::util::Ptr<buma3d::IRenderPass>              shadow;
    //  buma3d::util::Ptr<buma3d::IRenderPass>              forward_msaa;
    //  buma3d::util::Ptr<buma3d::IRenderPass>              shadow_msaa;
    };
    struct DESCRIPTOR
    {
        buma3d::util::Ptr<buma3d::IDescriptorSetLayout>     material_layout;
        buma3d::util::Ptr<buma3d::IDescriptorSetLayout>     mesh_layout;

        buma3d::util::Ptr<buma3d::IDescriptorHeap>          heap;
        buma3d::util::Ptr<buma3d::IDescriptorPool>          material_pool;
        buma3d::util::Ptr<buma3d::IDescriptorPool>          mesh_pool;

        util::UpdateDescriptorSetDesc                       update_desc;
        buma3d::util::Ptr<buma3d::IDescriptorUpdate>        update;
    };
    struct PIPELINE_LAYOUT
    {
        buma3d::util::Ptr<buma3d::IPipelineLayout>          forward;
        buma3d::util::Ptr<buma3d::IPipelineLayout>          shadow;
    };
    struct PIPELINE
    {
        buma3d::util::Ptr<buma3d::IPipelineState>           forward_opaque;
        buma3d::util::Ptr<buma3d::IPipelineState>           forward_masked;
        buma3d::util::Ptr<buma3d::IPipelineState>           forward_blend;
        buma3d::util::Ptr<buma3d::IPipelineState>           shadow_opaque;
        buma3d::util::Ptr<buma3d::IPipelineState>           shadow_masked;
    };

    DESCRIPTOR      descriptor;
    RENDER_PASS     render_pass;
    PIPELINE_LAYOUT pipeline_layout;
    PIPELINE        pipeline;

};

class RnederingScene
{
public:
    RnederingScene();
    ~RnederingScene();

private:

};


}// namespace buma
