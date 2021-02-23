#pragma once

namespace buma
{
namespace draws
{

class DrawsInstance;

class ParametersSignatureCache
{
public:
    using SameSigMaterials = std::vector<ScopedRef<DrawsMaterial>>;

public:
    ParametersSignatureCache(DrawsInstance* _ins);
    ~ParametersSignatureCache();

    const std::shared_ptr<ParametersSignature>& Request(DrawsMaterial* _material);

    const SameSigMaterials* GetSameSigMaterials(const std::shared_ptr<ParametersSignature>& _signature);
    const SameSigMaterials* GetSameSigMaterials(const MATERIAL_PARAMETERS_LAYOUT& _layout);

private:
    DrawsInstance*                                                                                          ins;
    std::unordered_map<MATERIAL_PARAMETERS_LAYOUT           , std::shared_ptr<ParametersSignature>>         signatures;
    std::unordered_map<std::shared_ptr<ParametersSignature> , std::shared_ptr<SameSigMaterials>>            same_sig_materials;

};


}// namespace draws
}// namespace buma
