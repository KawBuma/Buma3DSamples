#pragma once

namespace buma
{
namespace draws
{

class DrawsInstance;

class ParametersSignature
{
public:
    ParametersSignature(DrawsInstance* _ins);
    bool CreateSignature(const MATERIAL_PARAMETERS_LAYOUT& _layout);
    ~ParametersSignature();

    const buma3d::util::Ptr<buma3d::IPipelineLayout>& GetSignature() const { return pipeline_layout; }

private:
    DrawsInstance*                              ins;
    buma3d::util::Ptr<buma3d::IPipelineLayout>  pipeline_layout;

};


}// namespace draws
}// namespace buma
