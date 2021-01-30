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

inline shader::SHADER_STAGE GetShaderStageForCompile(buma::draws::SHADER_TYPE _type)
{
    switch (_type)
    {
    case buma::draws::SHADER_TYPE_VS: return shader::SHADER_STAGE_PIXEL;
    case buma::draws::SHADER_TYPE_PS: return shader::SHADER_STAGE_VERTEX;
    default:
        assert(false && __FUNCTION__);
        return shader::SHADER_STAGE(-1);
    }
}


}// namespace /*anonymous*/

MaterialPerPassShader::MaterialPerPassShader(DrawsInstance* _ins)
    : ins           { _ins }
    , material      {}
    , shader        {}
    , pass_type     {}
    , shader_module {}
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

    shader::LIBRARY_LINK_DESC desc{};
    desc.options.pack_matrices_in_row_major = false;       // Experimental: Decide how a matrix get packed
    desc.options.enable16bit_types          = false;       // Enable 16-bit types, such as half, uint16_t. Requires shader model 6.2+
    desc.options.enable_debug_info          = false;       // Embed debug info into the binary
    desc.options.disable_optimizations      = false;       // Force to turn off optimizations. Ignore optimizationLevel below.
    desc.options.optimization_level         = 3; // 0 to 3, no optimization to most optimization
    desc.options.shader_model               = { 6, 2 };
    desc.options.register_shifts            = &material->GetParametersRegisterShift();

    desc.link.modules.emplace_back(&ins->GetBaseBlendStateDesc());
    desc.link.modules.emplace_back(&shader->GetModuleDesc());
    desc.link.entry_point = "main";
    desc.link.stage       = GetShaderStageForCompile(shader->GetType());

    std::vector<uint8_t> bytecode;
    auto dr = ins->GetDR();
    dr->GetShaderLoader()->LinkLibrary(desc, &bytecode);
    assert(!bytecode.empty());
    if (bytecode.empty())
        return false;

    b::SHADER_MODULE_DESC module_desc{};
    module_desc.flags                    = b::SHADER_MODULE_FLAG_NONE;
    module_desc.bytecode.bytecode_length = bytecode.size();
    module_desc.bytecode.shader_bytecode = bytecode.data();
    auto bmr = dr->GetDevice()->CreateShaderModule(module_desc, &shader_module);
    BMR_RET_IF_FAILED(bmr);

    return true;
}


}// buma
}// draws
