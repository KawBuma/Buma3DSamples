#pragma once
namespace buma
{
namespace draws
{

class PipelineDescription
{
public:
    PipelineDescription()
    {
    }

    ~PipelineDescription()
    {
    }

private:
    buma3d::IRootSignature*                             root_signature;
    buma3d::IRenderPass*                                render_pass;
    uint32_t                                            subpass;
    buma3d::NodeMask                                    node_mask; // パイプラインステートオブジェクトが構築されるノードを示す単一のビットを指定します。
    buma3d::PIPELINE_STATE_FLAGS                        flags;

    uint32_t                                            num_shader_stages;
    buma3d::PIPELINE_SHADER_STAGE_DESC                  shader_stages;
    buma3d::INPUT_LAYOUT_DESC                           input_layout;
    buma3d::INPUT_ASSEMBLY_STATE_DESC                   input_assembly_state;
    buma3d::TESSELLATION_STATE_DESC                     tessellation_state; // PRIMITIVE_TOPOLOGY_PATCH_LISTの場合に使用します。
    buma3d::VIEWPORT_STATE_DESC                         viewport_state;
    buma3d::RASTERIZATION_STATE_DESC                    rasterization_state;
    buma3d::STREAM_OUTPUT_DESC                          stream_output;      // 現在はD3D12のみの対応です。
    buma3d::MULTISAMPLE_STATE_DESC                      multisample_state;
    buma3d::DEPTH_STENCIL_STATE_DESC                    depth_stencil_state;
    buma3d::BLEND_STATE_DESC                            blend_state;
    buma3d::DYNAMIC_STATE_DESC                          dynamic_state;


};


}// namespace draws
}// namespace buma
