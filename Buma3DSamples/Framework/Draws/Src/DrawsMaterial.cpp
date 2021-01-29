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


}// /*anonymous*/

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
    if (!CreateShaderModules(_desc.num_shaders, _desc.shaders)) return false;

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
    auto dr = ins->GetDR();

    // MaterialShader
    // MaterialPerPassShader
    // MaterialPerPassPipelines

    b::BMRESULT bmr{};
    shader_modules.resize(2);
    shader::LOAD_SHADER_DESC desc{};
    desc.options.packMatricesInRowMajor     = false;       // Experimental: Decide how a matrix get packed
    desc.options.enable16bitTypes           = false;       // Enable 16-bit types, such as half, uint16_t. Requires shader model 6.2+
    desc.options.enableDebugInfo            = false;       // Embed debug info into the binary
    desc.options.disableOptimizations       = false;       // Force to turn off optimizations. Ignore optimizationLevel below.

    desc.options.optimizationLevel          = 3; // 0 to 3, no optimization to most optimization
    desc.options.shaderModel                = { 6, 2 };

    desc.options.shiftAllTexturesBindings   = 1;// register(t0, space0) -> register(t1, space0)
    desc.options.shiftAllSamplersBindings   = 2;// register(s0, space0) -> register(s2, space0)
    desc.options.shiftAllCBuffersBindings   = 0;
    desc.options.shiftAllUABuffersBindings  = 0;

    auto&& loader = dr->GetShaderLoader();
    // vs
    {
        auto path = ins->GetShaderPath("Shader/VertexShader.hlsl");
        desc.entry_point    = "main";
        desc.filename       = path.c_str();
        desc.defines        = {};
        desc.stage          = { shader::SHADER_STAGE_VERTEX };
        std::vector<uint8_t> bytecode;
        loader->LoadShaderFromHLSL(desc, &bytecode);
        assert(!bytecode.empty());

        b::SHADER_MODULE_DESC module_desc{};
        module_desc.flags                    = b::SHADER_MODULE_FLAG_NONE;
        module_desc.bytecode.bytecode_length = bytecode.size();
        module_desc.bytecode.shader_bytecode = bytecode.data();
        bmr = device->CreateShaderModule(module_desc, &shader_modules[0]);
        BMR_ASSERT(bmr);
    }

    // ps
    {
        auto path = AssetPath("Shader/PixelShader.hlsl");
        desc.entry_point    = "main";
        desc.filename       = path.c_str();
        desc.defines        = {};
        desc.stage          = { shader::SHADER_STAGE_PIXEL };
        std::vector<uint8_t> bytecode;
        loader->LoadShaderFromHLSL(desc, &bytecode);
        assert(!bytecode.empty());

        b::SHADER_MODULE_DESC module_desc{};
        module_desc.flags                    = b::SHADER_MODULE_FLAG_NONE;
        module_desc.bytecode.bytecode_length = bytecode.size();
        module_desc.bytecode.shader_bytecode = bytecode.data();
        bmr = device->CreateShaderModule(module_desc, &shader_modules[1]);
        BMR_ASSERT(bmr);
    }

    return true;
}

