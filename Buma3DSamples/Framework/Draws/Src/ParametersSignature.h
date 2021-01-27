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

    const buma3d::util::Ptr<buma3d::IRootSignature>& GetSignature() const { return signature; }

private:
    DrawsInstance*                              ins;
    buma3d::util::Ptr<buma3d::IRootSignature>   signature;

};


}// namespace draws
}// namespace buma
