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

    const buma3d::INPUT_LAYOUT_DESC*        GetInputLayout(RENDER_PASS_TYPE _type) const;
    const buma3d::VIEWPORT_STATE_DESC*      GetViewportState() const;
    const buma3d::RASTERIZATION_STATE_DESC* GetRasterizationState() const;
    const buma3d::MULTISAMPLE_STATE_DESC*   GetMultisampleState() const;
    const buma3d::DEPTH_STENCIL_STATE_DESC* GetDepthStencilState(RENDER_PASS_TYPE _type) const;
    const buma3d::BLEND_STATE_DESC*         GetBlendState(MATERIAL_BLEND_MODE _mode) const;
    const buma3d::DYNAMIC_STATE_DESC*       GetDynamicState() const;

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

private:
    DrawsInstance*                                                                  ins;
    buma3d::IRenderPass*                                                            render_pass;

    buma3d::GRAPHICS_PIPELINE_STATE_DESC                                            pso_desc;
    std::array<std::shared_ptr<util::InputLayoutDesc>, RENDER_PASS_TYPE_NUM_TYPES>  input_layout;
    buma3d::TESSELLATION_STATE_DESC                                                 tessellation_state;
    buma3d::VIEWPORT_STATE_DESC                                                     viewport_state;
    buma3d::RASTERIZATION_STATE_DESC                                                rasterization_state;
    buma3d::STREAM_OUTPUT_DESC                                                      stream_output;
    buma3d::MULTISAMPLE_STATE_DESC                                                  multisample_state;
    std::array<buma3d::DEPTH_STENCIL_STATE_DESC, RENDER_PASS_TYPE_NUM_TYPES>        depth_stencil_state;
    std::array<util::BlendStateDesc, MATERIAL_BLEND_MODE_NUM_MODES>                 blend_state;
    buma3d::DYNAMIC_STATE_DESC                                                      dynamic_state;
    std::vector<buma3d::DYNAMIC_STATE>                                              dynamic_states;

};


}// namespace draws
}// namespace buma
