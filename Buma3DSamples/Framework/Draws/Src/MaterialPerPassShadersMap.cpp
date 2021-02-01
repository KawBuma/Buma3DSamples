#include "pch.h"
#include "MaterialPerPassShadersMap.h"

namespace buma
{
namespace draws
{

MaterialPerPassShadersMap::MaterialPerPassShadersMap(DrawsInstance* _ins)
    : ins       { _ins }
    , modules   {}
{

}

MaterialPerPassShadersMap::~MaterialPerPassShadersMap()
{
}

bool MaterialPerPassShadersMap::Init(DrawsMaterial* _material)
{
    switch (ins->GetRendererType())
    {
    case buma::draws::RENDERER_TYPE_DEFERRED:
        if (!CreateForDeferedRenderingShaders(_material)) return false;

    default: // TODO: deferred renderingを優先
        break;
    }

    return true;
}

bool MaterialPerPassShadersMap::CreateForDeferedRenderingShaders(buma::draws::DrawsMaterial* _material)
{
    // _materialが持つユーザー定義のマテリアル関数とDrawsのコアライブラリをリンクします。
    auto blend_mode = _material->GetBlendMode();
    for (auto& i : _material->GetShaders())
    {
        if (_material->GetBlendMode() == buma::draws::MATERIAL_BLEND_MODE_TRANSLUCENT)
        {
            auto&& t = modules[RENDER_PASS_TYPE_TRANSLUCENT].shaders.emplace_back(std::make_shared<MaterialPerPassShader>(ins));
            if (!t->Init(_material, i->GetStage(), i, RENDER_PASS_TYPE_TRANSLUCENT)) return false;
        }
        else
        {
            // NOTE: SHADER_STAGE_PSにはMask関数とMaterial関数が存在します。
            // PRE_DEPTHはMask, BASEはMaterialの様にそれぞれ片方のみを使用します。
            if (blend_mode == MATERIAL_BLEND_MODE_OPAQUE && i->GetStage() == SHADER_STAGE_PS)
            {
                /* ただし、Opaqueマテリアルの場合、PRE_DEPTH時のピクセルシェーダーはありません。*/
            }
            else 
            {
                auto&& mask = modules[RENDER_PASS_TYPE_PRE_DEPTH].shaders.emplace_back(std::make_shared<MaterialPerPassShader>(ins));
                if (!mask->Init(_material, i->GetStage(), i, RENDER_PASS_TYPE_PRE_DEPTH)) return false;
            }
            auto&& base = modules[RENDER_PASS_TYPE_BASE].shaders.emplace_back(std::make_shared<MaterialPerPassShader>(ins));
            if (!base->Init(_material, i->GetStage(), i, RENDER_PASS_TYPE_BASE)) return false;
        }
    }
}

const ShaderPerStages* MaterialPerPassShadersMap::GetPerPass(RENDER_PASS_TYPE _pass_type) const
{
    auto&& f = modules.find(_pass_type);
    return f == modules.end() ? &f->second : nullptr;
}


}// namespace draws
}// namespace buma
