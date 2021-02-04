#include "pch.h"
#include "PipelineDescription.h"

namespace buma
{
namespace draws
{

namespace /*anonymous*/
{

inline buma3d::RESOURCE_FORMAT GetVertexElementFormat(VERTEX_ELEMENT_TYPE _type)
{
    switch (_type)
    {
    case buma::draws::VERTEX_ELEMENT_TYPE_POSITION  : return buma3d::RESOURCE_FORMAT_R32G32B32A32_FLOAT; 
    case buma::draws::VERTEX_ELEMENT_TYPE_NORMAL    : return buma3d::RESOURCE_FORMAT_R32G32B32A32_FLOAT; 
    case buma::draws::VERTEX_ELEMENT_TYPE_TANGENT   : return buma3d::RESOURCE_FORMAT_R32G32B32A32_FLOAT; 
    case buma::draws::VERTEX_ELEMENT_TYPE_TEXCOORD0 : return buma3d::RESOURCE_FORMAT_R32G32_FLOAT;
    default:
        return buma3d::RESOURCE_FORMAT_UNKNOWN;
    }
}

}// namespace /*anonymous*/


PipelineDescription::PipelineDescription(DrawsInstance* _ins)
    : ins{ _ins }
{

}

PipelineDescription::~PipelineDescription()
{

}

void PipelineDescription::Init()
{
    PrepareInputLayout();
    PrepareTessellationState();
    PrepareViewportState();
    PrepareRasterizationState();
    PrepareStreamOutput();
    PrepareMultisampleState();
    PrepareDepthStencilState();
    PrepareBlendState();
    PrepareDynamicState();
}
void PipelineDescription::PrepareInputLayout()
{
    // prepass: opaque
    // prepass: masked
    // prepass: opaque&&カスタム頂点
    // prepass: masked&&カスタム頂点

    // RENDER_PASS_TYPE_PRE_DEPTH
    {
        // TODO: 入力レイアウトをリフレクションによって構築
        auto&& l = *(input_layout[RENDER_PASS_TYPE_PRE_DEPTH] = std::make_unique<util::InputLayoutDesc>());
        //l.AddNewInputSlot().SetSlotNumber(0).SetStrideInBytes(sizeof(VertPositionT)).AddNewInputElement(VERTEX_SEMANTIC_NAME_POSITION , 0, GetVertexElementFormat(VERTEX_ELEMENT_TYPE_POSITION));
        //l.AddNewInputSlot().SetSlotNumber(1).SetStrideInBytes(sizeof(VertTexcoordT)).AddNewInputElement(VERTEX_SEMANTIC_NAME_TEXCOORD0, 0, GetVertexElementFormat(VERTEX_ELEMENT_TYPE_TEXCOORD0));
        l.AddNewInputSlot().SetSlotNumber(0).SetStrideInBytes(sizeof(VertPositionT)).AddNewInputElement(VERTEX_SEMANTIC_NAME_POSITION , 0, GetVertexElementFormat(VERTEX_ELEMENT_TYPE_POSITION));
        l.AddNewInputSlot().SetSlotNumber(1).SetStrideInBytes(sizeof(VertNormalT))  .AddNewInputElement(VERTEX_SEMANTIC_NAME_NORMAL   , 0, GetVertexElementFormat(VERTEX_ELEMENT_TYPE_NORMAL));
        l.AddNewInputSlot().SetSlotNumber(2).SetStrideInBytes(sizeof(VertTangentT)) .AddNewInputElement(VERTEX_SEMANTIC_NAME_TANGENT  , 0, GetVertexElementFormat(VERTEX_ELEMENT_TYPE_TANGENT));
        l.AddNewInputSlot().SetSlotNumber(3).SetStrideInBytes(sizeof(VertTexcoordT)).AddNewInputElement(VERTEX_SEMANTIC_NAME_TEXCOORD0, 0, GetVertexElementFormat(VERTEX_ELEMENT_TYPE_TEXCOORD0));
    }
    // RENDER_PASS_TYPE_BASE
    {
        auto&& l = *(input_layout[RENDER_PASS_TYPE_BASE] = std::make_unique<util::InputLayoutDesc>());
        l.AddNewInputSlot().SetSlotNumber(0).SetStrideInBytes(sizeof(VertPositionT)).AddNewInputElement(VERTEX_SEMANTIC_NAME_POSITION , 0, GetVertexElementFormat(VERTEX_ELEMENT_TYPE_POSITION));
        l.AddNewInputSlot().SetSlotNumber(1).SetStrideInBytes(sizeof(VertNormalT))  .AddNewInputElement(VERTEX_SEMANTIC_NAME_NORMAL   , 0, GetVertexElementFormat(VERTEX_ELEMENT_TYPE_NORMAL));
        l.AddNewInputSlot().SetSlotNumber(2).SetStrideInBytes(sizeof(VertTangentT)) .AddNewInputElement(VERTEX_SEMANTIC_NAME_TANGENT  , 0, GetVertexElementFormat(VERTEX_ELEMENT_TYPE_TANGENT));
        l.AddNewInputSlot().SetSlotNumber(3).SetStrideInBytes(sizeof(VertTexcoordT)).AddNewInputElement(VERTEX_SEMANTIC_NAME_TEXCOORD0, 0, GetVertexElementFormat(VERTEX_ELEMENT_TYPE_TEXCOORD0));
    }
    input_layout[RENDER_PASS_TYPE_LIGHTING]     = input_layout[RENDER_PASS_TYPE_BASE];
    input_layout[RENDER_PASS_TYPE_TRANSLUCENT]  = nullptr;
}
void PipelineDescription::PrepareTessellationState()
{
    //tessellation_state.patch_control_points;
}
void PipelineDescription::PrepareViewportState()
{
    viewport_state.num_scissor_rects = 1;
    viewport_state.num_viewports     = 1;
}
void PipelineDescription::PrepareRasterizationState()
{
    rasterization_state.fill_mode                      = buma3d::FILL_MODE_SOLID;
    rasterization_state.cull_mode                      = buma3d::CULL_MODE_BACK;
    rasterization_state.is_front_counter_clockwise     = false;
    rasterization_state.is_enabled_depth_clip          = true;
    rasterization_state.is_enabled_depth_bias          = false;
    rasterization_state.depth_bias_scale               = 0;
    rasterization_state.depth_bias_clamp               = 0;
    rasterization_state.depth_bias_slope_scale         = 0;
    rasterization_state.is_enabled_conservative_raster = false;
    rasterization_state.line_rasterization_mode        = buma3d::LINE_RASTERIZATION_MODE_DEFAULT;
    rasterization_state.line_width                     = 1;
}
void PipelineDescription::PrepareStreamOutput()
{
    stream_output = {};
}
void PipelineDescription::PrepareMultisampleState()
{
    multisample_state.rasterization_samples          = 1;
    multisample_state.sample_masks                   = buma3d::B3D_DEFAULT_SAMPLE_MASK;
    multisample_state.is_enabled_alpha_to_coverage   = false;
    multisample_state.is_enabled_sample_rate_shading = false;
    multisample_state.sample_position_state          = {};
}
void PipelineDescription::PrepareDepthStencilState()
{
    auto&& ds = depth_stencil_state[RENDER_PASS_TYPE_PRE_DEPTH];
    ds.is_enabled_depth_test        = true;
    ds.is_enabled_depth_write       = true;
    ds.depth_comparison_func        = buma3d::COMPARISON_FUNC_GREATER_EQUAL;
    ds.is_enabled_depth_bounds_test = false;
    ds.min_depth_bounds             = 0;
    ds.max_depth_bounds             = 1;
    
    ds.is_enabled_stencil_test            = false;
    ds.stencil_front_face.fail_op         = buma3d::STENCIL_OP_KEEP;
    ds.stencil_front_face.depth_fail_op   = buma3d::STENCIL_OP_KEEP;
    ds.stencil_front_face.pass_op         = buma3d::STENCIL_OP_REPLACE;
    ds.stencil_front_face.comparison_func = buma3d::COMPARISON_FUNC_ALWAYS;
    ds.stencil_front_face.compare_mask    = buma3d::B3D_DEFAULT_STENCIL_COMPARE_MASK;
    ds.stencil_front_face.write_mask      = buma3d::B3D_DEFAULT_STENCIL_WRITE_MASK;
    ds.stencil_front_face.reference       = buma3d::B3D_DEFAULT_STENCIL_REFERENCE;
    
    ds.stencil_back_face.fail_op         = buma3d::STENCIL_OP_KEEP;
    ds.stencil_back_face.depth_fail_op   = buma3d::STENCIL_OP_KEEP;
    ds.stencil_back_face.pass_op         = buma3d::STENCIL_OP_REPLACE;
    ds.stencil_back_face.comparison_func = buma3d::COMPARISON_FUNC_ALWAYS;
    ds.stencil_back_face.compare_mask    = buma3d::B3D_DEFAULT_STENCIL_COMPARE_MASK;
    ds.stencil_back_face.write_mask      = buma3d::B3D_DEFAULT_STENCIL_WRITE_MASK;
    ds.stencil_back_face.reference       = buma3d::B3D_DEFAULT_STENCIL_REFERENCE;

    depth_stencil_state[RENDER_PASS_TYPE_BASE] = ds;
    depth_stencil_state[RENDER_PASS_TYPE_BASE].is_enabled_depth_write = true;
    depth_stencil_state[RENDER_PASS_TYPE_BASE].depth_comparison_func  = buma3d::COMPARISON_FUNC_GREATER_EQUAL;

    depth_stencil_state[RENDER_PASS_TYPE_LIGHTING] = ds;
    depth_stencil_state[RENDER_PASS_TYPE_LIGHTING].is_enabled_depth_test  = false;
    depth_stencil_state[RENDER_PASS_TYPE_LIGHTING].is_enabled_depth_write = false;

    depth_stencil_state[RENDER_PASS_TYPE_TRANSLUCENT] = ds;
    depth_stencil_state[RENDER_PASS_TYPE_TRANSLUCENT].is_enabled_depth_test = true;
    depth_stencil_state[RENDER_PASS_TYPE_TRANSLUCENT].is_enabled_depth_write = false;
    depth_stencil_state[RENDER_PASS_TYPE_TRANSLUCENT].depth_comparison_func  = buma3d::COMPARISON_FUNC_GREATER_EQUAL;
}
void PipelineDescription::PrepareBlendState()
{
    // TODO: GBuffer数の決定
    blend_state[MATERIAL_BLEND_MODE_OPAQUE]     .SetNumAttachmemns(1).GetBlendDesc(0).BlendDisabled();
    blend_state[MATERIAL_BLEND_MODE_MASKED]     .SetNumAttachmemns(1).GetBlendDesc(0).BlendDisabled();
    blend_state[MATERIAL_BLEND_MODE_TRANSLUCENT].SetNumAttachmemns(1).GetBlendDesc(0).BlendPMA();
}
void PipelineDescription::PrepareDynamicState()
{
    dynamic_states = { buma3d::DYNAMIC_STATE_VIEWPORT, buma3d::DYNAMIC_STATE_SCISSOR };
    dynamic_state.num_dynamic_states = (uint32_t)dynamic_states.size();
    dynamic_state.dynamic_states     = dynamic_states.data();
}

const buma3d::INPUT_LAYOUT_DESC* PipelineDescription::GetInputLayout(RENDER_PASS_TYPE _type) const
{
    return &input_layout[_type]->Get();
}
const buma3d::VIEWPORT_STATE_DESC* PipelineDescription::GetViewportState() const
{
    return &viewport_state;
}
const buma3d::RASTERIZATION_STATE_DESC* PipelineDescription::GetRasterizationState() const
{
    return &rasterization_state;
}
const buma3d::MULTISAMPLE_STATE_DESC* PipelineDescription::GetMultisampleState() const
{
    return &multisample_state;
}
const buma3d::DEPTH_STENCIL_STATE_DESC* PipelineDescription::GetDepthStencilState(RENDER_PASS_TYPE _type) const
{
    return &depth_stencil_state[_type];
}
const buma3d::BLEND_STATE_DESC* PipelineDescription::GetBlendState(MATERIAL_BLEND_MODE _mode) const
{
    return &blend_state[_mode].Get();
}
const buma3d::DYNAMIC_STATE_DESC* PipelineDescription::GetDynamicState() const
{
    return &dynamic_state;
}


}// namespace draws
}// namespace buma