bool DrawsMaterial::CreateGraphicsPipelines()
{
    auto dr = ins->GetDR();

    b::BMRESULT bmr{};
    // グラフィックスパイプラインの作成
    {
        b::GRAPHICS_PIPELINE_STATE_DESC pso_desc{};
        b::PIPELINE_SHADER_STAGE_DESC   shader_stages[2]{};
        b::INPUT_LAYOUT_DESC            input_layout{};
        b::INPUT_SLOT_DESC              input_slot{};
        b::INPUT_ELEMENT_DESC           input_elements[4]{};
        b::INPUT_ASSEMBLY_STATE_DESC    ia{};
        b::RASTERIZATION_STATE_DESC     rs{};
        b::MULTISAMPLE_STATE_DESC       ms{};
        b::DEPTH_STENCIL_STATE_DESC     ds{};
        b::BLEND_STATE_DESC             bs{};
        b::RENDER_TARGET_BLEND_DESC     attachments{};

        pso_desc.root_signature       = signature->GetSignature().Get();
        pso_desc.render_pass          = ins->GetBaseRenderPass();
        pso_desc.subpass              = 0;
        pso_desc.node_mask            = b::B3D_DEFAULT_NODE_MASK;
        pso_desc.flags                = b::PIPELINE_STATE_FLAG_NONE;

        {
            shader_stages[0].stage            = b::SHADER_STAGE_FLAG_VERTEX;
            shader_stages[0].entry_point_name = "main";
            shader_stages[0].flags            = b::PIPELINE_SHADER_STAGE_FLAG_NONE;
            shader_stages[0].module           = shader_modules[0].Get();

            shader_stages[1].stage            = b::SHADER_STAGE_FLAG_PIXEL;
            shader_stages[1].entry_point_name = "main";
            shader_stages[1].flags            = b::PIPELINE_SHADER_STAGE_FLAG_NONE;
            shader_stages[1].module           = shader_modules[1].Get();

            pso_desc.num_shader_stages    = 2;
            pso_desc.shader_stages        = shader_stages;
        }

        pso_desc.input_layout = ins->GetBaseInputLayoutDesc();

        {
            if ((ia.topology = ConvertPrimitiveTopology(topology)) == -1) return false;
            pso_desc.input_assembly_state = &ia;
        }

        pso_desc.tessellation_state = nullptr;

        {
            rs.fill_mode                        = is_wireframe ? b::FILL_MODE_WIREFRAME : b::FILL_MODE_SOLID;
            if ((rs.cull_mode = ConvertCullingMode(culling_mode)) == -1) return false;
            rs.is_front_counter_clockwise       = false;
            rs.is_enabled_depth_clip            = true;
            rs.is_enabled_depth_bias            = false;
            rs.depth_bias_scale                 = 0;
            rs.depth_bias_clamp                 = 0.f;
            rs.depth_bias_slope_scale           = 0.f;
            rs.is_enabled_conservative_raster   = false;
            rs.line_rasterization_mode          = b::LINE_RASTERIZATION_MODE_DEFAULT;
            rs.line_width                       = 1.f;
            
            pso_desc.rasterization_state  = &rs;
        }

        pso_desc.stream_output = nullptr;

        {
            ms.is_enabled_alpha_to_coverage     = blend_mode == MATERIAL_BLEND_MODE_MASKED;
            ms.is_enabled_sample_rate_shading   = false;
            ms.rasterization_samples            = 1;
            ms.sample_masks                     = b::B3D_DEFAULT_SAMPLE_MASK;
            ms.sample_position_state.is_enabled = false;
            ms.sample_position_state.desc       = nullptr;

            pso_desc.multisample_state = &ms;
        }

        {
            ds.is_enabled_depth_test        = true;
            ds.is_enabled_depth_write       = false;
            ds.depth_comparison_func        = b::COMPARISON_FUNC_GREATER_EQUAL;
            ds.is_enabled_depth_bounds_test = false;
            ds.min_depth_bounds             = 0;
            ds.max_depth_bounds             = 1;

            ds.is_enabled_stencil_test            = false;
            ds.stencil_front_face.fail_op         = b::STENCIL_OP_KEEP;
            ds.stencil_front_face.depth_fail_op   = b::STENCIL_OP_KEEP;
            ds.stencil_front_face.pass_op         = b::STENCIL_OP_REPLACE;
            ds.stencil_front_face.comparison_func = b::COMPARISON_FUNC_ALWAYS;
            ds.stencil_front_face.compare_mask    = b::B3D_DEFAULT_STENCIL_COMPARE_MASK;
            ds.stencil_front_face.write_mask      = b::B3D_DEFAULT_STENCIL_WRITE_MASK;
            ds.stencil_front_face.reference       = b::B3D_DEFAULT_STENCIL_REFERENCE;
            
            ds.stencil_back_face.fail_op         = b::STENCIL_OP_KEEP;
            ds.stencil_back_face.depth_fail_op   = b::STENCIL_OP_KEEP;
            ds.stencil_back_face.pass_op         = b::STENCIL_OP_REPLACE;
            ds.stencil_back_face.comparison_func = b::COMPARISON_FUNC_ALWAYS;
            ds.stencil_back_face.compare_mask    = b::B3D_DEFAULT_STENCIL_COMPARE_MASK;
            ds.stencil_back_face.write_mask      = b::B3D_DEFAULT_STENCIL_WRITE_MASK;
            ds.stencil_back_face.reference       = b::B3D_DEFAULT_STENCIL_REFERENCE;
            
            pso_desc.depth_stencil_state = &ds;
        }

        pso_desc.blend_state = ins->GetBaseBlendStateDesc(blend_mode);

        b::VIEWPORT_STATE_DESC vp{};
        {
            vp.num_viewports        = 1;
            vp.num_scissor_rects    = 1;
            vp.viewports            = nullptr;
            vp.scissor_rects        = nullptr;

            pso_desc.viewport_state = &vp;
        }

        b::DYNAMIC_STATE_DESC   dynamic_state_desc{};
        b::DYNAMIC_STATE        dynamic_states[] = { b::DYNAMIC_STATE_VIEWPORT, b::DYNAMIC_STATE_SCISSOR };
        {
            dynamic_state_desc.num_dynamic_states = _countof(dynamic_states);
            dynamic_state_desc.dynamic_states     = dynamic_states;
            pso_desc.dynamic_state                = &dynamic_state_desc;
        }

        bmr = dr->GetDevice()->CreateGraphicsPipelineState(pso_desc, &pipeline);
        BMR_ASSERT(bmr);
    }

    return true;
}

void DrawsMaterial::PrepareParametersRegisterShifts()
{
    register_shifts = { { REGISTER_SPACE_DRAWS_RESERVED }, { REGISTER_SPACE_TEXTURE_PARAMETER } };

    auto&& s = register_shifts[1];
    s.shift_tex_bindings;
    s.shift_cbuf_bindings;
    s.shift_cbuf_bindings;
    s.shift_cbuf_bindings;
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
