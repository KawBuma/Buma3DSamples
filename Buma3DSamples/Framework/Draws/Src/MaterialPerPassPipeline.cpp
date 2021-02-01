#include "pch.h"
#include "MaterialPerPassPipeline.h"

namespace b = buma3d;

namespace buma
{
namespace draws
{

namespace /*anonymous*/
{

inline buma3d::SHADER_STAGE_FLAG GetB3DShaderStage(buma::draws::SHADER_STAGE _type)
{
    switch (_type)
    {
    case buma::draws::SHADER_STAGE_VS: return buma3d::SHADER_STAGE_FLAG_PIXEL;
    case buma::draws::SHADER_STAGE_PS: return buma3d::SHADER_STAGE_FLAG_VERTEX;
    default:
        assert(false && __FUNCTION__);
        return buma3d::SHADER_STAGE_FLAG_UNKNOWN;
    }
}


}// namespace /*anonymous*/

MaterialPerPassPipeline::MaterialPerPassPipeline(DrawsInstance* _ins)
    : ins{ _ins }
{
}

MaterialPerPassPipeline::~MaterialPerPassPipeline()
{
}

bool MaterialPerPassPipeline::Init(DrawsMaterial* _material, RENDER_PASS_TYPE _pass_type)
{
    material  = _material;
    pass_type = _pass_type;

    b::BMRESULT bmr{};
    // グラフィックスパイプラインの作成
    {
        b::GRAPHICS_PIPELINE_STATE_DESC pso_desc = material->GetPsoDesc();

        // DrawsInstance
        //b::INPUT_LAYOUT_DESC            input_layout{};
        //b::INPUT_SLOT_DESC              input_slot{};
        //b::INPUT_ELEMENT_DESC           input_elements{};

        // material
        //b::DEPTH_STENCIL_STATE_DESC   ds{};
        //b::RASTERIZATION_STATE_DESC     rs{};
        //b::INPUT_ASSEMBLY_STATE_DESC    ia{};
        //b::MULTISAMPLE_STATE_DESC       ms{};
        //b::BLEND_STATE_DESC             bs{};
        //b::RENDER_TARGET_BLEND_DESC     attachments{};

        pso_desc.render_pass = ins->GetRenderPass(_pass_type);

        util::PipelineShaderStageDescs shader_stages;
        {
            auto&& per_pass = material->GetPerPassShadersMap().GetPerPass(_pass_type);
            for (auto& i : per_pass->GetShaders())
            {
                shader_stages.AddStage(GetB3DShaderStage(i->GetStage()), i->GetShaderModule().Get(), "main");
            }
            pso_desc.num_shader_stages  = shader_stages.GetSize();
            pso_desc.shader_stages      = shader_stages.Get();
        }

        pso_desc.input_layout        = ins->GetInputLayoutDesc(_pass_type);
        pso_desc.depth_stencil_state = ins->GetDepthStencilStateDesc(pass_type);

        bmr = ins->GetDR()->GetDevice()->CreateGraphicsPipelineState(pso_desc, &pipeline);
        BMR_RET_IF_FAILED(bmr);
    }

    return true;
}


}// namespace draws
}// namespace buma
