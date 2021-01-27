#include "pch.h"
#include "ParametersSignature.h"

namespace buma
{
namespace draws
{

ParametersSignature::ParametersSignature(DrawsInstance* _ins)
    : ins       { _ins }
    , signature {}
{

}

ParametersSignature::~ParametersSignature()
{

}

bool ParametersSignature::CreateSignature(const MATERIAL_PARAMETERS_LAYOUT& _layout)
{
    buma::util::RootSignatureDesc d;
    if (_layout.value_parameters)   d.AddRootParameter(static_cast<DrawsMaterialParameters*>(_layout.value_parameters)->GetRootParameter());
    if (_layout.sampler_parameters) d.AddRootParameter(static_cast<DrawsMaterialParameters*>(_layout.sampler_parameters)->GetRootParameter());
    if (_layout.texture_parameters) d.AddRootParameter(static_cast<DrawsMaterialParameters*>(_layout.texture_parameters)->GetRootParameter());
    auto bmr = ins->GetDR()->GetDevice()->CreateRootSignature(d.Get(buma3d::ROOT_SIGNATURE_FLAG_NONE), &signature);
    BMR_ASSERT(bmr);
    if (buma::util::IsFailed(bmr))
        return false;
    return true;
}


}// namespace draws
}// namespace buma
