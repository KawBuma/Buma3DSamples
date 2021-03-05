#include "pch.h"
#include "ParametersSignature.h"

namespace buma
{
namespace draws
{

ParametersSignature::ParametersSignature(DrawsInstance* _ins)
    : ins               { _ins }
    , pipeline_layout   {}
{

}

ParametersSignature::~ParametersSignature()
{

}

bool ParametersSignature::CreateSignature(const MATERIAL_PARAMETERS_LAYOUT& _layout)
{
    buma::util::PipelineLayoutDesc d;
    auto SetLayout = [&](uint32_t _space, DrawsMaterialParameters* _param)
    {
        if (_param)
            d.SetLayout(_space, _param->GetDescriptorSetLayout().Get());
        else
            d.SetLayout(_space, ins->GetRenderResource());
    };
    SetLayout(draws::REGISTER_SPACE_VALUE_PARAMETER  , static_cast<DrawsMaterialParameters*>(_layout.value_parameters));
    SetLayout(draws::REGISTER_SPACE_SAMPLER_PARAMETER, static_cast<DrawsMaterialParameters*>(_layout.sampler_parameters));
    SetLayout(draws::REGISTER_SPACE_TEXTURE_PARAMETER, static_cast<DrawsMaterialParameters*>(_layout.texture_parameters));
    d.SetFlags(buma3d::PIPELINE_LAYOUT_FLAG_NONE);
    d.Finalize();

    auto bmr = ins->GetDR()->GetDevice()->CreatePipelineLayout(d.Get(), &pipeline_layout);
    BMR_ASSERT(bmr);
    if (buma::util::IsFailed(bmr))
        return false;

    return true;
}


}// namespace draws
}// namespace buma
