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
    auto&& s = _material->GetShaders();
    ins->GetRendererType();
    for (auto& i : s)
    {
        i->GetStage();

    }

    return true;
}

const MaterialPerPassShader* MaterialPerPassShadersMap::Get(SHADER_STAGE _stage, RENDER_PASS_TYPE _pass) const
{
    modules[_stage][_pass].get();
}


}// namespace draws
}// namespace buma
