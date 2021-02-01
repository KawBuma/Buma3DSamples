#pragma once

namespace buma
{
namespace draws
{

class PipelineDescription
{
public:
    PipelineDescription(DrawsInstance* _ins);
    void Init();
    ~PipelineDescription();

private:
    void PrepareInputLayout();
    void PrepareTessellationState();
    void PrepareViewportState();
    void PrepareRasterizationState();
    void PrepareStreamOutput();
    void PrepareMultisampleState();
    void PrepareDepthStencilState();
    void PrepareBlendState();
    void PrepareDynamicState();
    void PrepareDynamicStates();

private:
    DrawsInstance*                                                                  ins;
    buma3d::IRenderPass*                                                            render_pass;

    buma3d::GRAPHICS_PIPELINE_STATE_DESC                                            pso_desc;
    std::array<std::shared_ptr<util::InputLayoutDesc>, RENDER_PASS_TYPE_NUM_TYPES>  input_layout;
    buma3d::TESSELLATION_STATE_DESC                                                 tessellation_state;
    buma3d::VIEWPORT_STATE_DESC                                                     viewport_state;
    buma3d::RASTERIZATION_STATE_DESC                                                rasterization_state;
    buma3d::STREAM_OUTPUT_DESC                                                      stream_output;
    std::array<buma3d::MULTISAMPLE_STATE_DESC, MATERIAL_BLEND_MODE_NUM_MODES>       multisample_state;
    std::array<buma3d::DEPTH_STENCIL_STATE_DESC, RENDER_PASS_TYPE_NUM_TYPES>        depth_stencil_state;
    std::array<util::BlendStateDesc, MATERIAL_BLEND_MODE_NUM_MODES>                 blend_state;
    buma3d::DYNAMIC_STATE_DESC                                                      dynamic_state;
    std::vector<buma3d::DYNAMIC_STATE>                                              dynamic_states;

};


}// namespace draws
}// namespace buma
