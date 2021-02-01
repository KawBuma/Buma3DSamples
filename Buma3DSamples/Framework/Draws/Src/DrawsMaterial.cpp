#include "pch.h"
#include "DrawsMaterial.h"

namespace b = buma3d;

namespace buma
{
namespace draws
{

namespace /*anonymous*/
{

inline buma3d::PRIMITIVE_TOPOLOGY ConvertPrimitiveTopology(PRIMITIVE_TOPOLOGY _topology)
{
    switch (_topology)
    {
    case buma::draws::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST : return buma3d::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    case buma::draws::PRIMITIVE_TOPOLOGY_LINE_LIST     : return buma3d::PRIMITIVE_TOPOLOGY_LINE_LIST;

    default:
        return buma3d::PRIMITIVE_TOPOLOGY(-1);
    }
}

inline buma3d::CULL_MODE ConvertCullingMode(CULLING_MODE _mode)
{
    switch (_mode)
    {
    case buma::draws::CULLING_MODE_NONE  : return buma3d::CULL_MODE_NONE;
    case buma::draws::CULLING_MODE_FRONT : return buma3d::CULL_MODE_FRONT;
    case buma::draws::CULLING_MODE_BACK  : return buma3d::CULL_MODE_BACK;

    default:
        return buma3d::CULL_MODE(-1);
    }
}


}// namespace /*anonymous*/


DrawsMaterial::DrawsMaterial()
    : ref_count { 1 }
    , ins       {}
{

}

DrawsMaterial::~DrawsMaterial()
{
    util::SafeRelease(layout.value_parameters);
    util::SafeRelease(layout.sampler_parameters);
    util::SafeRelease(layout.texture_parameters);
}

bool DrawsMaterial::Init(DrawsInstance* _ins, const MATERIAL_CREATE_DESC& _desc)
{
    ins = _ins;

    CopyDesc(_desc);
    PrepareParametersRegisterShifts();
    if (!RequestParametersSignature())                          return false;
    if (!CreateShaderModules(_desc.num_shaders, _desc.shaders)) return false;
    if (!CreateGraphicsPipelines())                             return false;

    return true;
}

void DrawsMaterial::CopyDesc(const buma::draws::MATERIAL_CREATE_DESC& _desc)
{
    is_wireframe  = _desc.is_wireframe;
    topology      = _desc.topology;
    culling_mode  = _desc.culling_mode;
    blend_mode    = _desc.blend_mode;
    shading_model = _desc.shading_model;
    layout        = _desc.layout;
    util::SafeAddRef(layout.value_parameters);
    util::SafeAddRef(layout.sampler_parameters);
    util::SafeAddRef(layout.texture_parameters);
}

bool DrawsMaterial::RequestParametersSignature()
{
    signature = ins->GetParametersSignatureCache().Request(this);
    if (!signature)
        return false;
    return true;
}

bool DrawsMaterial::CreateShaderModules(uint32_t _num_shaders, const MATERIAL_SHADER* _shaders)
{
    shaders.reserve(_num_shaders);
    for (uint32_t i = 0; i < _num_shaders; i++)
    {
        auto&& s = shaders.emplace_back(std::make_shared<MaterialShader>(ins));
        if (!s->Init(this, _shaders[i]))
            return false;
    }

    return true;
}

bool DrawsMaterial::CreatePerPassShaders()
{
    per_pass_shaders_map = std::make_unique<MaterialPerPassShadersMap>(ins);
    if (!per_pass_shaders_map->Init(this))
        return false;
    return true;
}

bool DrawsMaterial::PrepareGraphicsPipelineDesc()
{
    pso_desc.root_signature       = signature->GetSignature().Get();
    pso_desc.render_pass          = nullptr; // MaterialPerPassPipelineによって設定します。
    pso_desc.subpass              = 0;
    pso_desc.node_mask            = b::B3D_DEFAULT_NODE_MASK;
    pso_desc.flags                = b::PIPELINE_STATE_FLAG_NONE;

    pso_desc.input_assembly_state = &input_assembly;
    if ((input_assembly.topology = ConvertPrimitiveTopology(topology)) == -1) return false;

    pso_desc.tessellation_state = nullptr;

    pso_desc.rasterization_state = &rasterization_state;
    rasterization_state.fill_mode                       = is_wireframe ? b::FILL_MODE_WIREFRAME : b::FILL_MODE_SOLID;
    if ((rasterization_state.cull_mode                  = ConvertCullingMode(culling_mode)) == -1) return false;
    rasterization_state.is_front_counter_clockwise      = false;
    rasterization_state.is_enabled_depth_clip           = true;
    rasterization_state.is_enabled_depth_bias           = false;
    rasterization_state.depth_bias_scale                = 0;
    rasterization_state.depth_bias_clamp                = 0.f;
    rasterization_state.depth_bias_slope_scale          = 0.f;
    rasterization_state.is_enabled_conservative_raster  = false;
    rasterization_state.line_rasterization_mode         = b::LINE_RASTERIZATION_MODE_DEFAULT;
    rasterization_state.line_width                      = 1.f;

    pso_desc.stream_output = nullptr;

    pso_desc.multisample_state = ins->GetMultisampleStateDesc(blend_mode);
    //ms.is_enabled_alpha_to_coverage     = blend_mode == MATERIAL_BLEND_MODE_MASKED;
    //ms.is_enabled_sample_rate_shading   = false;
    //ms.rasterization_samples            = 1;
    //ms.sample_masks                     = b::B3D_DEFAULT_SAMPLE_MASK;
    //ms.sample_position_state.is_enabled = false;
    //ms.sample_position_state.desc       = nullptr;

    // {
    //     ds.is_enabled_depth_test        = true;
    //     ds.is_enabled_depth_write       = false;
    //     ds.depth_comparison_func        = b::COMPARISON_FUNC_GREATER_EQUAL;
    //     ds.is_enabled_depth_bounds_test = false;
    //     ds.min_depth_bounds             = 0;
    //     ds.max_depth_bounds             = 1;
    // 
    //     ds.is_enabled_stencil_test            = false;
    //     ds.stencil_front_face.fail_op         = b::STENCIL_OP_KEEP;
    //     ds.stencil_front_face.depth_fail_op   = b::STENCIL_OP_KEEP;
    //     ds.stencil_front_face.pass_op         = b::STENCIL_OP_REPLACE;
    //     ds.stencil_front_face.comparison_func = b::COMPARISON_FUNC_ALWAYS;
    //     ds.stencil_front_face.compare_mask    = b::B3D_DEFAULT_STENCIL_COMPARE_MASK;
    //     ds.stencil_front_face.write_mask      = b::B3D_DEFAULT_STENCIL_WRITE_MASK;
    //     ds.stencil_front_face.reference       = b::B3D_DEFAULT_STENCIL_REFERENCE;
    //     
    //     ds.stencil_back_face.fail_op         = b::STENCIL_OP_KEEP;
    //     ds.stencil_back_face.depth_fail_op   = b::STENCIL_OP_KEEP;
    //     ds.stencil_back_face.pass_op         = b::STENCIL_OP_REPLACE;
    //     ds.stencil_back_face.comparison_func = b::COMPARISON_FUNC_ALWAYS;
    //     ds.stencil_back_face.compare_mask    = b::B3D_DEFAULT_STENCIL_COMPARE_MASK;
    //     ds.stencil_back_face.write_mask      = b::B3D_DEFAULT_STENCIL_WRITE_MASK;
    //     ds.stencil_back_face.reference       = b::B3D_DEFAULT_STENCIL_REFERENCE;
    //     
    // }

    pso_desc.blend_state = ins->GetBlendStateDesc(blend_mode);

    //b::VIEWPORT_STATE_DESC vp{};
    //{
    //    vp.num_viewports        = 1;
    //    vp.num_scissor_rects    = 1;
    //    vp.viewports            = nullptr;
    //    vp.scissor_rects        = nullptr;
    //
    //}
    //
    //b::DYNAMIC_STATE_DESC   dynamic_state_desc{};
    //b::DYNAMIC_STATE        dynamic_states[] = { b::DYNAMIC_STATE_VIEWPORT, b::DYNAMIC_STATE_SCISSOR };
    //{
    //    dynamic_state_desc.num_dynamic_states = _countof(dynamic_states);
    //    dynamic_state_desc.dynamic_states     = dynamic_states;
    //    pso_desc.dynamic_state                = &dynamic_state_desc;
    //}

    pso_desc.viewport_state = ins->GetDefaultViewportStateDesc();
    pso_desc.dynamic_state = ins->GetDefaultDynamicStateDesc();

    return true;
}

bool DrawsMaterial::CreateGraphicsPipelines()
{
    bool result = false;
    switch (ins->GetRendererType())
    {
    case buma::draws::RENDERER_TYPE_DEFERRED:
        result = CreateForDeferredRenderingPipelines();
        break;

    default:
        break;
    }
    return result;
}

bool DrawsMaterial::CreateForDeferredRenderingPipelines()
{
    if (blend_mode == buma::draws::MATERIAL_BLEND_MODE_TRANSLUCENT)
    {
        if (!(per_pass_pipelines[RENDER_PASS_TYPE_TRANSLUCENT] = std::make_unique<MaterialPerPassPipeline>(ins))->Init(this, RENDER_PASS_TYPE_TRANSLUCENT)) return false;
    }
    else
    {
        if (!(per_pass_pipelines[RENDER_PASS_TYPE_PRE_DEPTH] = std::make_unique<MaterialPerPassPipeline>(ins))->Init(this, RENDER_PASS_TYPE_PRE_DEPTH)) return false;
        if (!(per_pass_pipelines[RENDER_PASS_TYPE_BASE]      = std::make_unique<MaterialPerPassPipeline>(ins))->Init(this, RENDER_PASS_TYPE_BASE))      return false;
    }
    return true;
}

void DrawsMaterial::PrepareParametersRegisterShifts()
{
    register_shifts = { ins->GetReservedRegisterShift() };
}

bool DrawsMaterial::Create(DrawsInstance* _ins, const MATERIAL_CREATE_DESC& _desc, IDrawsMaterial** _dst)
{
    auto p = new DrawsMaterial();
    if (!p->Init(_ins, _desc))
    {
        p->Release();
        return false;
    }

    *_dst = p;
    return true;
}

void DrawsMaterial::AddRef()
{
    ++ref_count;
}

uint32_t DrawsMaterial::Release()
{
    uint32_t result = --ref_count;
    if (result == 0)
        delete this;
    return result;
}

bool DrawsMaterial::CreateMaterialConstant(const MATERIAL_CONSTANT_CREATE_DESC& _desc, IDrawsMaterialConstant** _dst)
{

    return true;
}

bool DrawsMaterial::CreateMaterialInstance(const MATERIAL_INSTANCE_CREATE_DESC& _desc, IDrawsMaterialInstance** _dst)
{

    return true;
}


}// namespace buma
}// namespace draws
