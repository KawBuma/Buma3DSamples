#include "pch.h"
#include "MaterialShader.h"

namespace buma
{
namespace draws
{

namespace /*anonymous*/
{

buma3d::SHADER_STAGE_FLAG GetShaderStage(buma::draws::SHADER_TYPE _type)
{
    switch (_type)
    {
    case buma::draws::SHADER_TYPE_VS: return buma3d::SHADER_STAGE_FLAG_PIXEL;
    case buma::draws::SHADER_TYPE_PS: return buma3d::SHADER_STAGE_FLAG_VERTEX;
    default:
        assert(false && __FUNCTION__);
        return buma3d::SHADER_STAGE_FLAG_UNKNOWN;
    }
}

}// namespace /*anonymous*/


MaterialShader::MaterialShader()
    : hlsl_code {}
    , type      {}
    , stage     {}
{
}

MaterialShader::~MaterialShader()
{
}

void MaterialShader::Init(SHADER_TYPE _type, const char* _hlsl_code)
{
    type = _type;

    stage = GetShaderStage(_type);
    hlsl_code = _hlsl_code;
}


}// buma
}// draws
