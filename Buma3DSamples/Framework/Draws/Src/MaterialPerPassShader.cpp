#include "pch.h"
#include "MaterialPerPassShader.h"

namespace b = buma3d;

namespace buma
{
namespace draws
{

namespace /*anonymous*/
{

inline const char* GetShaderNameFromPassType(RENDER_PASS_TYPE _type)
{
    switch (_type)
    {
    case buma::draws::RENDER_PASS_TYPE_PRE_DEPTH : return "DrawsPrePass.hlsl";
    case buma::draws::RENDER_PASS_TYPE_BASE      : return "DrawsBasePass";
    case buma::draws::RENDER_PASS_TYPE_LIGHTING  : return "DrawsLighting";
    default:
        return "";
    }
}

inline const char* GetShaderTypeName(SHADER_TYPE _type)
{
    switch (_type)
    {
    case buma::draws::SHADER_TYPE_VS : "_VS.hlsl";
    case buma::draws::SHADER_TYPE_PS : "_PS.hlsl";
    default:
        return ".hlsl";
    }
}

inline std::string GetShaderFileName(RENDER_PASS_TYPE _pass_type, SHADER_TYPE _shader_type)
{
    return std::string(GetShaderNameFromPassType(_pass_type)) + GetShaderTypeName(_shader_type);
}


}// namespace /*anonymous*/

MaterialPerPassShader::MaterialPerPassShader(DrawsInstance* _ins)
    : ins{ _ins }
{

}

MaterialPerPassShader::~MaterialPerPassShader()
{
}

bool MaterialPerPassShader::Init(DrawsMaterial* _material, std::shared_ptr<MaterialShader>& _shader, RENDER_PASS_TYPE _pass_type)
{
    material  = _material;
    shader    = _shader;
    pass_type = _pass_type;

    b::BMRESULT bmr{};
    shader::LOAD_SHADER_DESC desc{};
    desc.options.packMatricesInRowMajor     = false;       // Experimental: Decide how a matrix get packed
    desc.options.enable16bitTypes           = false;       // Enable 16-bit types, such as half, uint16_t. Requires shader model 6.2+
    desc.options.enableDebugInfo            = false;       // Embed debug info into the binary
    desc.options.disableOptimizations       = false;       // Force to turn off optimizations. Ignore optimizationLevel below.

    desc.options.optimizationLevel          = 3; // 0 to 3, no optimization to most optimization
    desc.options.shaderModel                = { 6, 2 };

    desc.options.shiftAllTexturesBindings   = 1;// register(t0, space0) -> register(t1, space0)
    desc.options.shiftAllSamplersBindings   = 2;// register(s0, space0) -> register(s2, space0)
    desc.options.shiftAllCBuffersBindings   = 0;
    desc.options.shiftAllUABuffersBindings  = 0;

    auto dr = ins->GetDR();
    auto&& loader = dr->GetShaderLoader();
    auto path = ins->GetShaderPath(GetShaderFileName(pass_type, shader->GetType()).c_str());
    desc.entry_point    = "main";
    desc.filename       = path.c_str();

    shader->GetShaderCode();
    desc.defines = {};



    desc.stage          = { shader::SHADER_STAGE_VERTEX };


    std::vector<uint8_t> bytecode;
    loader->LoadShaderFromHLSL(desc, &bytecode);
    assert(!bytecode.empty());
    if (bytecode.empty())
        return false;

    b::SHADER_MODULE_DESC module_desc{};
    module_desc.flags                    = b::SHADER_MODULE_FLAG_NONE;
    module_desc.bytecode.bytecode_length = bytecode.size();
    module_desc.bytecode.shader_bytecode = bytecode.data();
    bmr = dr->GetDevice()->CreateShaderModule(module_desc, &shader_module);
    BMR_RET_IF_FAILED(bmr);


    return true;
}


}// buma
}// draws
