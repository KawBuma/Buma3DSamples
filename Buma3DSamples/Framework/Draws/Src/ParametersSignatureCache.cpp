#include "pch.h"
#include "ParametersSignatureCache.h"

namespace buma
{
namespace draws
{

ParametersSignatureCache::ParametersSignatureCache(DrawsInstance* _ins)
    : ins{ _ins }
{

}

ParametersSignatureCache::~ParametersSignatureCache()
{
}

const std::shared_ptr<ParametersSignature>& ParametersSignatureCache::Request(DrawsMaterial* _material)
{
    auto&& sig = signatures[_material->GetParametersLayout()];
    if (!sig)
    {
        sig = std::make_shared<ParametersSignature>(ins);
        auto res = sig->CreateSignature(_material->GetParametersLayout());
        assert(res);
        if (!res)
            return sig;
    }

    // 新規マテリアルとして追加
    auto&& m = same_sig_materials[sig];
    if (!m) m = std::make_shared<SameSigMaterials>();
    assert(std::find(m->begin(), m->end(), _material) == m->end());
    m->emplace_back(_material);

    return sig;
}

const ParametersSignatureCache::SameSigMaterials* ParametersSignatureCache::GetSameSigMaterials(const std::shared_ptr<ParametersSignature>& _signature)
{
    auto find_mats = same_sig_materials.find(_signature);
    if (find_mats == same_sig_materials.end())
        return nullptr;

    return find_mats->second.get();
}

const ParametersSignatureCache::SameSigMaterials* ParametersSignatureCache::GetSameSigMaterials(const MATERIAL_PARAMETERS_LAYOUT& _layout)
{
    auto find_sig = signatures.find(_layout);
    if (find_sig == signatures.end())
        return nullptr;

    return GetSameSigMaterials(find_sig->second);
}


}// namespace draws
}// namespace buma
