#include "pch.h"
#include "MaterialShader.h"

namespace b = buma3d;

namespace buma
{
namespace draws
{

namespace /*anonymous*/
{

buma3d::SHADER_STAGE_FLAG GetShaderStage(buma::draws::SHADER_STAGE _type)
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


MaterialShader::MaterialShader(DrawsInstance* _ins)
    : ins       { _ins }
    , material  {}
    , name      {}
    , hlsl_code {}
    , type      {}
    , stage     {}
    , library   {}
{
}

MaterialShader::~MaterialShader()
{
}

bool MaterialShader::Init(DrawsMaterial* _material, const MATERIAL_SHADER& _shader)
{
    material  = _material;
    type      =  _shader.type;
    stage     = GetShaderStage(_shader.type);
    name      = _shader.name;
    hlsl_code = _shader.shader_hlsl;
    if (!CreateLibrary())
        return false;

    return true;
}

bool MaterialShader::CreateLibrary()
{
    shader::LOAD_SHADER_DESC desc{};
    desc.options.pack_matrices_in_row_major = false;       // Experimental: Decide how a matrix get packed
    desc.options.enable16bit_types          = false;       // Enable 16-bit types, such as half, uint16_t. Requires shader model 6.2+
    desc.options.enable_debug_info          = false;       // Embed debug info into the binary
    desc.options.disable_optimizations      = false;       // Force to turn off optimizations. Ignore optimizationLevel below.
    desc.options.optimization_level         = 3; // 0 to 3, no optimization to most optimization
    desc.options.shader_model               = { 6, 2 };
    desc.options.register_shifts            = &material->GetParametersRegisterShift();

    desc.entry_point = nullptr;
    desc.filename    = nullptr;
    desc.stage       = shader::SHADER_STAGE_LIBRARY;

    // マテリアルのライブラリ化
    auto&& loader = ins->GetDR()->GetShaderLoader();
    loader->LoadShaderFromHLSLString(desc, hlsl_code.c_str(), &library);
    assert(!library.empty());
    if (library.empty())
        return false;

    module_desc.library_name = name.c_str();
    module_desc.target       = &library;
    return true;
}


}// buma
}// draws
